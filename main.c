

#include "prompt.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#include <sys/wait.h>
#endif


// Helper to trim leading/trailing spaces/tabs
void trim(char *str) {
    int len = strlen(str);
    int start = 0, end = len - 1;
    while (isspace((unsigned char)str[start])) start++;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    if (start > 0 || end < len - 1) memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';
}

void run_command(char *cmd, int background) {
    trim(cmd);
    if (strlen(cmd) == 0) return;
    static char prev_dir[256] = "";
    extern char shell_home[256]; // from prompt.c
    if (strcmp(cmd, "exit") == 0) {
        exit(0);
    } else if (strncmp(cmd, "cd ", 3) == 0) {
        if (chdir(cmd + 3) != 0) {
            perror("cd failed");
        }
    } else if (strncmp(cmd, "hop", 3) == 0 && (cmd[3] == ' ' || cmd[3] == '\0')) {
        // hop command
        char *args[32];
        int argc = 0;
        char *rest = cmd + 3;
        while (*rest == ' ' || *rest == '\t') rest++;
        if (*rest == '\0') {
            // No argument: hop to home
            strcpy(prev_dir, shell_home);
            chdir(shell_home);
            char cwd[256];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
            return;
        }
        char *token = strtok(rest, " ");
        while (token && argc < 32) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        for (int i = 0; i < argc; ++i) {
            char target[256];
            if (strcmp(args[i], "~") == 0) {
                strcpy(target, shell_home);
            } else if (strcmp(args[i], "-") == 0) {
                if (prev_dir[0] == '\0') {
                    printf("No previous directory\n");
                    continue;
                }
                strcpy(target, prev_dir);
            } else if (args[i][0] == '~') {
                snprintf(target, sizeof(target), "%s%s", shell_home, args[i]+1);
            } else {
                strcpy(target, args[i]);
            }
            char cwd_before[256];
            getcwd(cwd_before, sizeof(cwd_before));
            if (chdir(target) != 0) {
                perror("hop failed");
            } else {
                char cwd_after[256];
                getcwd(cwd_after, sizeof(cwd_after));
                printf("%s\n", cwd_after);
                strcpy(prev_dir, cwd_before);
            }
        }
    } else {
#ifdef _WIN32
        // Windows: use _spawnl for background, system for foreground
        if (background) {
            intptr_t pid = _spawnl(_P_NOWAIT, "C:\\Windows\\System32\\cmd.exe", "cmd.exe", "/C", cmd, NULL);
            if (pid == -1) {
                printf("ERROR : '%s' is not a valid command\n", cmd);
            } else {
                static int job = 1;
                printf("[%d] %d\n", job++, (int)pid);
            }
        } else {
            int ret = system(cmd);
            if (ret == -1) {
                printf("ERROR : '%s' is not a valid command\n", cmd);
            }
        }
#else
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
        } else if (pid == 0) {
            // Child process
            char *argv[4] = {"/bin/sh", "-c", cmd, NULL};
            execvp("/bin/sh", argv);
            printf("ERROR : '%s' is not a valid command\n", cmd);
            exit(1);
        } else {
            static int job = 1;
            if (background) {
                printf("[%d] %d\n", job++, pid);
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
        }
#endif
    }
}

int main() {
    initialize_prompt();

    while (1) {
        display_prompt();

        char buffer[256];
        if (!fgets(buffer, sizeof(buffer), stdin)) break;
        buffer[strcspn(buffer, "\n")] = '\0';

        // Tokenize by ;
        char *cmd_group = strtok(buffer, ";");
        while (cmd_group) {
            // Tokenize by &
            char *cmd = strtok(cmd_group, "&");
            int first = 1;
            while (cmd) {
                trim(cmd);
                if (strlen(cmd) > 0) {
                    run_command(cmd, !first);
                }
                cmd = strtok(NULL, "&");
                first = 0;
            }
            cmd_group = strtok(NULL, ";");
        }
    }

    return 0;
}
