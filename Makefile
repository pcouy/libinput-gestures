# Copyright (C) 2015 Mark Blakeney. This program is distributed under
# the terms of the GNU General Public License.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or any
# later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License at <http://www.gnu.org/licenses/> for more
# details.

libinput-gestures: libinput-gestures.c libinput-gestures.h config.c config.h
	gcc -g libinput-gestures.c config.c libcyaml/build/release/libcyaml.a -I libcyaml/include -o libinput-gestures `pkg-config --cflags --libs libinput libudev yaml-0.1`
	sudo chown root:input libinput-gestures
	sudo chmod g+s libinput-gestures
	sudo chmod u+s libinput-gestures
	sudo chmod o+w libinput-gestures
