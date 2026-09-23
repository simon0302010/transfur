CC = gcc
CFLAGS = -std=c89 -Wdeclaration-after-statement -Werror=declaration-after-statement -pedantic-errors
LDLIBS = -lm

ifeq ($(OS),Windows_NT)
	LDLIBS = -lmingw32 -lSDL2main -lSDL2 -lm
else ifeq ($(shell uname -s),Linux)
	LDLIBS += -lX11
	CFLAGS += -D_POSIX_C_SOURCE=199309
else ifeq ($(shell uname -s),Darwin)
	CFLAGS += $(shell pkg-config --cflags sdl2)
	LDLIBS = $(shell pkg-config --libs sdl2) -lm
endif

TARGET = build/transfur
SRC = src/main.c src/gui/gui.c
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

.PHONY: run clean