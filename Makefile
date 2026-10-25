CC = cc
CFLAGS = -std=c89 \
	-Wdeclaration-after-statement \
	-Werror=declaration-after-statement \
	-Wimplicit-function-declaration \
	-Werror=implicit-function-declaration \
	-pedantic-errors \
	-include src/platform.h \
	-DTRANSFUR_DEBUG
LDLIBS = -lm

ifeq ($(OS),Windows_NT)
	OS_NAME := windows
	EXE := .exe
	LDLIBS += -lws2_32 -lwininet
	COMMON_SRC_EXTRA = src/relay/relay.c
else
	OS_NAME := $(shell uname -s | tr '[:upper:]' '[:lower:]')
	EXE :=
	ifeq ($(shell uname -s),Linux)
		CFLAGS += -D_POSIX_C_SOURCE=199309 -D_DEFAULT_SOURCE $(shell pkg-config --cflags ao)
		LDLIBS += -lpthread $(shell pkg-config --libs ao)
		COMMON_SRC_EXTRA = src/relay/relay.c
		ifeq ($(shell pkg-config --exists bluez || test -f /usr/include/bluetooth/bluetooth.h && echo 1 || echo 0),1)
			CFLAGS += -DHAS_BLUETOOTH
			LDLIBS += -lbluetooth
		endif
	else ifeq ($(shell uname -s),Darwin)
		CFLAGS += -D_DEFAULT_SOURCE
		LDLIBS += -lpthread
	endif
endif

#TARGET = build/$(OS_NAME)/transfur
#ASM_SRC =
COMMON_SRC = src/main.c \
	src/interfaces/interfaces.c \
	src/interfaces/file/file.c \
	src/interfaces/lan/lan.c \
	src/interfaces/wan/wan.c \
	src/interfaces/audio/audio.c \
	src/interfaces/discovery.c \
	src/interfaces/lan/lan_discover.c \
	src/interfaces/bluetooth/bluetooth.c \
	src/interfaces/bluetooth/bt_discover.c \
	src/interfaces/lan/pip.c \
	src/interfaces/serial/serial.c \
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
	src/gui/core/widgets/split.c \
	${COMMON_SRC_EXTRA}

# NUKLEAR GUI
NUKLEAR_CFLAGS = $(CFLAGS)
NUKLEAR_LDLIBS = $(LDLIBS)
NUKLEAR_SRC = $(COMMON_SRC) src/gui/nuklear/gui_nuklear.c src/interfaces/file/browser.c

ifeq ($(OS),Windows_NT)
	NUKLEAR_SRC += src/gui/nuklear/windows/nuklear_windows.c
	NUKLEAR_LDLIBS += -ld3d9
else ifeq ($(shell uname -s),Linux)
	NUKLEAR_SRC += src/gui/nuklear/linux/nuklear_linux.c
	NUKLEAR_LDLIBS += -lX11
else ifeq ($(shell uname -s),Darwin)
	NUKLEAR_SRC += src/gui/nuklear/macos/nuklear_macos.c
	NUKLEAR_CFLAGS += $(shell pkg-config --cflags sdl2)
	NUKLEAR_LDLIBS += $(shell pkg-config --libs sdl2)
endif

# BASIC GUI
BASIC_CFLAGS = $(CFLAGS) -DUSE_BASIC_GUI
BASIC_LDLIBS = $(LDLIBS)
BASIC_SRC = $(COMMON_SRC) \
	src/gui/basic/basic.c \
	src/gui/basic/render.c \
	src/gui/basic/event.c \
	src/gui/basic/assets.c \
	src/gui/basic/text.c \
	src/gui/basic/widgets/loading_bar.c \
	src/gui/basic/widgets/progress_bar.c \
	src/gui/basic/widgets/text_input.c \
	src/gui/basic/widgets/text.c \
	src/gui/basic/design.c \
	src/gui/basic/widgets/button.c \
	src/gui/basic/drawing.c \
	src/gui/basic/widgets/split.c \
	src/gui/basic/widgets/group.c

ENABLE_THEMES ?= 0

ifeq ($(ENABLE_THEMES), 1)
	BASIC_CFLAGS += -DENABLE_THEMES
	BASIC_SRC += src/gui/basic/images.c src/gui/basic/theme.c src/gui/basic/theme_switcher.c \
		src/gui/basic/wallpaper.c \
		src/gui/basic/wallpapers/colors.c \
		src/gui/basic/wallpapers/quilt.c
endif

ifeq ($(shell uname -s),Linux)
	BASIC_SRC += src/gui/basic/wrappers/sdl2.c
	BASIC_CFLAGS += $(shell pkg-config --cflags sdl2)
	BASIC_LDLIBS += $(shell pkg-config --libs sdl2)
