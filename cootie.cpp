#include <Arduino.h>

#include "cootie.h"

#include "wpm.h"
#include "serial.h"


cootie::~cootie(void) {
}

cootie::cootie(void) {
    m_msTwitchTimer = 0;
    m_twitchCount = 0;
    m_then = millis();
    m_wasKeyDown = false;
}

cootie::cootie(paddles const *old_config) {
    m_msTwitchTimer = 0;
    m_twitchCount = 0;
    m_then = millis();
    m_wasKeyDown = false;

    m_leftPaddle = old_config->m_leftPaddle;
    m_rightPaddle = old_config->m_rightPaddle;
    m_ditPaddle = old_config->m_ditPaddle;
    m_dahPaddle = old_config->m_dahPaddle;
    m_transmitter = old_config->m_transmitter;
    m_paddleMode = old_config->m_paddleMode;
}

input_mode_t cootie::update(unsigned long now, input_mode_t input_mode) {
    TRANSMITTER_SELECT(m_transmitter);
    m_msTwitchTimer += (now - m_then);
    m_then = now;
    while (m_msTwitchTimer > WPM_TWITCHES()) {
        m_msTwitchTimer -= WPM_TWITCHES();
	m_twitchCount++;
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

    if ((MODE_PADDLE_NORMAL == input_mode) || (MODE_PADDLE_REVERSE == input_mode)) {
	if (m_rightClosed || m_leftClosed) {
	    if (!m_wasKeyDown) {
		m_wasKeyDown = true;
		KEY_UP(m_transmitter, m_twitchCount);
		m_twitchCount = 0;
		m_msTwitchTimer = 0;
		TRANSMITTER_KEY_DOWN(m_transmitter);
	    }
	}
	else {
	    if (m_wasKeyDown) {
		m_wasKeyDown = false;
		KEY_DOWN(m_transmitter, m_twitchCount);
		m_twitchCount = 0;
		m_msTwitchTimer = 0;
		TRANSMITTER_KEY_UP(m_transmitter);
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

