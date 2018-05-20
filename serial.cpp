#include "options.h"

#ifdef FEATURE_SERIAL_INPUT

#include "Arduino.h"

#include "serial.h"
#include "text_to_morse.h"

serial *system_serial = NULL;

void serial_initialize(void) {
    delay(1000);
    Serial.begin(115200);
    while (!Serial) {
    }

    system_serial = new serial(true);
}


serial::serial(bool echo) : m_echoChars(echo) {
}

void serial::update(void) {
    if (Serial.available()) {
	char c;

	Serial.readBytes(&c, 1);
	// TODO:  Put the protocol processing here
	// TODO:  Remove the echo
	Serial.write(c);
	if ('\r' == c) {
	    Serial.write('\n');
	}
    }
}

#endif // FEATURE_SERIAL_INPUT
