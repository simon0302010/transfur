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
        char s_path[PATH_MAX];
#else
        struct dirent *entry;
        DIR *dp;
        size_t entries_capacity;
#endif

        if (ctx == NULL || entries == NULL || entry_count == NULL ||
            ctx->location[0] == '\0')
                return EINVAL;

#ifdef OS_WINDOWS
        snprintf(s_path, sizeof(s_path), "%s\\*.*", ctx->location);

        if ((h_find = FindFirstFile(s_path, &fd_file)) ==
            INVALID_HANDLE_VALUE) {
                fprintf(stderr, "FindFirstFile: No such file or directory\n");
                return ENOENT;
        }

        *entry_count = 0;
        entries_capacity = 64;

        *entries = malloc(entries_capacity * sizeof(**entries));
        if (*entries == NULL) {
                FindClose(h_find);
                perror("malloc");
                return 1;
        }

        do {
                char *name = (*entries)[*entry_count].name;

                if (fd_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        if (!(options & BROWSER_DIRECTORIES))
                                continue;
                        (*entries)[*entry_count].entry_type =
                            browser_entry_directory;
                } else {
                        if (!(options & BROWSER_FILES))
                                continue;
                        (*entries)[*entry_count].entry_type =
                            browser_entry_file;
                }

                strncpy(name, fd_file.cFileName, sizeof(name) - 1);
                name[sizeof(name) - 1] = '\0';

                if (++*entry_count >= entries_capacity) {
                        struct browser_entry *tmp;

                        entries_capacity *= 2;
                        tmp = realloc(*entries,
                                      entries_capacity * sizeof(**entries));

                        if (tmp == NULL) {
                                FindClose(h_find);

                                free(*entries);
                                return 1;
                        }

                        *entries = tmp;
                }
        } while (FindNextFile(h_find, &fd_file));

        FindClose(h_find);
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
#ifdef OS_WINDOWS
        DWORD dw_attrib;
        char full_path[PATH_MAX + 1];
        char real_path[PATH_MAX + 1];
        int written;

        if (path == NULL || ctx == NULL || path[0] == '\0' ||
            ctx->location[0] == '\0')
                return EINVAL;

        if (relative) {
                /* Points to current browser location */
                if (strcmp(path, ".") == 0)
                        return 0;
                written = snprintf(full_path, sizeof(full_path), "%s\\%s",
                                   ctx->location, path);
        } else {
                /* Points to current browser location */
                if (strcmp(path, ctx->location) == 0)
                        return 0;
                written = snprintf(full_path, sizeof(full_path), "%s", path);
        }

        if (written < 0 || (size_t)written >= sizeof(full_path))
                return ENAMETOOLONG;

        dw_attrib = GetFileAttributes(full_path);

        if (dw_attrib == INVALID_FILE_ATTRIBUTES) {
                return ENOENT; /* Probably the right error code for this */
        }

        if (!(dw_attrib & FILE_ATTRIBUTE_DIRECTORY)) {
                return ENOTDIR;
        }

        written =
            GetFullPathName(full_path, sizeof(real_path), real_path, NULL);

        if (written == 0)
                return ENOENT;

        if (written >= sizeof(real_path)) {
                return ENAMETOOLONG;
        }
#else
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
#endif

        strncpy(ctx->location, real_path, PATH_MAX);
        ctx->location[sizeof(ctx->location) - 1] = '\0';

        return 0;
}