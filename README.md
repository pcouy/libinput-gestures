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

After cloning the repo, `cd` into it and edit `config.h`. `make
libinput-gestures` builds the executable and changes the permissions on it to
enable `setgid(...)` (requires root privileges, you will be prompted via
`sudo`). Running the program **does _NOT_ require you to add your user to the
`input` group** : just run `./libinput-gestures`.

### Configuration

For now, configuration is done through editing the `config.h` file and re-building the project (inspired by what's done in many [suckless](https://suckless.org/) projects). I plan on supporting runtime configuration files later.

The provided `config.h` file contains comments to show the expected syntax. Configuration makes intensive use of the enums and structures defined in `libinput-gestures.h`
