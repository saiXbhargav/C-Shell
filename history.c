#include "history.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HISTORY_FILE "history.txt"
#define HISTORY_MAX 20
#define HISTORY_DISPLAY 10

// External globals from main.c
extern char history_cmds[HISTORY_MAX][256];
extern int history_count;
extern int history_start;
extern char shell_home[256];

void load_history() {
    char history_path[512];
#ifdef _WIN32
    snprintf(history_path, sizeof(history_path), "%s\\%s", shell_home, HISTORY_FILE);
#else
    snprintf(history_path, sizeof(history_path), "%s/%s", shell_home, HISTORY_FILE);
#endif
    
    FILE *f = fopen(history_path, "r");
    if (!f) return;
    
    history_count = 0; 
    history_start = 0;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0 && history_count < HISTORY_MAX) {
            strcpy(history_cmds[history_count++], line);
        }
    }
    fclose(f);
}

void save_history() {
    char history_path[512];
#ifdef _WIN32
    snprintf(history_path, sizeof(history_path), "%s\\%s", shell_home, HISTORY_FILE);
#else
    snprintf(history_path, sizeof(history_path), "%s/%s", shell_home, HISTORY_FILE);
#endif
    
    FILE *f = fopen(history_path, "w");
    if (!f) return;
    
    for (int i = 0; i < history_count; ++i) {
        int idx = (history_start + i) % HISTORY_MAX;
        fprintf(f, "%s\n", history_cmds[idx]);
    }
    fclose(f);
}

void add_to_history(const char *cmd) {
    if (strncmp(cmd, "history", 7) == 0) return;
    
    if (history_count > 0) {
        int last_idx = (history_start + history_count - 1) % HISTORY_MAX;
        if (strcmp(cmd, history_cmds[last_idx]) == 0) return;
    }
    
    if (history_count < HISTORY_MAX) {
        strcpy(history_cmds[history_count++], cmd);
    } else {
        strcpy(history_cmds[history_start], cmd);
        history_start = (history_start + 1) % HISTORY_MAX;
    }
    save_history();
}

void show_history() {
    int display_count = (history_count < HISTORY_DISPLAY) ? history_count : HISTORY_DISPLAY;
    int start_idx = (history_count <= HISTORY_DISPLAY) ? 0 : (history_count - HISTORY_DISPLAY);
    
    for (int i = 0; i < display_count; ++i) {
        int idx = (history_start + start_idx + i) % HISTORY_MAX;
        printf("%s\n", history_cmds[idx]);
    }
}
