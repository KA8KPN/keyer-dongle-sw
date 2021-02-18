#ifndef BUG_H_INCLUDED
#define BUG_H_INCLUDED

#include "paddles.h"

class bug : public paddles {
public:
    bug(void);
    bug(paddles const *old);
    virtual input_mode_t update(unsigned long now, input_mode_t mode) override;
    virtual ~bug(void);

private:
    bool m_ditClosed, m_dahClosed;
    bool m_leftClosed, m_rightClosed;
    unsigned long m_then;
    bool m_wasKeyDown, m_wasSendingDit;
    unsigned long m_nextStateTransitionMs;
};

#endif // BUG_H_INCLUDED
