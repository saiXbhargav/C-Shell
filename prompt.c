
#include "prompt.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#define HOST_NAME_MAX 256
#else
#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#endif

extern char shell_home[256];  // Global for hop

void initialize_prompt() {
    getcwd(shell_home, sizeof(shell_home));//get current working directory
}

void display_prompt() {
    char cwd[256], display_path[256], hostname[HOST_NAME_MAX];//current working directory
#ifdef _WIN32
    const char *user = getenv("USERNAME");//get username
    if (!user) user = "user";
    DWORD size = HOST_NAME_MAX;
    if (!GetComputerNameA(hostname, &size)) {
        strcpy(hostname, "host");
    }
#else
    struct passwd *pw = getpwuid(getuid());
    gethostname(hostname, sizeof(hostname));
#endif
    getcwd(cwd, sizeof(cwd));
    get_relative_path(cwd, shell_home, display_path);
#ifdef _WIN32
    printf("<%s@%s:%s> ", user, hostname, display_path);
#else
    printf("<%s@%s:%s> ", pw->pw_name, hostname, display_path);
#endif
    fflush(stdout);
}
