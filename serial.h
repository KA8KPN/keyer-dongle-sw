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
    void send_continue(void);
    void xoff(int xmitter);
    void xon(int xmitter);
    void xmitter_config(int xmitter);

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
#define SERIAL_XOFF(t) system_serial->xoff(t)
#define SERIAL_XON(t) system_serial->xon(t)
#define CONTACT_CLOSED(t, r) system_serial->contact_closed(t, r)
#define POT_VALUE(v) system_serial->pot_value(v)
#define KEY_UP(t, tw) system_serial->key_up(t, tw)
#define KEY_DOWN(t, tw) system_serial->key_down(t, tw)
#else // ! FEATURE_SERIAL_INPUT
#define SERIAL_INITIALIZE()
#define SERIAL_UPDATE()
#define SERIAL_XOFF(t) ((void)t)
#define SERIAL_XON(t) ((void)t)
#define CONTACT_CLOSED(t, r) ((void)t),((void)r)
#define POT_VALUE(v) ((void)v)
#define KEY_UP(t, tw) ((void)t),((void)tw)
#define KEY_DOWN(t, tw) ((void)t),((void)tw)
#endif // ! FEATURE_SERIAL_INPUT

#endif // SERIAL_H_INCLUDED
