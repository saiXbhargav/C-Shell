#include "execute.h"
#include "builtins.h"
#include "history.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

// External globals from main.c
#ifdef _WIN32
extern PROCESS_INFORMATION foreground_process;
extern volatile int has_foreground_process;
#else
extern pid_t foreground_pid;
#endif

void run_command(char *cmd) {
    trim(cmd);
    if (strlen(cmd) == 0) return;
    
    add_to_history(cmd);
    int is_background = 0;
    int len = strlen(cmd);
    while (len > 0 && (cmd[len-1] == ' ' || cmd[len-1] == '\t')) len--;
    if (len > 0 && cmd[len-1] == '&') {
        is_background = 1;
        cmd[len-1] = '\0';
        trim(cmd);
    }
    
    char cmd_copy[256];
    strcpy(cmd_copy, cmd);
    
    char *command = strtok(cmd_copy, " \t");
    char *args = cmd + strlen(command);
    while (*args == ' ' || *args == '\t') args++;
    
    if (strcmp(command, "exit") == 0) {
        exit(0);
    } else if (strcmp(command, "echo") == 0) {
        echo_command(args);
    } else if (strcmp(command, "pwd") == 0) {
        pwd_command();
    } else if (strcmp(command, "cd") == 0) {
        cd_command(args);
    } else if (strcmp(command, "history") == 0) {
        show_history();
    } else {
#ifdef _WIN32
        if (is_background) {
            intptr_t pid = _spawnlp(_P_NOWAIT, "cmd.exe", "cmd.exe", "/C", cmd, NULL);
            if (pid == -1) {
                printf("ERROR : '%s' is not a valid command\n", cmd);
            }
        } else {
            STARTUPINFOA si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&foreground_process, sizeof(foreground_process));
            
            char command_line[512];
            snprintf(command_line, sizeof(command_line), "cmd.exe /C %s", cmd);
            
            if (CreateProcessA(NULL, command_line, NULL, NULL, TRUE, 0, NULL, NULL, &si, &foreground_process)) {
                has_foreground_process = 1;
                WaitForSingleObject(foreground_process.hProcess, INFINITE);
                CloseHandle(foreground_process.hProcess);
                CloseHandle(foreground_process.hThread);
                has_foreground_process = 0;
            } else {
                printf("ERROR : '%s' is not a valid command\n", cmd);
            }
        }
#else
        char *input_file = NULL, *output_file = NULL;
        char command_buf[256];
        strcpy(command_buf, cmd);

        char *out_ptr = strchr(command_buf, '>');
        if (out_ptr) {
            *out_ptr = '\0';
            out_ptr++;
            while (*out_ptr == ' ' || *out_ptr == '\t') out_ptr++;
            output_file = strtok(out_ptr, " \t<");
        }

        char *in_ptr = strchr(command_buf, '<');
        if (in_ptr) {
            *in_ptr = '\0';
            in_ptr++;
            while (*in_ptr == ' ' || *in_ptr == '\t') in_ptr++;
            input_file = strtok(in_ptr, " \t>");
        }

        trim(command_buf);
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
        } else if (pid == 0) {
            if (input_file) {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in < 0) {
                    perror("Input file open failed");
                    exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (output_file) {
                int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    perror("Output file open failed");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            char *argv[4] = {"/bin/sh", "-c", command_buf, NULL};
            execvp("/bin/sh", argv);
            printf("ERROR : '%s' is not a valid command\n", command_buf);
            exit(1);
        } else {
            if (!is_background) {
                foreground_pid = pid;
                int status;
                waitpid(pid, &status, 0);
                foreground_pid = -1;
            }
        }
#endif
    }
}
