#ifndef COOTIE_H_INCLUDED
#define COOTIE_H_INCLUDED

#include "paddles.h"

class cootie : public paddles {
public:
    cootie(void);
    cootie(paddles const *old);
    virtual input_mode_t update(unsigned long now, input_mode_t mode) override;
    virtual ~cootie(void);
    virtual keyer_mode_t keyer_mode(void) const { return KEYER_STRAIGHT; }

private:
    bool m_leftClosed, m_rightClosed;
    unsigned long m_then;
    bool m_wasKeyDown;
};

#endif // COOTIE_H_INCLUDED
