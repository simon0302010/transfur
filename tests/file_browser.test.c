#include <stdio.h>
#include <stdlib.h>

#include "../src/interfaces/file/browser.h"

int main(void) {
        struct browser_ctx ctx;
        char cwd[PATH_MAX];
        struct browser_entry *entries = NULL;
        size_t entry_count = 0;
        size_t i;

        browser_init_default(&ctx);

        browser_getcwd(&ctx, cwd, sizeof(cwd));
        puts(cwd);

        browser_dir(&ctx, BROWSER_DIRECTORIES, &entries, &entry_count);
        if (entry_count == 0) {
                puts("no directories in current folder");
                return 1;
        }

        printf("changing directory to %s\n", entries[0].name);

        printf("browser_cd: %i\n", browser_cd(&ctx, entries[0].name, true));

        browser_getcwd(&ctx, cwd, sizeof(cwd));
        puts(cwd);

        free((void *)entries);

        return 0;
}