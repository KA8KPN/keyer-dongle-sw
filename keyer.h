#ifndef KEYER_H_INCLUDED
#define KEYER_H_INCLUDED

#include "options.h"

typedef enum input_mode { MODE_PADDLE_NORMAL, MODE_PADDLE_REVERSE, MODE_REMOTE_CONTROL } input_mode_t;

typedef enum keyer_mode {KEYER_IAMBIC_A, KEYER_IAMBIC_B, KEYER_ULTIMATIC, KEYER_SEMIAUTO, KEYER_STRAIGHT } keyer_mode_t;

#define ANALOG_IN_MAX 1023

#define MIN_WPM 5L
#define MAX_WPM 40L
#define MAX_WPM_OFFSET 25L
#define DEFAULT_WPM 20
#define MS_PER_TWITCH 120
#define TWITCHES_PER_DOT 10
#define TWITCHES_PER_DASH (3*TWITCHES_PER_DOT)
#define MS_PER_DOT    (TWITCHES_PER_DOT*MS_PER_TWITCH)
#define MS_PER_DASH   (3*MS_PER_DOT)
#define MS_PER_WORD   (7*MS_PER_DOT)
#define WPM_POT_PORT A7

#define LEFT_PADDLE  12
#define RIGHT_PADDLE 11

#define PTT_1       6
#define KEY_OUT_1   5
#define SELECT_1    4
#define PTT_DELAY_1 10
#define PTT_HANG_1  750

#define PTT_2       9
#define KEY_OUT_2   8
#define SELECT_2    7
#define PTT_DELAY_2 10
#define PTT_HANG_2  750

#define PTT_3       17
#define KEY_OUT_3   18
#define SELECT_3    19
#define PTT_DELAY_3 10
#define PTT_HANG_3  750

#define PTT_4       14
#define KEY_OUT_4   15
#define SELECT_4    16
#define PTT_DELAY_4 10
#define PTT_HANG_4  750

#define SIDETONE 10
#define SIDETONE_FREQUENCY 800

#endif // KEYER_H_INCLUDED
