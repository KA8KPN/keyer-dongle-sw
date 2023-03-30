#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

// The speed control is just a speed control, and it uses a potentiometer and
// it's use is discouraged.  The rotary encoder is used for a lot more than
// speed control, so including it replaces the speed control feature.

// Comment this out to remove the code for the speed control
//#define FEATURE_SPEED_CONTROL
// Comment this out to remove the code for the rotary encoder
#define FEATURE_ROTARY_ENCODER
// Comment this out for a keyer that is only standalone
#define FEATURE_SERIAL_INPUT

#if defined(FEATURE_ROTARY_ENCODER) && defined(FEATURE_SPEED_CONTROL)
#error "You can have only one of FEATURE_ROTARY_ENCODER or FEATURE_SPEED_CONTROL"
#endif

#endif // OPTIONS_H_INCLUDED
