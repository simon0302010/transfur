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

        browser_dir(&ctx, 0, &entries, &entry_count);
        for (i = 0; i < entry_count; i++) {
                printf("name=%s type=%s\n", entries[i].name,
                       (entries[i].entry_type == browser_entry_file)
                           ? "file"
                           : "directory");
        }
        free((void *)entries);

        return 0;
}