#ifndef IAMBIC_A_H_INCLUDED
#define IAMBIC_A_H_INCLUDED

#include "paddles.h"

class iambicA : public paddles {
public:
    iambicA(void);
    virtual input_mode_t update(unsigned long now, input_mode_t mode) override;

private:
    bool m_ditClosed, m_dahClosed;
    bool m_leftClosed, m_rightClosed;
    unsigned long m_nextStateTransitionMs;
    unsigned long m_startReadingPaddlesMs;
    keyer_state_t m_keyerState;
    keyer_state_t m_lastKeyerState;
};

#endif // IAMBIC_A_H_INCLUDED
