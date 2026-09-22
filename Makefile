CC = gcc
CFLAGS = -std=c89 -Wdeclaration-after-statement -Werror=declaration-after-statement -pedantic-errors
LDLIBS = -lm

ifeq ($(OS),Windows_NT)
  # Nothing for Windows
else ifeq ($(shell uname -s),Linux)
  LDLIBS += -lX11
else ifeq ($(shell uname -s),Darwin)
  # Nothing for macOS
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