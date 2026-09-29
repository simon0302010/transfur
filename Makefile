CC = gcc
CFLAGS = -std=c89 -Wdeclaration-after-statement -Werror=declaration-after-statement -pedantic-errors
LDLIBS = -lm

ifeq ($(OS),Windows_NT)
	OS_NAME := windows
else 
	OS_NAME := $(shell uname -s)
endif

TARGET = build/$(OS_NAME)/transfur
SRC = src/main.c \
	src/interfaces/interfaces.c \
	src/interfaces/file/file.c \
	src/interfaces/lan/lan.c \
	src/interfaces/serial/serial.c \
	src/gui/tui/raw.c \
	src/gui/tui/tui.c \
	src/misc/ttime.c \
	src/gui/basic/sdl3/basic_wrapper.c \
	src/gui/basic/basic.c \
	src/gui/basic/text.c \
	src/misc/console.c
OBJ = $(SRC:src/%.c=build/%.o)

ifeq ($(OS),Windows_NT)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/windows/nuklear_windows.c
	LDLIBS = -lmingw32 -lSDL2main -lSDL2 -lws2_32 -lm
else ifeq ($(shell uname -s),Linux)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/linux/nuklear_linux.c
	LDLIBS += -lX11
	CFLAGS += -D_POSIX_C_SOURCE=199309
else ifeq ($(shell uname -s),Darwin)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/macos/nuklear_macos.c
	CFLAGS += $(shell pkg-config --cflags sdl2)
	LDLIBS = $(shell pkg-config --libs sdl2) -lm
endif

CFLAGS += $(shell pkg-config --cflags sdl3)
LDLIBS += $(shell pkg-config --libs sdl3)

$(TARGET): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

test:
	mkdir -p build
	gcc $(CFLAGS) -I. -o build/test_file tests/file.test.c src/interfaces/file/file.c
	gcc $(CFLAGS) -I. -o build/test_serial_local tests/serial_local.test.c src/interfaces/serial/serial.c src/interfaces/file/file.c
	gcc $(CFLAGS) -I. -o build/test_lan_local tests/lan_local.test.c src/interfaces/lan/lan.c

.PHONY: run clean