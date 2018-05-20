#include <Arduino.h>

#include "options.h"

#include "keying.h"
#include "wpm.h"
#include "serial.h"

#include "keyer.h"

input_mode_t input_mode;

#define DEFINE_MORSE_TABLES
#include "morse_tables.h"

void setup () {
    input_mode = MODE_PADDLE_NORMAL;

    WPM_INITIALIZE();
    KEYING_INITIALIZE();
    PADDLES_INITIALIZE(RIGHT_PADDLE, LEFT_PADDLE);
    SERIAL_INITIALIZE();
}

void loop() {
    unsigned long now = millis();

    input_mode = PADDLES_UPDATE(now, input_mode);
    SERIAL_UPDATE();
    WPM_UPDATE();
    KEYING_UPDATE(now);
}	
