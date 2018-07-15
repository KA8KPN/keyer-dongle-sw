#include "options.h"

#if defined(FEATURE_SPEED_CONTROL)

#include <Arduino.h>

#include "wpm.h"
#include "serial.h"

#include "keyer.h"

wpm *system_wpm = NULL;

void wpm_initialize(void) {
    system_wpm = new wpm(WPM_POT_PORT);
}

wpm::wpm(byte potPin) : m_potPin(potPin) {
    m_percentage = 200;
}

void wpm::update(void) {
    int potValue = 0;
    unsigned wpm;
 
    // The WPM is MIN_WPM + (MAX_WPM - MIN_WPM) * potValue / 1023; appropriately rounded
    potValue = analogRead(m_potPin);
    wpm = MIN_WPM + ((MAX_WPM - MIN_WPM) * potValue + ANALOG_IN_MAX/2) / ANALOG_IN_MAX;
    if (MIN_WPM > wpm) {
	wpm = MIN_WPM;
    }
    m_twitches = MS_PER_TWITCH/wpm;
    m_dotTwitches = MS_PER_DOT/wpm;
    m_dashTwitches = MS_PER_DASH/wpm;
    m_wordTwitches = MS_PER_WORD/wpm;
    // My pot only goes to 900 at its top end
    int percentage =  (100L * potValue) / 900;
    // There's a lot of noise on the AI, so I need a delta of at least two
    if (percentage != m_percentage) {
	system_serial->pot_value(percentage);
	m_percentage = percentage;
    }
}

#endif // defined(FEATURE_SPEED_CONTROL)
