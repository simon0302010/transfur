#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(OS_LINUX) || defined(OS_MACOS)
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(OS_WINDOWS)
#include <windows.h>
#endif

#include "browser.h"

int browser_init_default(struct browser_ctx *ctx) {
        char cwd[PATH_MAX];

        if (getcwd(cwd, PATH_MAX) == NULL)
                return 1;

        strncpy(ctx->location, cwd, sizeof(cwd));

        return 0;
}

int browser_getcwd(struct browser_ctx *ctx, char *buf, size_t n) {
        if (ctx->location[0] == '\0')
                return 1;

        strncpy(buf, ctx->location, n);
        if (n > 0)
                buf[n - 1] = '\0'; /* Ensure null-termination */

        return 0;
}

int browser_dir(struct browser_ctx *ctx, browser_options_t options,
                struct browser_entry **entries, size_t *entry_count) {
        struct dirent *entry;
        DIR *dp;
        size_t entries_capacity;

        if (ctx->location[0] == '\0')
                return 1;

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
                strcpy((*entries)[*entry_count].name, entry->d_name);

                /* d_type might sometimes be empty. Just hope it isn't. */
                switch (entry->d_type) {
                case (1u << 2):
                        (*entries)[*entry_count].entry_type =
                            browser_entry_directory;
                        break;
                case (1u << 3):
                        (*entries)[*entry_count].entry_type =
                            browser_entry_file;
                        break;
                default:
                        (*entries)[*entry_count].entry_type =
                            browser_entry_unknown;
                        break;
                }

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

        return 0;
}
