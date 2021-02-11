#include <Arduino.h>

#include "bug.h"

#include "wpm.h"
#include "serial.h"

bug::bug(void) {
    m_msTwitchTimer = 0;
    m_twitchCount = 0;
    m_then = millis();
    m_wasKeyDown = false;
    m_wasSendingDit = false;
    m_nextStateTransitionMs = 0;
    m_dahClosed = false;
    m_ditClosed = false;
}

input_mode_t bug::update(unsigned long now, input_mode_t input_mode) {
    TRANSMITTER_SELECT(m_transmitter);
    m_ditClosed = (0 == digitalRead(m_ditPaddle));
    m_dahClosed = (0 == digitalRead(m_dahPaddle));

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

    m_msTwitchTimer += (now - m_then);
    m_then = now;
    while (m_msTwitchTimer > WPM_TWITCHES()) {
        m_msTwitchTimer -= WPM_TWITCHES();
	m_twitchCount++;
    }

    if ((MODE_PADDLE_NORMAL == input_mode) || (MODE_PADDLE_REVERSE == input_mode)) {
	if (m_dahClosed) {
	    m_nextStateTransitionMs = now;
	    m_wasSendingDit = false;
	    if (!m_wasKeyDown) {
		m_wasKeyDown = true;
		KEY_UP(m_transmitter, m_twitchCount);
		m_twitchCount = 0;
		m_msTwitchTimer = 0;
		TRANSMITTER_KEY_DOWN(m_transmitter);
	    }
	}
	else {
	    if (m_ditClosed) {
		if (m_nextStateTransitionMs < now) {
		    if (!m_wasSendingDit) {
			unsigned ptt_delay;

			m_wasSendingDit = true;
			KEY_UP(m_transmitter, m_twitchCount);
			ptt_delay = TRANSMITTER_KEY_DOWN(m_transmitter);
			m_nextStateTransitionMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		    }
		    else {
			m_wasSendingDit = false;
			KEY_DOWN(m_transmitter, m_twitchCount);
			TRANSMITTER_KEY_UP(m_transmitter);
			m_nextStateTransitionMs = now + WPM_DOT_TWITCHES();
		    }
		    m_twitchCount = 0;
		    m_msTwitchTimer = 0;
		}
	    }
	    else {
		if (m_wasSendingDit) {
		    if (m_nextStateTransitionMs < now) {
			m_wasSendingDit = false;
			KEY_DOWN(m_transmitter, m_twitchCount);
			TRANSMITTER_KEY_UP(m_transmitter);
			m_nextStateTransitionMs = now + WPM_DOT_TWITCHES();
		    }
		}
		else {
		    m_nextStateTransitionMs = now;
		    if (m_wasKeyDown) {
			m_wasKeyDown = false;
			KEY_DOWN(m_transmitter, m_twitchCount);
			TRANSMITTER_KEY_UP(m_transmitter);
			m_twitchCount = 0;
			m_msTwitchTimer = 0;
		    }
		}
	    }
	}
    }
    if (100 <= m_twitchCount) {
	if (m_wasKeyDown) {
	    KEY_DOWN(m_transmitter, m_twitchCount);
	}
	else {
	    KEY_UP(m_transmitter, m_twitchCount);
	}
	m_twitchCount = 0;
    }
    return input_mode;
}

