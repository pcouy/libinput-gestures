#include "libinput-gestures.h"

//#define DEBUG

// `trigger_config` format : {threshold, min_duration, max_duration}
#define BASIC_SWIPE_CONFIG {50, 0, 3000}
#define LONG_SWIPE_CONFIG {50, 0, 30000}
#define PINCH_OUT_CONFIG {1.2, 0, 3000}
#define PINCH_IN_CONFIG {0.75, 0, 3000}
#define CATCH_ALL_CONFIG {0, 0, 0}

static const struct trigger user_triggers[] = {
//  {gesture_type, swipe_direction, n_fingers, trigger_type, trigger_config, CMD(command, arg1, arg2, ...)}
    {SWIPE, LEFT, 3, ON_END, BASIC_SWIPE_CONFIG, CMD("notify-send", "hello-left")},
    {SWIPE, RIGHT, 3, ON_END, BASIC_SWIPE_CONFIG, CMD("notify-send", "hello-right")},
    {SWIPE, UP, 3, ON_END, BASIC_SWIPE_CONFIG, CMD("notify-send", "hello-up")},
    {SWIPE, DOWN, 3, ON_END, BASIC_SWIPE_CONFIG, CMD("notify-send", "hello-down")},
    {SWIPE, LEFT, 3, REPEAT, LONG_SWIPE_CONFIG, CMD("notify-send", "hello-repeat-left")},
    {SWIPE, RIGHT, 3, REPEAT, LONG_SWIPE_CONFIG, CMD("notify-send", "hello-repeat-right")},
    {SWIPE, UP, 3, REPEAT, LONG_SWIPE_CONFIG, CMD("notify-send", "hello-repeat-up")},
    {SWIPE, DOWN, 3, REPEAT, LONG_SWIPE_CONFIG, CMD("notify-send", "hello-repeat-down")},
    {SWIPE, NONE, 3, ON_END, BASIC_SWIPE_CONFIG, CMD("notify-send", "hello")},
    {PINCH, NONE, 2, ON_END, PINCH_IN_CONFIG, CMD("notify-send", "pinch-in")},
    {PINCH, NONE, 2, ON_END, PINCH_OUT_CONFIG, CMD("notify-send", "pinch-out")},
    {SWIPE, NONE, 3, ON_END, CATCH_ALL_CONFIG, CMD("notify-send", "catchall-swipe")},
    {SWIPE, NONE, 3, ON_THRESHOLD, BASIC_SWIPE_CONFIG, CMD("notify-send", "catchall-swipe-threshold")},
    {PINCH, NONE, 2, ON_END, CATCH_ALL_CONFIG, CMD("notify-send", "catchall-pinch")},
    {PINCH, NONE, 2, ON_THRESHOLD, PINCH_IN_CONFIG, CMD("notify-send", "catchall-pinch-in-threshold")},
    {PINCH, NONE, 2, REPEAT, PINCH_OUT_CONFIG, CMD("notify-send", "catchall-pinch-out-repeat")},
};
