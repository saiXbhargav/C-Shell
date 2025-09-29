#include "utils.h"
#include <string.h>
#include <stdio.h>

void get_relative_path(const char *cwd, const char *home, char *out) {
    size_t home_len = strlen(home);

    if (strncmp(cwd, home, home_len) == 0) {
        if (strlen(cwd) == home_len) {
            sprintf(out, "~");
        } else {
            sprintf(out, "~%s", cwd + home_len);
        }
    } else {
        sprintf(out, "%s", cwd);
    }
}
