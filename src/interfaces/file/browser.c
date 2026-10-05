#include "browser.h"

#include <stdlib.h>
#include <string.h>

#if defined(OS_LINUX || OS_MACOS)
#include <direnet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(OS_WINDOWS)
#include <windows.h>
#endif

struct browser_entry {
        char name[BROWSER_MAX_NAME + 1];
        int is_dir;
};

static int browser_opened = 0;
static char browser_dir_path[BROWSER_MAX_PATH] = "";
static struct browser_entry *browser_items = NULL;
static int browser_item_count = 0;

static void browser_join(char *out, int out_size, const char *dir, const char *name);
static void browser_parent(char *out, int out_size, const char *dir);
static int browser_push(const char *name, int is_dir);
static void browser_clear(void);
static int browser_cmp (const void *a, const void *b);
static int browser_is_abs(const char *path);
static void browser_abs(char *out, int out_size, const char *path);

static void browser_join(char *out, int out_size, const char *dir, const char *name) {
        size_t limit;
        size_t pos;
        size_t n;

        if (out_size <= 0) {
                return;
        }

        out[0] = '\0';
        limit = (size_t)out_size - 1;
        pos = 0;

        n = strlen(dir);
        if (n > limit) {
                n = limit;
        }
        memcpy(out, dir, n);
        pos = n;
        
        if (pos > 0 && out[pos - 1] != '/' && pos < limit) {
                out[pos] = '/';
                pos++;
        }

        n = strlen(name);
        if (n > limit - pos) {
                n = limit - pos;
        }
        memcpy(out + pos, name, n);
        pos += n;

        out[pos] = '\0';
}

static void browser_parent(char *out, int out_size, const char *dir) {
        size_t len;

        if (out_size <= 0) {
                return;
        }

        out[0] = '\0';
        len = strlen(dir);

        while (len > 1 && dir[len - 1] == '/') {
                len--;
        }

        while (len > 0 && dir[len - 1] != '/') {
                len--;
        }

        while (len > 1 && dir[len - 1] == '/') {
                len--;
        }

        if(len > (size_t)out_size - 1) {
                len = (size_t)out_size - 1;
        }

        memcpy(out, dir, len);
        out[len] = '\0';

        if (out[0] == '\0') {
                out[0] = '/';
                out[1] = '\0';
        }
}

static int browser_push(const char *name, int is_dir) {
        struct browser_entry *grown;
        int count;

        if (browser_item_count >= BROWSER_MAX_ENTRIES) {
                return -1;
        }

        if ((browser_item_count % 64) == 0) {
                grown = (struct browser_entry *)realloc(
                        browser_items, 
                        (size_t)(browser_item_count + 64) *
                            sizeof(struct browser_entry));
                if (grown == NULL) {
                        return -1;
                }
                browser_items = grown;
        }

        count = browser_item_count;
        strncpy(browser_items[count].name, name, BROWSER_MAX_NAME);
        browser_items[count].name[BROWSER_MAX_NAME] = '\0';
        browser_items[count].is_dir = is_dir;
        browser_item_count++;

        return 0;
}

static void browser_clear(void) {
        if (browser_items != NULL) {
                free(browser_items);
                browser_items = NULL;
        }

        browser_item_count = 0;
}

static int browser_cmp(const void *a, const void *b) {
        const struct browser_entry *ea;
        const struct browser_entry *eb;

        ea = (const struct browser_entry *)a;
        eb = (const struct browser_entry *)b;

        if (ea->is_dir != eb->is_dir) {
                return ea->is_dir ? -1 : 1;
        }

        return strcmp(ea->name, eb->name);
}

static int browser_is_abs(const char *path) {
#if defined(OS_WINDOWS) 
        if (((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) || path[1] == ':') {
                return 1;
        }

        return path[0] == '\\' || path[0] == '/';
#else
        return path[0] == '/';
#endif
}

static void browser_abs(char *out, int out_size, const char *path) {
        char cwd[BROWSER_MAX_PATH];
        int len;

        if (out_size <= 0) {
                return;
        }

        if (path == NULL) {
                path = "";
        }

        if (path[0] == '\0' || browser_is_abs(path)) {
                strncpy(out, path, (size_t)out_size - 1);
                out[out_size - 1] = '\0';
                return;
        }

        cwd[0] = '\0';

#if defined(OS_WINDOWS)
        len = GetCurrentDirectoryA((DWORD)sizeof cwd, cwd);
        if (len <= 0 || len >= (int)sizeof cwd) {
                cwd[0] = '\0';
        }
#else
        if (getcwd(cwd, sizeof cwd) == NULL) {
                cwd[0] = '\0';
        }
        len = (int)strlen(cwd);
#endif

        browser_join(out, out_size, cwd, path);
}

static int browser_read_dir(const char *dir) {
        char parent[BROWSER_MAX_PATH];
        int failed;

        browser_clear();

        failed = 0;
        browser_parent(parent, sizeof parent, dir);
        if (strncmp(parent, dir) != 0 && browser_push("..", 1) != 0) {
                failed = 1;
        }

#if defined(OS_LINUX) || defined(OS_MACOS)
        if (!failed) {
                DIR *handle;
                struct dirent *entry;
                struct stat info;
                char full[BROWSER_MAX_PATH];

                handle = opendir(dir);
                if (handle == NULL) {
                        failed = 1;
                }

                while (!failed && (entry = readdir(handle)) != NULL) {
                        int is_dir;

                        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                                continue;
                        }

                        browser_join(full, sizeof full, dir, entry->d_name);

                        is_dir = 0;
                        if (stat(full, &info) == 0) {
                                is_dir = S_ISDIR(info.st_mode) ? 1 : 0;
                        }

                        if (browser_push(entry->d_name, is_dir) != 0) {
                                failed = 1;
                        }
                }
                
                if (handle != NULL) {
                        closedir(handle);
                }
        }

        /* TODO: Finish Windows implementation */
}