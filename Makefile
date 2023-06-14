# PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

GCC_ARGS := libinput-gestures.c config.c libcyaml/build/release/libcyaml.a -I libcyaml/include -o libinput-gestures `pkg-config --cflags --libs libinput libudev yaml-0.1`
BUILD_DEPS := libinput-gestures.c libinput-gestures.h config.c config.h libcyaml

libinput-gestures: $(BUILD_DEPS)
	gcc $(GCC_ARGS)
	$(MAKE) permissions

.PHONY: debug
debug: $(BUILD_DEPS)
	gcc -g $(GCC_ARGS)
	$(MAKE) permissions

.PHONY: permissions
permissions:
	sudo chown root:input libinput-gestures
	sudo chmod g+s libinput-gestures
	sudo chmod u+s libinput-gestures
	sudo chmod o+w libinput-gestures

install: libinput-gestures
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 2755 -o root -g input libinput-gestures $(DESTDIR)$(PREFIX)/bin/

.PHONY: libcyaml
libcyaml:
	$(MAKE) -C libcyaml

.PHONY: clean
clean:
	rm -f libinput-gestures
	$(MAKE) -C libcyaml clean
