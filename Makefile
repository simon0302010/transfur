CC = gcc
CFLAGS = -std=c89 -Wdeclaration-after-statement -Werror=declaration-after-statement -pedantic-errors -include src/platform.h -DTRANSFUR_DEBUG
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
	src/interfaces/wan/wan.c \
	src/interfaces/lan/discover.c \
	src/interfaces/lan/pip.c \
	src/interfaces/serial/serial.c \
	src/interfaces/bluetooth/bluetooth.c \
	src/relay/relay.c \
	src/misc/console.c \
	src/misc/debug.c \
	src/misc/ttime.c \
	src/misc/be32.c \
	src/gui/app/app.c \
	src/gui/core/ui_core.c \
	src/gui/core/widgets/group.c \
	src/gui/core/widgets/loading_bar.c \
	src/gui/core/widgets/progress_bar.c \
	src/gui/core/widgets/text.c \
	src/gui/core/widgets/text_input.c \
	src/gui/core/widgets/button.c \
	src/misc/debug.c \
	src/gui/core/widgets/split.c

TUI_SRC = src/gui/tui/widgets/split.c \
	src/gui/tui/widgets/group.c \
	src/gui/tui/widgets/button.c \
	src/gui/tui/raw.c \
	src/gui/tui/render.c \
	src/gui/tui/input.c \
	src/gui/tui/tui.c \
	src/gui/tui/widgets/loading_bar.c \
	src/gui/tui/widgets/progress_bar.c \
	src/gui/tui/widgets/text_input.c \
	src/gui/tui/widgets/text.c

BASIC_SRC = src/gui/basic/basic.c \
	src/gui/basic/render.c \
	src/gui/basic/event.c \
	src/gui/basic/text.c \
	src/gui/basic/widgets/loading_bar.c \
	src/gui/basic/widgets/progress_bar.c \
	src/gui/basic/widgets/text_input.c \
	src/gui/basic/widgets/text.c \
	src/gui/basic/design.c \
	src/gui/basic/widgets/button.c \
	src/gui/basic/design.c \
	src/gui/basic/drawing.c \
	src/gui/basic/widgets/split.c \
	src/gui/basic/widgets/group.c

ifeq ($(GUI),basic)
    CFLAGS += -DUSE_BASIC_GUI
    SRC += $(BASIC_SRC)
else ifeq ($(GUI),tui)
    CFLAGS += -DUSE_TUI
    SRC += $(TUI_SRC)
endif

ifeq ($(OS),Windows_NT)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/windows/nuklear_windows.c
	LDLIBS = -lmingw32 -lSDL2main -lSDL2 -lws2_32 -lm
else ifeq ($(shell uname -s),Linux)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/linux/nuklear_linux.c src/gui/basic/wrappers/sdl3.c src/interfaces/file/browser.c
	LDLIBS += -lX11 -lpthread -lbluetooth $(shell pkg-config --libs sdl3)
	CFLAGS += -D_POSIX_C_SOURCE=199309 -D_DEFAULT_SOURCE $(shell pkg-config --cflags sdl3)
else ifeq ($(shell uname -s),Darwin)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/macos/nuklear_macos.c
	CFLAGS += -D_DEFAULT_SOURCE $(shell pkg-config --cflags sdl2)
	LDLIBS = $(shell pkg-config --libs sdl2) -lpthread -lm
endif

ASM_SRC = src/gui/basic/font.S

ENABLE_THEMES ?= 1

ifeq ($(ENABLE_THEMES), 1)
	CFLAGS += -DENABLE_THEMES
	SRC += src/gui/basic/images.c src/gui/basic/theme.c
	ASM_SRC += src/gui/basic/image_assets.S
endif

OBJ = $(SRC:src/%.c=build/$(OS_NAME)/%.o) $(ASM_SRC:src/%.S=build/$(OS_NAME)/%.o)

$(TARGET): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

build/$(OS_NAME)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/$(OS_NAME)/%.o: src/%.S
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

test:
	mkdir -p build/$(OS_NAME)
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_file tests/file.test.c src/interfaces/file/file.c src/misc/debug.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_serial_local tests/serial_local.test.c src/interfaces/serial/serial.c src/interfaces/file/file.c src/misc/debug.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_lan_local tests/lan_local.test.c src/interfaces/lan/lan.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_pip tests/pip.test.c src/interfaces/lan/pip.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_file_browser tests/file_browser.test.c src/interfaces/file/browser.c

.PHONY: run clean