else ifeq ($(shell uname -s),Darwin)
	BASIC_SRC += src/gui/basic/wrappers/sdl2.c
	BASIC_CFLAGS += $(shell pkg-config --cflags sdl2)
	BASIC_LDLIBS += $(shell pkg-config --libs sdl2)
else
	BASIC_SRC += src/gui/basic/wrappers/sdl2.c
	BASIC_LDLIBS += -lSDL2main -lSDL2
endif

# TUI GUI

TUI_CFLAGS = $(CFLAGS) -DUSE_TUI
TUI_LDLIBS = $(LDLIBS)
TUI_SRC = $(COMMON_SRC) \
    src/gui/tui/widgets/split.c \
	src/gui/tui/widgets/group.c \
	src/gui/tui/widgets/button.c \
	src/gui/tui/raw.c \
	src/gui/tui/render.c \
	src/gui/tui/input.c \
	src/gui/tui/tui.c \
	src/gui/tui/widgets/loading_bar.c \
	src/gui/tui/widgets/progress_bar.c \
	src/gui/tui/widgets/text_input.c \
	src/gui/tui/widgets/text.c \
	src/gui/tui/text.c

BIN_NUKLEAR = build/$(OS_NAME)/transfur-nuklear$(EXE)
BIN_BASIC = build/$(OS_NAME)/transfur-basic$(EXE)
BIN_TUI = build/$(OS_NAME)/transfur-tui$(EXE)
TARGET = build/$(OS_NAME)/transfur$(EXE)

NUKLEAR_OBJ = $(NUKLEAR_SRC:src/%.c=build/$(OS_NAME)/nuklear/%.o)
BASIC_OBJ = $(BASIC_SRC:src/%.c=build/$(OS_NAME)/basic/%.o)
TUI_OBJ = $(TUI_SRC:src/%.c=build/$(OS_NAME)/tui/%.o)

ifeq ($(GUI),basic)
.DEFAULT_GOAL := basic
else ifeq ($(GUI),tui)
.DEFAULT_GOAL := tui
else ifeq ($(GUI),nuklear)
.DEFAULT_GOAL := nuklear
else
.DEFAULT_GOAL := all
endif

all: $(BIN_NUKLEAR) $(BIN_BASIC) $(BIN_TUI) $(TARGET)

nuklear: $(BIN_NUKLEAR) $(TARGET)
basic: $(BIN_BASIC)
tui: $(BIN_TUI)

$(TARGET): $(BIN_NUKLEAR)
	mkdir -p $(dir $@)
	cp -f $< $@

$(BIN_NUKLEAR): $(NUKLEAR_OBJ)
	mkdir -p $(dir $@)
	$(CC) $(NUKLEAR_CFLAGS) $^ -o $@ $(NUKLEAR_LDLIBS)

$(BIN_BASIC): $(BASIC_OBJ)
	mkdir -p $(dir $@)
	$(CC) $(BASIC_CFLAGS) $^ -o $@ $(BASIC_LDLIBS)

$(BIN_TUI): $(TUI_OBJ)
	mkdir -p $(dir $@)
	$(CC) $(TUI_CFLAGS) $^ -o $@ $(TUI_LDLIBS)

build/$(OS_NAME)/nuklear/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(NUKLEAR_CFLAGS) -c $< -o $@

build/$(OS_NAME)/basic/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(BASIC_CFLAGS) -c $< -o $@

build/$(OS_NAME)/tui/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(TUI_CFLAGS) -c $< -o $@

run: $(BIN_NUKLEAR)
	./$(BIN_NUKLEAR)

run-basic: $(BIN_BASIC)
	./$(BIN_BASIC)

run-tui: $(BIN_TUI)
	./$(BIN_TUI)

clean:
	rm -rf build

test:
	mkdir -p build/$(OS_NAME)
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_file tests/file.test.c src/interfaces/file/file.c src/misc/debug.c src/misc/be32.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_serial_local tests/serial_local.test.c src/interfaces/serial/serial.c src/interfaces/file/file.c src/misc/debug.c src/misc/be32.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_lan_local tests/lan_local.test.c src/interfaces/lan/lan.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_pip tests/pip.test.c src/interfaces/lan/pip.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_file_browser tests/file_browser.test.c src/interfaces/file/browser.c
	$(CC) $(CFLAGS) -I. -o build/$(OS_NAME)/test_audio tests/audio.test.c src/interfaces/audio/audio.c src/misc/be32.c src/misc/bool.h $(LDLIBS)

.PHONY: all nuklear basic tui run run-basic run-tui clean test