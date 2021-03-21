#include "options.h"

#if defined(FEATURE_ROTARY_ENCODER)

#include <Arduino.h>
#include <limits.h>

#include "rotary_encoder.h"
#include "serial.h"
#include "paddles.h"

#include "keyer.h"

#define SENSITIVITY 4

rotary_encoder *system_rotary_encoder = NULL;

static void encoder_isr(void);

void rotary_encoder_initialize(void) {
    system_rotary_encoder = new rotary_encoder();
    pinMode(ENCODER_PIN_A, INPUT);
    pinMode(ENCODER_PIN_B, INPUT);
    attachInterrupt(0, encoder_isr, CHANGE);
    pinMode(LED_BUILTIN, OUTPUT);
}

static int encoder_position;

static void encoder_isr() {
    if (digitalRead(ENCODER_PIN_A) == digitalRead(ENCODER_PIN_B)) {
	if (encoder_position < INT_MAX) {
	    encoder_position++;
	}
    }
    else {
	if (encoder_position > INT_MIN) {
	    encoder_position--;
	}
    }
}

rotary_encoder::rotary_encoder(void) {
    m_position = SENSITIVITY * DEFAULT_WPM;
    m_percentage = 200;
    m_wasPressed = false;
    m_pressedMs = 0;
    m_blinkMode = BLINK_OFF;
    m_knobMode = KNOB_SPEED;
    m_blinkState = false;
    m_lastBlinkChange = 0;
}

