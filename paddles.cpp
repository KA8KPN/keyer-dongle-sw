#include <Arduino.h>

#include "paddles.h"
#include "serial.h"

#include "iambic-a.h"
#include "cootie.h"
// #include "bug.h"

paddles *system_paddles = NULL;

void paddles_initialize(byte right_paddle, byte left_paddle)
{
    // system_paddles = new iambicA();
    system_paddles = new cootie();
    system_paddles->set_paddle_ports(right_paddle, left_paddle);
    pinMode(left_paddle, INPUT);
    digitalWrite(left_paddle, HIGH);
    pinMode(right_paddle, INPUT);
    digitalWrite(right_paddle, HIGH);
}

paddles::paddles(void) : m_leftPaddle(0), m_rightPaddle(0), m_paddleMode(MODE_PADDLE_NORMAL) {
    m_transmitter = 0;
}


void paddles::set_paddle_ports(byte right_paddle, byte left_paddle) {
    m_leftPaddle = left_paddle;
    m_rightPaddle = right_paddle;
    m_ditPaddle = m_leftPaddle;
    m_dahPaddle = m_rightPaddle;
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

void paddles::set_transmitter(int t) {
    TRANSMITTER_UNSELECT(m_transmitter);
    m_transmitter = t;
}
