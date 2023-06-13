# `libinput-gestures`

This work is inspired by the [Python version of `libinput-gestures`](https://github.com/bulletmark/libinput-gestures).
This is still a work in progress.

## Motivation

From the [Python version's README](https://github.com/bulletmark/libinput-gestures/blob/master/README.md#installation) : 

> You **must be a member of the _input_ group** to have permission to read the
> touchpad device:

Adding your user to the `input` group is a security risk, as it allows any
process running under your session to read all inputs regardless of window
focus, including inputs from the keyboard.

The recommended way to run a program that accesses input devices without the
user being a member of the `input` group is to [use
`setgid()`](https://linuxhint.com/setuid-setgid-sticky-bit/). However, this is
not possible to do that using an interpreted language, as the `sgid` bit needs
to be set on the binary itself. Doing this with the Python version would
require changing the group ownership and setting the `sgid` bit on the Python
interpreter, which would allow any Python program on the computer to read all
inputs.

Rewriting this in C makes it possible to use `setgid()` in the proper way.

Moreover, I happened to have issues with the Python implementation where some
libinput messages would get stuck in the buffer.

## User manual (short draft)

While this project is still a bit rough around the edges, it currently provides
most of [the python version](https://github.com/bulletmark/libinput-gestures)'s
features :

- Ability to map gestures to external commands
- Handles 4 directions for swipes
- Handles pinch in and out
- Handles gesture thresholds and timeouts

After cloning the repo, `cd` into it and build the executable. `make
libinput-gestures` builds the executable and changes the permissions on it to
enable `setgid(...)` (requires root privileges, you will be prompted via
`sudo`). Running the program **does _NOT_ require you to add your user to the
`input` group** : just run `./libinput-gestures`.

### Configuration

Configuration is done through the `config.yaml` file. This file has two main sections :

`trigger_configs` lets you define named gesture configuration in the following format :

```yaml
trigger_configs:
  - name: default
  - name: swipe_threshold
    config:
      threshold: 50
  - name: swipe_timeout
    config:
      threshold: 50
      min_duration: 0     # No minimum duration for detecting the gesture
      max_duration: 3000  # The gesture will timeout 3s after being started
  - name: pinch_out
    config:
      threshold: 1.2
  - name: pinch_in
    config:
      threshold: 0.75
```

All values for the `config` parts are optional (as well as the config part itself). If omitted, parameters will default to 0.

`triggers` defines the conditions for triggering actions and the corresponding actions :

```yaml
triggers:
  - type: swipe
    direction: left
    fingers: 3
    trigger_on: end
    config: default
    command:
      - notify-send
      - Test yaml swipe left
  - type: swipe
    direction: right
    fingers: 3
    trigger_on: end
    config: default
    command:
      - notify-send
      - Test yaml swipe right
  - type: swipe
    fingers: 3
    trigger_on: end
    config: swipe_threshold
    command:
      - notify-send
      - Test yaml swipe vertical
  - type: pinch
    fingers: 2
    trigger_on: end
    config: pinch_out
    command:
      - notify-send
      - Test yaml pinch out 2 end
  - type: pinch
    fingers: 2
    trigger_on: end
    config: pinch_in
    command:
      - notify-send
      - Test yaml pinch in 2 end
```

Each trigger has the follwing fields :

- `type` (required) : `swipe` or `pinch`
- `fingers` (required) : integer
- `trigger_on` (required) : `end` or `threshold` or `repeat`
- `direction` (optional): `none` (default) or `left` or `right` or `up` or `down`. Only used for swipe gestures, not used for pinch gestures. `none` matches any direction when used with swipe gestures.
- `config` (required) : string matching the name of a config from the `trigger_configs` section
- `command` (required) : list of args for the command to run when this trigger is matched (the 1st is the command name)

## TODO notes

- Triggers that are not external program calls
- Clean seat detection ?
