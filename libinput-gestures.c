#define _POSIX_C_SOURCE 201609L

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <libudev.h>
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <string.h>
#include <libinput.h>
#include <errno.h>

#include "libinput-gestures.h"

int main(int argc, char* argv[]){
	printf("Rewrite of libinput-gestures using suid/sgid\n");
	printf("\n");
	printf("\tUsage : libinput-gestures-setgid [\"u\"|\"g\"]\n\n");

	if (argc >= 2 && strcmp(argv[1], "u") == 0) {
		try_root_suid();
	} else {
		try_input_sgid();
	}

	monitor_events();

	printf("exit\n");
}

void monitor_events()
{
	const static struct libinput_interface interface = {
		.open_restricted = open_restricted,
		.close_restricted = close_restricted,
	};

	struct libinput *li;
	struct libinput_event *event;
	struct timespec sleep_delay = {
		.tv_sec = 0,
		.tv_nsec = 500 * 1000,
	};
	struct event_state state;

	li = libinput_udev_create_context(&interface, NULL, udev_new());
	libinput_udev_assign_seat(li, get_seat());
	libinput_dispatch(li);

	while (1) {
		event = libinput_get_event(li);
		if (event != NULL && libinput_event_get_type(event) != LIBINPUT_EVENT_DEVICE_ADDED){
			state = handle_event(event, state);
		} else {
			nanosleep(&sleep_delay, NULL);
		}
		libinput_event_destroy(event);
		libinput_dispatch(li);
	}

}

struct event_state handle_event(struct libinput_event *event, struct event_state state)
{
	enum libinput_event_type type = libinput_event_get_type(event);
	enum libinput_event_type allowed_types[] = {LIBINPUT_EVENT_GESTURE_HOLD_BEGIN, LIBINPUT_EVENT_GESTURE_HOLD_END, LIBINPUT_EVENT_GESTURE_PINCH_BEGIN, LIBINPUT_EVENT_GESTURE_PINCH_END, LIBINPUT_EVENT_GESTURE_PINCH_UPDATE, LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN, LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE, LIBINPUT_EVENT_GESTURE_SWIPE_END, LIBINPUT_EVENT_NONE}; // Add LIBINPUT_EVENT_NONE at the end as a guard
	
	int i = 0;
	while ( allowed_types[i] != type ) {
		if (allowed_types[i] == LIBINPUT_EVENT_NONE) return state;
		i++;
	}

	struct libinput_event_gesture *gesture = libinput_event_get_gesture_event(event);
	if (gesture == NULL) return state;
	
#ifdef DEBUG
	print_gesture(gesture);
#endif

	struct gesture_breakdown breakdown = gesture_to_breakdown(gesture);
	
	if (breakdown.step == CANCEL) {
		return new_state();
	}
	if (breakdown.step == BEGIN) {
#ifndef DEBUG
		print_gesture(gesture);
#endif
		if (state.start_time != 0 || state.event_type != ERR_TYPE) {
			printf("Error : Gesture begin before previous gesture end or cancel\n");
			printf("Discarding previous gesture\n");
			state = new_state();
		}
		state.start_time = libinput_event_gesture_get_time(gesture);
		state.event_type = breakdown.type;
		return state;
	}
	if (breakdown.type != state.event_type) {
		printf("Error : Type mismatch between event and state\n");
		printf("Discarding event, continuing with previous gesture\n");
		return state;
	}

	state = dispatch_gesture(breakdown, gesture, state);

	return state;
}

struct event_state dispatch_gesture(
		struct gesture_breakdown breakdown,
		struct libinput_event_gesture* gesture,
		struct event_state state)
{
	if(breakdown.type == SWIPE && breakdown.step == UPDATE) return handle_swipe_update(gesture, state);
	if(breakdown.type == SWIPE && breakdown.step == END) return handle_swipe_end(gesture, state);
	if(breakdown.type == PINCH && breakdown.step == UPDATE) return handle_pinch_update(gesture, state);
	if(breakdown.type == PINCH && breakdown.step == END) return handle_pinch_end(gesture, state);
	if(breakdown.type == HOLD && breakdown.step == END) return handle_hold_end(gesture, state);

	printf("Error : Did not identify gesture\n");
	return state;
}

struct event_state handle_swipe_update(struct libinput_event_gesture *gesture, struct event_state state)
{
	float dx = libinput_event_gesture_get_dx(gesture);
	float last_cumulative_dx = state.s.swipe.cumulative_dx;
	state.s.swipe.cumulative_dx+= dx;

	float dy = libinput_event_gesture_get_dy(gesture);
	float last_cumulative_dy = state.s.swipe.cumulative_dy;
	state.s.swipe.cumulative_dy+= dy;

	return state;
}

