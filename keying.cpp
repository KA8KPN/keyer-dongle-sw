#include <Arduino.h>

#include "keying.h"
#include "keyer.h"

#define MAX_TRANSMITTERS  4

transmitter *transmitters[MAX_TRANSMITTERS] = {NULL, NULL, NULL, NULL};

static void sidetone_action(int frequency, bool key_down) {
    if (key_down) {
	tone(SIDETONE, frequency);
    }
    else {
	noTone(SIDETONE);
    }
}

static void null_sidetone(int frequency, bool key_down) {
    (void) frequency;
    (void) key_down;
}

transmitter::transmitter(int ptt_line, int key_out_line, unsigned ptt_delay, unsigned ptt_hang, void (*sidetone_action)(int, bool)) {
    m_pttLine = ptt_line;
    m_keyOutLine = key_out_line;
    m_pttDelay = ptt_delay;
    m_pttHang = ptt_hang;
    m_state = keying_idle;
    m_nextStateChange = 0;
    m_sidetoneAction = sidetone_action;
}

void transmitter::key_up(void) {
    m_state = keying_key_up;
    digitalWrite(m_keyOutLine, LOW);
    m_sidetoneAction(SIDETONE_FREQUENCY, false);
    m_nextStateChange = millis() + m_pttHang;
}

unsigned transmitter::key_down(void) {
    unsigned result = 0;
 
    // Key down may not actually key down.  If the PTT has already been triggered for longer than the delay time, then we key down.
    // If a PTT delay has been specified and the system is not in transmit mode, then we trigger the
    // ptt line, returning the delay and the update function will actually key the transmitter after
    // the appropriate ptt delay
    if (keying_idle == m_state) {
	m_state = keying_ptt_wait;
	ptt_push();
	m_nextStateChange = millis() + m_pttDelay;
	result = m_pttDelay;
    }
    else {
	key_down_actual();
    }
    return result;
}

void transmitter::key_down_actual(void) {
    m_state = keying_key_down;
    digitalWrite(m_keyOutLine, HIGH);
    m_sidetoneAction(SIDETONE_FREQUENCY, true);
}

void transmitter::ptt_push(void) {
    digitalWrite(m_pttLine, HIGH);
}

void transmitter::ptt_release(void) {
    digitalWrite(m_pttLine, LOW);
}


void transmitter::update(unsigned long now) {
    if (m_nextStateChange < now) {
	switch(m_state) {
	case keying_ptt_wait:
	    key_down_actual();
	    break;

	case keying_key_up:
	    ptt_release();
	    m_state = keying_idle;
	    break;

	default:
	    break;
	}
    }
}


transmitter::~transmitter(void) {
}


void keying_initialize(void) {
    pinMode(SIDETONE, OUTPUT);
    digitalWrite(SIDETONE, LOW);

    transmitters[0] = new transmitter(PTT_1, KEY_OUT_1, PTT_DELAY_1, PTT_HANG_1, sidetone_action);
    transmitters[1] = new transmitter(PTT_2, KEY_OUT_2, PTT_DELAY_2, PTT_HANG_2, null_sidetone);
    transmitters[2] = new transmitter(PTT_3, KEY_OUT_3, PTT_DELAY_3, PTT_HANG_3, null_sidetone);
    transmitters[3] = new transmitter(PTT_4, KEY_OUT_4, PTT_DELAY_4, PTT_HANG_4, null_sidetone);
}
