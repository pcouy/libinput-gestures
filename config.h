#include "libinput-gestures.h"

//#define DEBUG


static const struct action user_actions[] = {
//  {gesture_type, n_fingers, action_type, CMD(command, arg1, arg2, ...)}
    {SWIPE, 3, ON_END, CMD("notify-send", "hello")},
    {SWIPE, 4, ON_END, CMD("sleep", "20")},
    {PINCH, 2, ON_END, CMD("id")},
};
