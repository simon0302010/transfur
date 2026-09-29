#ifndef CONSOLE_H
#define CONSOLE_H

struct consolesize {
        int rows;
        int columns;
};

struct consolesize get_console_size(void);

#endif