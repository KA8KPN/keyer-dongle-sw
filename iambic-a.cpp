#include <Arduino.h>

#include "iambic-a.h"

#include "wpm.h"
#include "rotary_encoder.h"
#include "serial.h"

iambicA::~iambicA(void) {
}

iambicA::iambicA(void) {
    m_ditClosed = false;
    m_dahClosed = false;
    m_rightClosed = false;
    m_leftClosed = false;

    m_nextStateTransitionMs = 100 + millis();
    m_startReadingPaddlesMs = 0;
    m_keyerState = KEY_UP_LONGER;
    m_lastKeyerState = KEY_UP_LONGER;
    m_msTwitchTimer = 0;
    m_twitchCount = 0;
}

iambicA::iambicA(paddles const *old_config) {
    m_ditClosed = false;
    m_dahClosed = false;
    m_rightClosed = false;
    m_leftClosed = false;

    m_nextStateTransitionMs = 100 + millis();
    m_startReadingPaddlesMs = 0;
    m_keyerState = KEY_UP_LONGER;
    m_lastKeyerState = KEY_UP_LONGER;
    m_msTwitchTimer = 0;
    m_twitchCount = 0;

    m_leftPaddle = old_config->m_leftPaddle;
    m_rightPaddle = old_config->m_rightPaddle;
    m_ditPaddle = old_config->m_ditPaddle;
    m_dahPaddle = old_config->m_dahPaddle;
    m_transmitter = old_config->m_transmitter;
    m_paddleMode = old_config->m_paddleMode;
}

input_mode_t iambicA::update(unsigned long now, input_mode_t input_mode) {
    if (now >= m_startReadingPaddlesMs) {
	m_dahClosed = m_dahClosed || (0 == digitalRead(m_dahPaddle));
	m_ditClosed = m_ditClosed || (0 == digitalRead(m_ditPaddle));
    }

    bool closed = (0 == digitalRead(m_rightPaddle));
    if (!m_rightClosed && closed) {
	CONTACT_CLOSED(m_transmitter, true);
    }
    m_rightClosed = closed;

    closed = (0 == digitalRead(m_leftPaddle));
    if (!m_leftClosed && closed) {
	CONTACT_CLOSED(m_transmitter, false);
    }
    m_leftClosed = closed;

    if (m_msTwitchTimer <= now) {
	m_msTwitchTimer = now + WPM_TWITCHES();
	if (m_twitchCount < 10000) {
	    ++m_twitchCount;
	}
    }

    if ((MODE_PADDLE_NORMAL == input_mode) || (MODE_PADDLE_REVERSE == input_mode)) {
	if (now >= m_nextStateTransitionMs) {
	    unsigned ptt_delay;
	    keyer_state_t tempKeyerState;
 
	    tempKeyerState = m_keyerState;
	    switch(m_keyerState) {
	    case KEY_DIT:
	    case KEY_DAH:
		m_keyerState = KEY_UP;
		break;

	    case KEY_UP:
	    case KEY_UP_LONGER:
		// I ought to be able to build this into a table and make it table driven
		switch(m_lastKeyerState) {
		case KEY_DIT:
		    if (m_dahClosed) {
			m_keyerState = KEY_DAH;
		    }
		    else if (m_ditClosed) {
			m_keyerState = KEY_DIT;
		    }
		    break;

		case KEY_DAH:
		default:
		    if (m_ditClosed) {
			m_keyerState = KEY_DIT;
		    }
		    else if (m_dahClosed) {
			m_keyerState = KEY_DAH;
		    }
		    break;
		}
		break;
	    }
	    m_lastKeyerState = tempKeyerState;
	    switch(m_keyerState) {
	    case KEY_DIT:
		KEY_UP(m_transmitter, m_twitchCount);
		KEY_DOWN(m_transmitter, TWITCHES_PER_DOT);
		ptt_delay = TRANSMITTER_KEY_DOWN(m_transmitter);
		m_nextStateTransitionMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		m_startReadingPaddlesMs = now + WPM_DOT_TWITCHES() + WPM_DOT_TWITCHES()/4 + ptt_delay;
		m_ditClosed = false;
		m_dahClosed = false;
		break;

	    case KEY_DAH:
		KEY_UP(m_transmitter, m_twitchCount);
		KEY_DOWN(m_transmitter, TWITCHES_PER_DASH);
		ptt_delay = TRANSMITTER_KEY_DOWN(m_transmitter);
		m_nextStateTransitionMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		m_startReadingPaddlesMs = now + WPM_DASH_TWITCHES() + WPM_DOT_TWITCHES()/4 + ptt_delay;
		m_ditClosed = false;
		m_dahClosed = false;
		break;

	    case KEY_UP:
		if (KEY_UP == m_lastKeyerState) {
		    input_mode = m_paddleMode;
		    if (100 <= m_twitchCount) {
			KEY_UP(m_transmitter, m_twitchCount);
			m_twitchCount = 0;
			m_keyerState = KEY_UP_LONGER;
		    }
		}
		else {
		    m_twitchCount = 0;
		    TRANSMITTER_KEY_UP(m_transmitter);
		    m_nextStateTransitionMs = now + WPM_DOT_TWITCHES();
		}
		break;

	    default:
		break;
	    }
	}
    }
    return input_mode;
}

