#ifndef PADDLES_H_INCLUDED
#define PADDLES_H_INCLUDED

#include "keyer.h"
#include "keying.h"

class paddles {
public:
    paddles(void);
    virtual input_mode_t update(unsigned long now, input_mode_t mode) = 0;

    void set_paddle_ports(byte right_paddle, byte left_paddle);
    input_mode_t toggle_reverse(void);
    void set_transmitter(int t);
    int transmitter(void) const { return m_transmitter; }
    input_mode_t mode(void) const { return m_paddleMode; }

protected:
    uint8_t m_leftPaddle, m_rightPaddle;
    uint8_t m_ditPaddle, m_dahPaddle;
    int m_transmitter;
    input_mode_t m_paddleMode;
    unsigned long m_msTwitchTimer;
    unsigned m_twitchCount;
};

extern paddles *system_paddles;
void paddles_initialize(byte right_paddle, byte left_paddle);

#define PADDLES_INITIALIZE(right_paddle, left_paddle) paddles_initialize(right_paddle, left_paddle)
#define PADDLES_REVERSE() system_paddles->toggle_reverse()
#define PADDLES_UPDATE(now, keyer_mode) system_paddles->update(now, keyer_mode)
#define PADDLES_SET_TRANSMITTER(t)      system_paddles->set_transmitter(t)
#define PADDLES_POINTER system_paddles

#endif // PADDLES_H_INCLUDED
