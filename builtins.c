#include "builtins.h"
#include "history.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _WIN32
#include <direct.h>
#define chdir _chdir
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

// External globals needed by built-ins
extern char shell_home[256];
extern char prev_dir[256];

// ECHO command implementation
void echo_command(char *args) {
    char *token = strtok(args, " \t");
    int first = 1;
    
    while (token != NULL) {
        if (!first) {
            printf(" ");
        }
        printf("%s", token);
        first = 0;
        token = strtok(NULL, " \t");
    }
    printf("\n");
}

// PWD command implementation
void pwd_command() {
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

// CD command implementation
void cd_command(char *args) {
    // The trim function is in utils.c, but it was originally called on args in run_command.
    // For simplicity, we assume args is already trimmed or handle it here if needed.
    
    if (strlen(args) == 0) {
        // cd with no arguments - go to home
        char current_dir[512];
        getcwd(current_dir, sizeof(current_dir));
        strcpy(prev_dir, current_dir);
        
        if (chdir(shell_home) != 0) {
            perror("cd");
        }
        return;
    }
    
    // Check for multiple arguments
    char args_copy[256];
    strcpy(args_copy, args);
    char *first_arg = strtok(args_copy, " \t");
    char *second_arg = strtok(NULL, " \t");
    
    if (second_arg != NULL) {
        printf("cd: too many arguments\n");
        return;
    }
    
    char current_dir[512];
    getcwd(current_dir, sizeof(current_dir));
    
    char target_dir[512];
    
    if (strcmp(first_arg, "~") == 0) {
        // cd ~ - go to home
        strcpy(target_dir, shell_home);
    } else if (strcmp(first_arg, "..") == 0) {
        // cd .. - go up one level
        strcpy(target_dir, "..");
    } else if (strcmp(first_arg, "-") == 0) {
        // cd - - go to previous directory
        if (strlen(prev_dir) == 0) {
            printf("cd: No previous directory\n");
            return;
        }
        strcpy(target_dir, prev_dir);
        printf("%s\n", target_dir);
    } else {
        // cd [directory_path]
        strcpy(target_dir, first_arg);
    }
    
    strcpy(prev_dir, current_dir);
    
    if (chdir(target_dir) != 0) {
        perror("cd");
        // Restore prev_dir on failure
        strcpy(prev_dir, "");
    }
}