void rotary_encoder::update(void) {
    unsigned long now = millis();
    int btn = analogRead(ENCODER_BUTTON);
    // btn has value 0 when pressed, and value 1023 when not pressed
    bool pressed = (btn < 512);
    if (pressed) {
	if (m_wasPressed != pressed) {
	    m_pressedMs = now;
	}
	long duration = now - m_pressedMs;
	if (KNOB_SPEED == m_knobMode) {
	    if (1000 > duration) {
		m_blinkMode = BLINK_ON;
	    }
	    else {
		if (3000 < duration) {
		    m_blinkMode = BLINK_SLOW;
		}
		else {
		    m_blinkMode = BLINK_FAST;
		}
	    }
	}
	else {
	    m_blinkMode = BLINK_OFF;
	}
    }
    else {
	if (m_wasPressed) {
	    if (KNOB_SPEED == m_knobMode) {
		long duration = now - m_pressedMs;
		if (1000 > duration) {
		    m_knobMode= KNOB_XMIT_SELECT;
		}
		else {
		    TRANSMITTER_UNSELECT(PADDLES_TRANSMITTER());
		    if (3000 > duration) {
			m_knobMode= KNOB_REVERSE;
		    }
		    else {
			m_knobMode= KNOB_KEY_MODE;		    
		    }
		}
	    }
	    else {
		m_knobMode = KNOB_SPEED;
		digitalWrite(SELECT_1, LOW);
		digitalWrite(SELECT_2, LOW);
		digitalWrite(SELECT_3, LOW);
		digitalWrite(SELECT_4, LOW);
		TRANSMITTER_SELECT(PADDLES_TRANSMITTER());
	    }
	}

	switch (m_knobMode) {
	case KNOB_XMIT_SELECT:
	{
	    if (encoder_position > 10) {
		encoder_position = 0;
		if (3 > PADDLES_TRANSMITTER()) {
		    PADDLES_SET_TRANSMITTER(1+PADDLES_TRANSMITTER());
		}
		else {
		    PADDLES_SET_TRANSMITTER(0);
		}
	    }
	    if (encoder_position < -10) {
		encoder_position = 0;
		if (0 < PADDLES_TRANSMITTER()) {
		    PADDLES_SET_TRANSMITTER(PADDLES_TRANSMITTER()-1);
		}
		else {
		    PADDLES_SET_TRANSMITTER(3);
		}
	    }
	    TRANSMITTER_SELECT(PADDLES_TRANSMITTER());
	}
	break;

	case KNOB_REVERSE:
	{
	    if (PADDLES_REVERSED()) {
		digitalWrite(SELECT_1, LOW);
		digitalWrite(SELECT_2, LOW);
		digitalWrite(SELECT_3, HIGH);
		digitalWrite(SELECT_4, HIGH);
	    }
	    else {
		digitalWrite(SELECT_1, HIGH);
		digitalWrite(SELECT_2, HIGH);
		digitalWrite(SELECT_3, LOW);
		digitalWrite(SELECT_4, LOW);
	    }
	}
	if ((encoder_position > 10) || (encoder_position < -10)) {
	    PADDLES_REVERSE();
	    encoder_position = 0;
	}
	break;

	// The current modes are:
	// Iambic-A ._
	// Cootie or Manual _._. __
	// Bug or Semiautomatic _... ...
	// I will light the dots for the mode
	// So, Iambic will be "A" and will light transmitter 1
	// Cootie will light 2 and 4
	// Semiauto will light 1, 2, and 3
	case KNOB_KEY_MODE:
	{
	    switch(PADDLES_MODE())
	    {
	    case KEYER_SEMIAUTO:
		digitalWrite(SELECT_1, HIGH);
		digitalWrite(SELECT_2, HIGH);
		digitalWrite(SELECT_3, HIGH);
		digitalWrite(SELECT_4, LOW);
		if (encoder_position > 10) {
		    encoder_position = 0;
		    PADDLES_SET_COOTIE();
		}		
		if (encoder_position < -10) {
		    encoder_position = 0;
		    PADDLES_SET_IAMBIC_A();
		}		
		break;

	    case KEYER_STRAIGHT:
		digitalWrite(SELECT_1, LOW);
		digitalWrite(SELECT_2, HIGH);
		digitalWrite(SELECT_3, LOW);
		digitalWrite(SELECT_4, HIGH);
		if (encoder_position > 10) {
		    encoder_position = 0;
		    PADDLES_SET_IAMBIC_A();
		}		
		if (encoder_position < -10) {
		    encoder_position = 0;
		    PADDLES_SET_BUG();
		}		
		break;

	    default:
		digitalWrite(SELECT_1, HIGH);
		digitalWrite(SELECT_2, LOW);
		digitalWrite(SELECT_3, LOW);
		digitalWrite(SELECT_4, LOW);
		if (encoder_position > 10) {
		    encoder_position = 0;
		    PADDLES_SET_BUG();
		}		
		if (encoder_position < -10) {
		    encoder_position = 0;
		    PADDLES_SET_COOTIE();
		}		
		break;
	    }
	}
	break;

	default: // KNOB_SPEED
	{
	    TRANSMITTER_SELECT(PADDLES_TRANSMITTER());
	    m_pressedMs = 0;

	    m_position = m_position + encoder_position;
	    m_position = (m_position < SENSITIVITY*MIN_WPM) ? SENSITIVITY*MIN_WPM : m_position;
	    m_position = (m_position > SENSITIVITY*MAX_WPM) ? SENSITIVITY*MAX_WPM : m_position;
	    unsigned int wpm = (m_position+(SENSITIVITY/2)) / SENSITIVITY;
	    encoder_position = 0;

	    m_twitches = MS_PER_TWITCH/wpm;
	    m_dotTwitches = MS_PER_DOT/wpm;
	    m_dashTwitches = MS_PER_DASH/wpm;
	    m_wordTwitches = MS_PER_WORD/wpm;

	    int percentage =  (100L * (wpm - MIN_WPM)) / (MAX_WPM - MIN_WPM);
	    if (percentage != m_percentage) {
		POT_VALUE(percentage);
		m_percentage = percentage;
	    }
	}
	break;
	}
    }
    switch (m_blinkMode) {
    case BLINK_ON:
	digitalWrite(LED_BUILTIN, HIGH);
	m_blinkState = true;
	m_lastBlinkChange = now;
	break;

    case BLINK_SLOW:
	if (500 < (now - m_lastBlinkChange)) {
	    if (m_blinkState) {
		digitalWrite(LED_BUILTIN, LOW);
		m_blinkState = false;
	    }
	    else {
		digitalWrite(LED_BUILTIN, HIGH);
		m_blinkState = true;
	    }
	    m_lastBlinkChange = now;
	}
	break;

    case BLINK_FAST:
	if (100 < (now - m_lastBlinkChange)) {
	    if (m_blinkState) {
		digitalWrite(LED_BUILTIN, LOW);
		m_blinkState = false;
	    }
	    else {
		digitalWrite(LED_BUILTIN, HIGH);
		m_blinkState = true;
	    }
	    m_lastBlinkChange = now;
	}
	break;

    default: // BLINK_OFF
	digitalWrite(LED_BUILTIN, LOW);
	m_blinkState = false;
	m_lastBlinkChange = now;
	break;
    }
    m_wasPressed = pressed;
}

#endif // defined(FEATURE_ROTARY_ENCODER)
