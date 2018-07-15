#include "options.h"

#ifdef FEATURE_SERIAL_INPUT

#include "Arduino.h"

#include "serial.h"
#include "controller.h"

serial *system_serial = NULL;

void serial_initialize(void) {
    delay(1000);
    Serial.begin(115200);
    while (!Serial) {
    }

    system_serial = new serial(true);
}


serial::serial(bool echo) : m_echoChars(echo) {
    m_bufPtr = 0;
    m_sawCr = false;
}

void serial::process(void) {
    if (('u' == m_buffer[0]) && (':' == m_buffer[1])) {
	char *dptr;
	int xmitter;

	xmitter = strtol(m_buffer+2, &dptr, 10);
	if (dptr != (2 + m_buffer)) {
	    controllers[xmitter-1]->key(false,strtol(dptr + 1, NULL, 10));
	}
    }
    if (('d' == m_buffer[0]) && (':' == m_buffer[1])) {
	char *dptr;
	int xmitter;

	xmitter = strtol(m_buffer+2, &dptr, 10);
	if (dptr != (2 + m_buffer)) {
	    controllers[xmitter-1]->key(true, strtol(dptr + 1, NULL, 10));
	}
    }
}

void serial::update(void) {
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

    if (right) {
	sprintf(buff, "pr:%d\r\n", xmitter);
    }
    else {
	sprintf(buff, "pl:%d\r\n", xmitter);
    }
    Serial.write((uint8_t *)buff, strlen(buff));
}

void serial::key_up(int xmitter, unsigned twitches) {
    char buff[30];

    sprintf(buff, "u:%d:%u\r\n", xmitter, twitches);
    Serial.write((uint8_t *)buff, strlen(buff));
}

void serial::key_down(int xmitter, unsigned twitches) {
    char buff[30];

    sprintf(buff, "d:%d:%u\r\n", xmitter, twitches);
    Serial.write((uint8_t *)buff, strlen(buff));
}

void serial::pot_value(unsigned value) {
    char buff[15];

    sprintf(buff, "s:%u\r\n", value);
    Serial.write((uint8_t *)buff, strlen(buff));
}

void serial::flow_control(int xmitter, bool flow_on) {
    char buff[30];

    if (flow_on) {
	sprintf(buff, "xon:%d\r\n", xmitter);
    }
    else {
	sprintf(buff, "xoff:%d\r\n", xmitter);
    }
    Serial.write((uint8_t *)buff, strlen(buff));
}


#endif // FEATURE_SERIAL_INPUT
