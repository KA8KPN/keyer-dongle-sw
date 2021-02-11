#include <Arduino.h>

#include "options.h"

#include "keying.h"
#include "wpm.h"
#include "serial.h"
#include "paddles.h"
#include "controller.h"

#include "keyer.h"

input_mode_t input_mode;

void setup () {
    input_mode = MODE_PADDLE_NORMAL;

    WPM_INITIALIZE();
    KEYING_INITIALIZE();
    PADDLES_INITIALIZE(RIGHT_PADDLE, LEFT_PADDLE);
    SERIAL_INITIALIZE();
    initialize_controllers();
    PADDLES_REVERSE();
}

void loop() {
    unsigned long now = millis();

    SERIAL_UPDATE();
    WPM_UPDATE();
    for(int i=0; i<MAX_TRANSMITTERS; ++i) {
	// The paddles are special because the transmitter that the paddles have been configured for
	// can be keyed either with the paddles or by the remote controller.  The other transmitters
	// can only be keyed by the remote controller
	if (i == system_paddles->transmitter()) {
	    // digitalWrite(SELECT_3, HIGH);
	    input_mode = PADDLES_UPDATE(now, input_mode);
	    input_mode = controllers[i]->update(now, input_mode);
	}
	else {
 	    // digitalWrite(SELECT_4, HIGH);
	    controllers[i]->update(now, MODE_REMOTE_CONTROL);
	}
        KEYING_UPDATE(i, now);
    }
}	
