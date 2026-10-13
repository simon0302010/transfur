#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(OS_LINUX) || defined(OS_MACOS)
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(OS_WINDOWS)
#include <direct.h>
#include <windows.h>
#endif

#include "../../misc/bool.h"
#include "browser.h"

int browser_init_default(struct browser_ctx *ctx) {
        char cwd[PATH_MAX];

        if (ctx == NULL)
                return EINVAL;

#ifdef OS_WINDOWS
        if (_getcwd(cwd, PATH_MAX) == NULL) /* Untested */
                return 1;
#else
        if (getcwd(cwd, PATH_MAX) == NULL)
                return 1;
#endif

        strncpy(ctx->location, cwd, sizeof(ctx->location));
        ctx->location[sizeof(ctx->location) - 1] = '\0';

        return 0;
}

int browser_getcwd(struct browser_ctx *ctx, char *buf, size_t n) {
        if (ctx == NULL || buf == NULL || n == 0 || ctx->location[0] == '\0')
                return EINVAL;

        strncpy(buf, ctx->location, n);
        if (n > 0)
                buf[n - 1] = '\0'; /* Ensure null-termination */

        return 0;
}

/*
Windows implementation based on:
https://stackoverflow.com/questions/2314542/listing-directory-contents-using-c-and-windows
*/
int browser_dir(struct browser_ctx *ctx, browser_options_t options,
                struct browser_entry **entries, size_t *entry_count) {
#ifdef OS_WINDOWS
        WIN32_FIND_DATA fd_file;
        HANDLE h_find = NULL;
        struct dirent *entry;
        size_t entries_capacity;
#else
        struct dirent *entry;
        DIR *dp;
        size_t entries_capacity;
#endif

        if (ctx == NULL || entries == NULL || entry_count == NULL ||
            ctx->location[0] == '\0')
                return EINVAL;

#ifdef OS_WINDOWS

#else
        dp = opendir(ctx->location);
        if (dp == NULL) {
                perror("opendir");
                return 1;
        }

        *entry_count = 0;
        entries_capacity = 64;

        *entries = malloc(entries_capacity * sizeof(**entries));
        if (*entries == NULL) {
                closedir(dp);
                perror("malloc");
                return 1;
        }

        while ((entry = readdir(dp))) {
                /* d_type might sometimes be empty. Just hope it isn't. */
                switch (entry->d_type) {
                case DT_DIR:
                        if (!(options & BROWSER_DIRECTORIES))
                                continue;
                        (*entries)[*entry_count].entry_type =
                            browser_entry_directory;
                        break;
                case DT_REG:
                        if (!(options & BROWSER_FILES))
                                continue;
                        (*entries)[*entry_count].entry_type =
                            browser_entry_file;
                        break;
                default:
                        (*entries)[*entry_count].entry_type =
                            browser_entry_unknown;
                        break;
                }

                if (!(options & BROWSER_HIDDEN) && entry->d_name[0] == '.')
                        continue;

                strcpy((*entries)[*entry_count].name, entry->d_name);

                if (++*entry_count >= entries_capacity) {
                        struct browser_entry *tmp;

                        entries_capacity *= 2;
                        tmp = realloc(*entries,
                                      entries_capacity * sizeof(**entries));

                        if (tmp == NULL) {
                                closedir(dp);
                                free(*entries);
                                return 1;
                        }

                        *entries = tmp;
                }
        }

        closedir(dp);
#endif

        return 0;
}

int browser_cd(struct browser_ctx *ctx, const char *path, tbool relative) {
        struct stat s = {0};
        char full_path[PATH_MAX + 1];
        char real_path[PATH_MAX + 1];
        int written;

        if (path == NULL || ctx == NULL || path[0] == '\0' ||
            ctx->location[0] == '\0')
                return EINVAL;

        if (relative) {
                written = snprintf(full_path, sizeof(full_path), "%s/%s",
                                   ctx->location, path);
        } else {
                written = snprintf(full_path, sizeof(full_path), "%s", path);
        }

        if (written < 0 || (size_t)written >= sizeof(full_path))
                return ENAMETOOLONG;

        if (stat(full_path, &s) == -1) {
                perror("stat");
                return errno;
        }

        if (!S_ISDIR(s.st_mode))
                return ENOTDIR;

        if (realpath(full_path, real_path) == NULL) {
                perror("realpath");
                return errno;
        }

        strncpy(ctx->location, real_path, PATH_MAX);
        ctx->location[sizeof(ctx->location) - 1] = '\0';

        return 0;
}