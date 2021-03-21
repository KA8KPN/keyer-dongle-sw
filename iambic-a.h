#ifndef IAMBIC_A_H_INCLUDED
#define IAMBIC_A_H_INCLUDED

#include "paddles.h"

class iambicA : public paddles {
public:
    iambicA(void);
    iambicA(paddles const *old);
    virtual input_mode_t update(unsigned long now, input_mode_t mode) override;
    virtual ~iambicA(void);
    virtual keyer_mode_t keyer_mode(void) const override { return KEYER_IAMBIC_A; }

private:
    bool m_ditClosed, m_dahClosed;
    bool m_leftClosed, m_rightClosed;
    unsigned long m_nextStateTransitionMs;
    unsigned long m_startReadingPaddlesMs;
    keyer_state_t m_keyerState;
    keyer_state_t m_lastKeyerState;
};

#endif // IAMBIC_A_H_INCLUDED
