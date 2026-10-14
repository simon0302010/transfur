WATCOM ?= /opt/watcom
export WATCOM
export EDPATH := $(WATCOM)/eddat
export INCLUDE := $(WATCOM)/h

CC := owcc -b dos
CFLAGS := -std=c89 -mcmodel=l -O2 -Wall -include src/platform.h -DTRANSFUR_DEBUG
LDLIBS :=
TARGET := build/dos/transfur.exe

SRC = src/main.c \
	src/interfaces/interfaces.c \
	src/interfaces/file/file.c \
	src/interfaces/serial/serial.c \
	src/misc/console.c \
	src/misc/debug.c \
	src/misc/ttime.c \
	src/gui/app/app.c \
	src/gui/core/ui_core.c \
	src/gui/core/widgets/group.c \
	src/gui/core/widgets/loading_bar.c \
	src/gui/core/widgets/progress_bar.c \
	src/gui/core/widgets/text.c \
	src/gui/core/widgets/text_input.c \
	src/gui/core/widgets/button.c \
	src/gui/tui/raw.c \
	src/gui/tui/render.c \
	src/gui/tui/input.c \
	src/gui/tui/tui.c \
	src/gui/tui/widgets/loading_bar.c \
	src/gui/tui/widgets/progress_bar.c \
	src/gui/tui/widgets/text_input.c \
	src/gui/tui/widgets/text.c \
	src/misc/debug.c \
	src/gui/basic/basic.c \
	src/gui/basic/render.c \
	src/gui/basic/event.c \
	src/gui/basic/text.c \
	src/gui/basic/widgets/loading_bar.c \
	src/gui/basic/widgets/progress_bar.c \
	src/gui/basic/widgets/text_input.c \
	src/gui/basic/widgets/text.c \
	src/gui/basic/design.c \
	src/gui/tui/widgets/button.c \
	src/gui/basic/widgets/button.c \
	src/gui/basic/design.c \
	src/gui/basic/drawing.c \
	src/gui/basic/widgets/split.c \
	src/gui/core/widgets/split.c \
	src/gui/basic/widgets/group.c \
	src/gui/tui/widgets/split.c \
	src/gui/tui/widgets/group.c

OBJ = $(SRC:src/%.c=build/%.o) $(ASM_SRC:src/%.S=build/%.o)

$(TARGET): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: src/%.S
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

.PHONY: run clean