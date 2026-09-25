CC = gcc
CFLAGS = -std=c89 -Wdeclaration-after-statement -Werror=declaration-after-statement -pedantic-errors
LDLIBS = -lm

TARGET = build/transfur
SRC = src/main.c \
	src/interfaces/interfaces.c \
	src/interfaces/file/file.c \
	src/interfaces/lan/lan.c \
	src/interfaces/serial/serial.c
OBJ = $(SRC:src/%.c=build/%.o)

ifeq ($(OS),Windows_NT)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/windows/nuklear_windows.c
	LDLIBS = -lmingw32 -lSDL2main -lSDL2 -lm
else ifeq ($(shell uname -s),Linux)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/linux/nuklear_linux.c
	LDLIBS += -lX11
	CFLAGS += -D_POSIX_C_SOURCE=199309
else ifeq ($(shell uname -s),Darwin)
	SRC += src/gui/nuklear/gui_nuklear.c src/gui/nuklear/macos/nuklear_macos.c
	CFLAGS += $(shell pkg-config --cflags sdl2)
	LDLIBS = $(shell pkg-config --libs sdl2) -lm
endif

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

.PHONY: run clean