struct event_state handle_pinch_update(struct libinput_event_gesture *gesture, struct event_state state)
{
	state.s.pinch.last_scale = libinput_event_gesture_get_scale(gesture);
	return state;
}

struct event_state handle_swipe_end(struct libinput_event_gesture *gesture, struct event_state state)
{
	print_gesture(gesture);
	printf("Cumulative dx=%f dy=%f (started at %d)\n", state.s.swipe.cumulative_dx, state.s.swipe.cumulative_dy, state.start_time);
	return new_state();
}

struct event_state handle_pinch_end(struct libinput_event_gesture *gesture, struct event_state state)
{
	print_gesture(gesture);
	printf("Final scale = %f (started at %d)\n", state.s.pinch.last_scale, state.start_time);
	return new_state();
}

struct event_state handle_hold_end(struct libinput_event_gesture *gesture, struct event_state state)
{
	return new_state();
}

struct event_state new_state()
{
	struct event_state s = {
		.event_type = ERR_TYPE,
		.start_time = 0,
		.s.not_initialised = 1,
	};
	return s;
}

void try_input_sgid() {
	gid_t gid = getgrnam("input")->gr_gid;

	if (setgid(gid) != 0){
		printf("Could not set gid to input group (id=%d)\n", gid);
	} else {
		printf("Successfully set gid to input group (id=%d)\n", gid);
	}
}

void try_root_suid() {
	uid_t uid = 0;

	if (setuid(uid) != 0){
		printf("Could not set uid to 0\n");
	} else {
		printf("Successfully set uid to 0\n");
	}
}

static int open_restricted(const char *path, int flags, void *user_data)
{
        int fd = open(path, flags);
        return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data)
{
        close(fd);
}

struct gesture_breakdown gesture_to_breakdown(struct libinput_event_gesture *gesture)
{
	struct gesture_breakdown result;
	enum libinput_event_type event_type = libinput_event_get_type(libinput_event_gesture_get_base_event(gesture));	

	switch (event_type) {
		case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
		case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
		case LIBINPUT_EVENT_GESTURE_SWIPE_END:
			result.type = SWIPE;
			break;
		case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
		case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
		case LIBINPUT_EVENT_GESTURE_PINCH_END:
			result.type = PINCH;
			break;
		case LIBINPUT_EVENT_GESTURE_HOLD_BEGIN:
		case LIBINPUT_EVENT_GESTURE_HOLD_END:
			result.type = HOLD;
			break;
		default:
			result.type = ERR_TYPE;
			break;
	}

	switch (event_type) {
		case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
		case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
		case LIBINPUT_EVENT_GESTURE_HOLD_BEGIN:
			result.step = BEGIN;
			break;
		case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
		case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
			result.step = UPDATE;
			break;
		case LIBINPUT_EVENT_GESTURE_PINCH_END:
		case LIBINPUT_EVENT_GESTURE_SWIPE_END:
		case LIBINPUT_EVENT_GESTURE_HOLD_END:
			if (libinput_event_gesture_get_cancelled(gesture)) {
				result.step = CANCEL;
				break;
			}
			result.step = END;
			break;
		default:
			result.step = ERR_STEP;
			break;
	}

	return result;
}

void print_gesture(struct libinput_event_gesture *gesture)
{
	printf("[%d] ", libinput_event_gesture_get_time(gesture));

	struct gesture_breakdown breakdown;
	breakdown = gesture_to_breakdown(gesture);

	switch (breakdown.type) {
		case SWIPE:
			printf("Swipe ");
			break;
		case PINCH:
			printf("Pinch ");
			break;
		case HOLD:
			printf("Hold ");
			break;
		default:
			break;
	}

	printf("%d fingers ", libinput_event_gesture_get_finger_count(gesture));

	switch (breakdown.step) {
		case BEGIN:
			printf("start ");
			break;
		case UPDATE:
			printf("update ");
			break;
		case CANCEL:
			printf("cancelled ");
			break;
		case END:
			printf("end ");
			break;
		default:
			break;
	}

	if (breakdown.step == UPDATE) {
		if (breakdown.type == PINCH) {
			printf("Scale:%f ", libinput_event_gesture_get_scale(gesture));
		} else if (breakdown.type == SWIPE) {
			printf("Delta ");
			printf("X:%f ", libinput_event_gesture_get_dx(gesture));
			printf("Y:%f ", libinput_event_gesture_get_dy(gesture));
			printf("Delta (u) ");
			printf("X:%f ", libinput_event_gesture_get_dx_unaccelerated(gesture));
			printf("Y:%f ", libinput_event_gesture_get_dy_unaccelerated(gesture));
		}
	}

	printf("\n");
}

char* get_seat()
{
	char* dst = getenv("XDG_SEAT");
	if (dst == NULL) {
		printf("Could not read `XDG_SEAT` env variable, using `seat0`\n");
		dst = "seat0";
	}
	return dst;
}