CC = gcc
CFLAGS = -std=c89 -Wdeclaration-after-statement -Werror=declaration-after-statement -pedantic-errors -include src/platform.h
LDLIBS = -lm

ifeq ($(OS),Windows_NT)
	OS_NAME := windows
else
	OS_NAME := $(shell uname -s | tr '[:upper:]' '[:lower:]')
endif

TARGET = build/$(OS_NAME)/transfur
SRC = src/main.c \
	src/interfaces/interfaces.c \
	src/interfaces/file/file.c \
	src/interfaces/lan/lan.c \
	src/interfaces/serial/serial.c \
	src/misc/console.c \
	src/misc/ttime.c \
	src/gui/app/app.c \
	src/gui/core/ui_core.c \
	src/gui/core/widgets/group.c \
	src/gui/core/widgets/loading_bar.c \
	src/gui/core/widgets/progress_bar.c \
	src/gui/core/widgets/text.c \
	src/gui/core/widgets/text_input.c \
	src/gui/tui/raw.c \
	src/gui/tui/render.c \
	src/gui/tui/input.c \
	src/gui/tui/tui.c \
	src/gui/tui/widgets/loading_bar.c \
	src/gui/tui/widgets/progress_bar.c \
	src/gui/tui/widgets/text_input.c \
	src/gui/tui/widgets/text.c \
	src/gui/basic/basic.c \
	src/gui/basic/render.c \
	src/gui/basic/event.c \
	src/gui/basic/text.c \
	src/gui/basic/widgets/text.c

ifeq ($(OS),Windows_NT)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/windows/nuklear_windows.c
	LDLIBS = -lmingw32 -lSDL2main -lSDL2 -lws2_32 -lm
else ifeq ($(shell uname -s),Linux)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/linux/nuklear_linux.c src/gui/basic/sdl3/basic_wrapper.c
	LDLIBS += -lX11 -lpthread $(shell pkg-config --libs sdl3)
	CFLAGS += -D_POSIX_C_SOURCE=199309 $(shell pkg-config --cflags sdl3)
else ifeq ($(shell uname -s),Darwin)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/macos/nuklear_macos.c
	CFLAGS += $(shell pkg-config --cflags sdl2)
	LDLIBS = $(shell pkg-config --libs sdl2) -lpthread -lm
endif

OBJ = $(SRC:src/%.c=build/%.o)

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