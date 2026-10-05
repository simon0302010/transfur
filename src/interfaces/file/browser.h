#ifndef BROWSER_H
#define BROWSER_H

#define BROWSER_MAX_PATH 480
#define BROWSER_MAX_NAME 255
#define BROWSER_MAX_ENTRIES 1024

int browser_open(const char *start_path);
void browser_close(void);
int browser_is_open(void);

const char *browser_dir(void);
int browser_count(void);
const char *browser_name(int index);
int browser_is_dir(int index);
int browser_path(int index, char *out, int out_size);

int browser_enter(int index);
int browser_up(void);

#endif