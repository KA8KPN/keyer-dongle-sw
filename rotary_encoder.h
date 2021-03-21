#ifndef ROTARY_ENCODER_H_INCLUDED
#define ROTARY_ENCODER_H_INCLUDED

#include "options.h"

#if defined(FEATURE_ROTARY_ENCODER)

class rotary_encoder {
public:
    rotary_encoder();
    void update(void);
    unsigned twitches(void) const { return m_twitches; }
    unsigned dot_twitches(void) const { return m_dotTwitches; }
    unsigned dash_twitches(void) const { return m_dashTwitches; }
    unsigned word_twitches(void) const { return m_wordTwitches; }

private:
    typedef enum blink_mode {BLINK_OFF, BLINK_ON, BLINK_SLOW, BLINK_FAST} t_blinkMode;
    typedef enum knob_mode {KNOB_SPEED, KNOB_XMIT_SELECT, KNOB_REVERSE, KNOB_KEY_MODE} t_knobMode;
    unsigned m_position;
    unsigned m_twitches;
    unsigned m_dotTwitches;
    unsigned m_dashTwitches;
    unsigned m_wordTwitches;
    int m_percentage;
    bool m_wasPressed;
    unsigned long m_pressedMs;
    t_blinkMode m_blinkMode;
    t_knobMode m_knobMode;
    bool m_blinkState;
    unsigned long m_lastBlinkChange;
};

extern rotary_encoder *system_rotary_encoder;
void rotary_encoder_initialize(void);

#define WPM_INITIALIZE()    rotary_encoder_initialize()
#define WPM_UPDATE()        system_rotary_encoder->update()
#define WPM_TWITCHES()      system_rotary_encoder->twitches()
#define WPM_DOT_TWITCHES()  system_rotary_encoder->dot_twitches()
#define WPM_DASH_TWITCHES() system_rotary_encoder->dash_twitches()
#define WPM_WORD_TWITCHES() system_rotary_encoder->word_twitches()
#else // !defined(FEATURE_ROTARY_ENCODER)
#if !defined(FEATURE_SPEED_CONTROL)
#define WPM_INITIALIZE()
#define WPM_UPDATE()
#define WPM_TWITCHES()      (MS_PER_TWITCH/DEFAULT_WPM)
#define WPM_DOT_TWITCHES()  (MS_PER_DOT/DEFAULT_WPM)
#define WPM_DASH_TWITCHES() (MS_PER_DASH/DEFAULT_WPM)
#define WPM_WORD_TWITCHES() (MS_PER_WORD/DEFAULT_WPM)
#endif // !defined(FEATURE_SPEED_CONTROL)

#endif // defined(FEATURE_ROTARY_ENCODER)

#endif // !ROTARY_ENCODER_H_INCLUDED
