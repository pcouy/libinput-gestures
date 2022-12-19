#ifndef _INCLUDED_INPUT_GESTURES
#define _INCLUDED_INPUT_GESTURES
#include <sys/types.h>
#define DEBUG

#include <libinput.h>

struct gesture_breakdown;

void try_input_sgid();
void try_root_suid();
static int open_restricted(const char *path, int flags, void *user_data);
static void close_restricted(int fd, void *user_data);
void print_gesture(struct libinput_event_gesture *gesture);
struct event_state handle_event(struct libinput_event *event, struct event_state state);
struct event_state new_state(void);
struct event_state dispatch_gesture(struct gesture_breakdown breakdown, struct libinput_event_gesture *gesture, struct event_state state);
struct event_state handle_swipe_update(struct libinput_event_gesture *gesture, struct event_state state);
struct event_state handle_pinch_update(struct libinput_event_gesture *gesture, struct event_state state);
struct event_state handle_swipe_end(struct libinput_event_gesture *gesture, struct event_state state);
struct event_state handle_pinch_end(struct libinput_event_gesture *gesture, struct event_state state);
struct event_state handle_hold_end(struct libinput_event_gesture *gesture, struct event_state state);
void monitor_events();
struct gesture_breakdown gesture_to_breakdown(struct libinput_event_gesture *gesture);

enum gesture_type {
	ERR_TYPE,
	SWIPE,
	PINCH,
	HOLD,
};

enum gesture_step {
	ERR_STEP,
	BEGIN,
	UPDATE,
	END,
	CANCEL,
};

struct gesture_breakdown {
	enum gesture_type type;
	enum gesture_step step;
};

struct swipe_state {
	float cumulative_dx;
	float cumulative_dy;
	float last_x_threshold;
	float last_y_threshold;
};

struct pinch_state {
	float last_scale;
	float last_threshold;
};

union event_inner_state {
	int not_initialised;
	struct swipe_state swipe;
	struct pinch_state pinch;
};

struct event_state {
	enum gesture_type event_type;
	uint32_t start_time;
	union event_inner_state s;
};

#endif
