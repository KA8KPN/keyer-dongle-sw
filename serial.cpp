#include "options.h"

#ifdef FEATURE_SERIAL_INPUT

#include "Arduino.h"
// #include "avr/wdt.h"

#include "serial.h"
#include "controller.h"
#include "paddles.h"

serial *system_serial = NULL;

void serial_initialize(void) {
    // wdt_enable(WDTO_120MS);
    // delay(1000);
    Serial.begin(115200);
    while (!Serial) {
    }

    system_serial = new serial(true);
}


serial::serial(bool echo) : m_echoChars(echo) {
    m_bufPtr = 0;
    m_sawCr = false;
}

void serial::send_continue(void) {
    Serial.write((uint8_t *)"a\r\n", 3);
}

void serial::xoff(int xmitter) {
    char buff[15];
    int len;
    len = sprintf(buff, "xoff:%d\r\n", xmitter);
    Serial.write((uint8_t *)buff, len);
}

void serial::xon(int xmitter) {
    char buff[15];
    int len;
    len = sprintf(buff, "xon:%d\r\n", xmitter);
    Serial.write((uint8_t *)buff, len);
}

void serial::xmitter_config(int xmitter) {
    // Okay, there is a 'c', 5 colons and a crlf, so 8 characters
    // plus a single digit transmitter number, a single digit active flag
    // and three three digit numbers, which is 11 more characters
    // and a nll terminator
    char buff[25];
    int len;
    len = sprintf(buff, "c:%d:%d:%d:%d:%d\r\n", xmitter, xmitter==system_paddles->transmitter(),
		  controllers[xmitter-1]->wpm(), controllers[xmitter-1]->kdd(),
		  controllers[xmitter-1]->kud());
    Serial.write((uint8_t *)buff, len);
}

void serial::process(void) {
    if (('u' == m_buffer[0]) && (':' == m_buffer[1])) {
	char *dptr;
	int xmitter;

	xmitter = strtol(m_buffer+2, &dptr, 10);
	if (dptr != (2 + m_buffer)) {
	    controllers[xmitter-1]->key(false,strtol(dptr + 1, NULL, 10));
	    send_continue();
	}
    }
    if (('d' == m_buffer[0]) && (':' == m_buffer[1])) {
	char *dptr;
	int xmitter;

	xmitter = strtol(m_buffer+2, &dptr, 10);
	if (dptr != (2 + m_buffer)) {
	    controllers[xmitter-1]->key(true, strtol(dptr + 1, NULL, 10));
	    send_continue();
	}
    }
    // The reset command simple does a tight loop, which will get killed by the watchdog timer
    if (0 == strncmp("reset", m_buffer, 5)) {
	while(1) {
	}
    }
    if (('s' == m_buffer[0]) && (':' == m_buffer[1])) {
	char *dptr;
	int xmitter;

	xmitter = strtol(m_buffer+2, &dptr, 10);
	if (dptr != (2 + m_buffer)) {
	    controllers[xmitter-1]->wpm(strtol(dptr + 1, NULL, 10));
	    send_continue();
	}
    }
    if (('c' == m_buffer[0]) && (':' == m_buffer[1])) {
	char *dptr;
	int xmitter;

	xmitter = strtol(m_buffer+2, &dptr, 10);
	if (dptr != (2 + m_buffer)) {
	    send_continue();
	    xmitter_config(xmitter);
	}
    }
    if (0 == strncmp("kdd:", m_buffer, 4)) {
	char *dptr;
	int xmitter;

	xmitter = strtol(m_buffer+4, &dptr, 10);
	if (dptr != (4 + m_buffer)) {
	    send_continue();
	    controllers[xmitter-1]->kdd(strtol(dptr + 1, NULL, 10));
	}
    }
    if (0 == strncmp("kud:", m_buffer, 4)) {
	char *dptr;
	int xmitter;

	xmitter = strtol(m_buffer+4, &dptr, 10);
	if (dptr != (4 + m_buffer)) {
	    send_continue();
	    controllers[xmitter-1]->kud(strtol(dptr + 1, NULL, 10));
	}
    }
}

void serial::update(void) {
    // Normally wdt_reset() would be called here
    // unsigned long now = millis();
    while (Serial.available()) {
	char c;

	Serial.readBytes(&c, 1);
	m_buffer[m_bufPtr] = c;
	if ((m_bufPtr + 1) < m_bufSiz) {
	    ++m_bufPtr;
	}
	if (m_sawCr && ('\n' == c)) {
	    m_buffer[m_bufPtr] = '\0';
	    Serial.write(0x13);
	    process();
	    Serial.write(0x11);
	    m_bufPtr = 0;
	}
	if ('\r' == c) {
	    m_sawCr = true;
	}
	else {
	    m_sawCr = false;
	}
    }
}


void serial::contact_closed(int xmitter, bool right) {
    char buff[15];
    int len;

    if (right) {
	len = sprintf(buff, "pr:%d\r\n", xmitter);
    }
    else {
	len = sprintf(buff, "pl:%d\r\n", xmitter);
    }
    Serial.write((uint8_t *)buff, len);
}

void serial::key_up(int xmitter, unsigned twitches) {
    char buff[30];
    int len;

    len = sprintf(buff, "u:%d:%u\r\n", xmitter, twitches);
    Serial.write((uint8_t *)buff, len);
}

void serial::key_down(int xmitter, unsigned twitches) {
    char buff[30];
    int len;

    len = sprintf(buff, "d:%d:%u\r\n", xmitter, twitches);
    Serial.write((uint8_t *)buff, len);
}

void serial::pot_value(unsigned value) {
    char buff[15];
    int len;

    len = sprintf(buff, "s:%u\r\n", value);
    Serial.write((uint8_t *)buff, len);
}

#endif // FEATURE_SERIAL_INPUT
