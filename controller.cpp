#include <Arduino.h>

#include "controller.h"
#include "paddles.h"
#include "serial.h"
#include "wpm.h"

remote_controller *controllers[MAX_TRANSMITTERS];

void initialize_controllers(void) {
    for (int i=0; i<MAX_TRANSMITTERS; ++i) {
	controllers[i] = new remote_controller(i);
    }
}


remote_controller::remote_controller(unsigned transmitter) : m_transmitter(transmitter) {
    m_head = 0;
    m_tail = 0;
    m_count = 0;
    m_paused = false;
    m_nextTransition = 0;
}

void remote_controller::key(bool key_down, unsigned duration) {
    if (m_count < KEY_LIST_SIZE) {
	m_keyList[m_tail].key_down = key_down;
	m_keyList[m_tail].duration = duration * ((unsigned long) WPM_TWITCHES());

	m_tail = (m_tail + 1) % KEY_LIST_SIZE;
	m_count++;
	if (!m_paused && (HIGH_WATER == m_count)) {
	    m_paused = true;
	    SERIAL_XOFF(m_transmitter);
	}
    }
}

void remote_controller::wpm(int wpm) {
    m_wpm = wpm;
    // TODO:  The rest of the WPM logic
}


void remote_controller::kud(int twitches) {
    m_kud = twitches;
}

void remote_controller::kdd(int ms) {
    m_kdd = ms;
}


input_mode_t remote_controller::update(unsigned long now, input_mode_t input_mode) {
    // If it is not transmitting right this instant, check to see if there's something in the queue
    if ((MODE_PADDLE_NORMAL == input_mode) || (MODE_PADDLE_REVERSE == input_mode)) {
	if (0 < m_count) {
	    input_mode = MODE_REMOTE_CONTROL;
	    m_nextTransition = 0;
	}
    }

    if (MODE_REMOTE_CONTROL == input_mode) {	
	if (m_nextTransition < now) {
	    // If there's a new queued item to look at, start to do whatever it says
	    // Or, if this queued item is done, then go to the next queued item.
	    if (0 < m_count) {
		m_nextTransition = now + m_keyList[m_head].duration;
		if (m_keyList[m_head].key_down) {
		    TRANSMITTER_KEY_DOWN(m_transmitter);
		}
		else {
		    TRANSMITTER_KEY_UP(m_transmitter);
		}
		m_count--;
		m_head = (m_head + 1) % KEY_LIST_SIZE;
		// If the number of items drops below the high water mark, turn on 
		if (m_paused && (LOW_WATER == m_count)) {
		    m_paused = false;
		    SERIAL_XON(m_transmitter);
		}
	    }
	    else {
		// If we're out of queued items, unkey the transmitter
		TRANSMITTER_KEY_UP(m_transmitter);
		// And get the paddle mode from the paddles class
		input_mode = system_paddles->mode();
	    }
	}
    }
    return input_mode;
}
