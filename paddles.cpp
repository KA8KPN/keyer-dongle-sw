#include <Arduino.h>

#include "paddles.h"
#include "wpm.h"
#include "serial.h"

paddles system_paddles;

void paddles_initialize(byte right_paddle, byte left_paddle)
{
    system_paddles.set_paddle_ports(right_paddle, left_paddle);
    pinMode(left_paddle, INPUT);
    digitalWrite(left_paddle, HIGH);
    pinMode(right_paddle, INPUT);
    digitalWrite(right_paddle, HIGH);
}

paddles::paddles(void) : m_leftPaddle(0), m_rightPaddle(0), m_paddleMode(MODE_PADDLE_NORMAL) {
    m_nextStateTransitionMs = 100 + millis();
    m_startReadingPaddlesMs = 0;
    m_keyerState = KEY_UP_LONGER;
    m_lastKeyerState = KEY_UP_LONGER;
    m_ditClosed = false;
    m_dahClosed = false;
    m_msTwitchTimer = 0;
    m_twitchCount = 0;
}


void paddles::set_paddle_ports(byte right_paddle, byte left_paddle) {
    m_leftPaddle = left_paddle;
    m_rightPaddle = right_paddle;
    m_ditPaddle = m_leftPaddle;
    m_dahPaddle = m_rightPaddle;
}

input_mode_t paddles::update(unsigned long now, input_mode_t input_mode) {
    if (now >= m_startReadingPaddlesMs) {
	m_dahClosed = m_dahClosed || (0 == digitalRead(m_dahPaddle));
	m_ditClosed = m_ditClosed || (0 == digitalRead(m_ditPaddle));
    }

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
		system_serial->key_up(m_twitchCount);
		system_serial->key_down(TWITCHES_PER_DOT);
		ptt_delay = TRANSMITTER_KEY_DOWN();
		m_nextStateTransitionMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		m_startReadingPaddlesMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		m_ditClosed = false;
		m_dahClosed = false;
		break;

	    case KEY_DAH:
		system_serial->key_up(m_twitchCount);
		system_serial->key_down(TWITCHES_PER_DASH);
		ptt_delay = TRANSMITTER_KEY_DOWN();
		m_nextStateTransitionMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		m_startReadingPaddlesMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		m_ditClosed = false;
		m_dahClosed = false;
		break;

	    case KEY_UP:
		if (KEY_UP == m_lastKeyerState) {
		    input_mode = m_paddleMode;
		    if (100 <= m_twitchCount) {
			system_serial->key_up(m_twitchCount);
			m_twitchCount = 0;
			m_keyerState = KEY_UP_LONGER;
		    }
		}
		else {
		    m_twitchCount = 0;
		    TRANSMITTER_KEY_UP();
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


input_mode_t paddles::toggle_reverse(void) {
    if (MODE_PADDLE_NORMAL == m_paddleMode) {
	m_dahPaddle = m_leftPaddle;
	m_ditPaddle = m_rightPaddle;
	m_paddleMode = MODE_PADDLE_REVERSE;
    }
    else {
	m_ditPaddle = m_leftPaddle;
	m_dahPaddle = m_rightPaddle;
	m_paddleMode = MODE_PADDLE_NORMAL;
    }
    return m_paddleMode;
}
