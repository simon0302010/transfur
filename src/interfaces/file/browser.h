#ifndef BROWSER_H
#define BROWSER_H

#include <stdio.h>

#ifdef OS_MSDOS
/* https://delorie.com/djgpp/doc/rbinter/id/30/28.html */
#define PATH_MAX 128
#elif defined(OS_WINDOWS)
#include <windows.h>
#define PATH_MAX MAX_PATH
#define NAME_MAX 255
#else /* This should cover POSIX */
#include <limits.h>
#ifndef PATH_MAX /* PATH_MAX may not exist */
#define PATH_MAX 4096
#endif
#ifndef NAME_MAX
#define NAME_MAX 255
#endif
#endif

typedef unsigned char browser_options_t;

/* Possible browser_options */
#define BROWSER_FILES (1u << 0)
#define BROWSER_DIRECTORIES (1u << 1)

/* I don't think this needs to hold more */
struct browser_ctx {
        char location[PATH_MAX];
};

/* Whether a browser_entry is a file or directory */
enum browser_entry_type {
        browser_entry_directory,
        browser_entry_file,
        browser_entry_unknown
};

/* A browser entry returned by functions like browser_dir */
struct browser_entry {
        char name[NAME_MAX + 1];
        enum browser_entry_type entry_type;
};

/* Initialized a new file browser using default options */
int browser_init_default(struct browser_ctx *ctx);

/*
Writes N bytes of the current path stored inside the browser context to BUF
*/
int browser_getcwd(struct browser_ctx *ctx, char *buf, size_t n);

/* Returns a list of elements in the current folder */
int browser_dir(struct browser_ctx *ctx, browser_options_t options,
                struct browser_entry **entries, size_t *entry_count);

/* Moves to the specified relative or absolute path */
int browser_cd(struct browser_ctx *ctx, const char *path);

#endif