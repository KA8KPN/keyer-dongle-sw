#ifndef COOTIE_H_INCLUDED
#define COOTIE_H_INCLUDED

#include "paddles.h"

class cootie : public paddles {
public:
    cootie(void);
    virtual input_mode_t update(unsigned long now, input_mode_t mode) override;

private:
    bool m_leftClosed, m_rightClosed;
    unsigned long m_then;
    bool m_wasKeyDown;
};

#endif // COOTIE_H_INCLUDED
