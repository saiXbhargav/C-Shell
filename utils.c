#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
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

// Helper to trim leading/trailing spaces/tabs
void trim(char *str) {
    int len = strlen(str);
    int start = 0, end = len - 1;
    while (start < len && isspace((unsigned char)str[start])) start++;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    if (start > 0 || end < len - 1) {
        memmove(str, str + start, end - start + 1);
    }
    str[end - start + 1] = '\0';
}
