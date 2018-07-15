#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include "options.h"

#ifdef FEATURE_SERIAL_INPUT
#include "keyer.h"
#include "keying.h"
#include "wpm.h"

void serial_initialize(void);

class serial {
public:
    serial(bool echo);
    void update(void);
    void key_up(int xmitter, unsigned twitches);
    void key_down(int xmitter, unsigned twitches);
    void contact_closed(int xmitter, bool right);
    void pot_value(unsigned value);
    void flow_control(int xmitter, bool flow_on);

private:
    void process(void);
    bool m_echoChars;
    static const int m_bufSiz = 25;
    char m_buffer[m_bufSiz];
    int m_bufPtr;
    bool m_sawCr;
};

extern serial *system_serial;
#define SERIAL_INITIALIZE() serial_initialize()
#define SERIAL_UPDATE() system_serial->update()
#else // ! FEATURE_SERIAL_INPUT
#define SERIAL_INITIALIZE()
#define SERIAL_UPDATE()
#endif // ! FEATURE_SERIAL_INPUT

#endif // SERIAL_H_INCLUDED
