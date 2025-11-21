#include "prompt.h"
#include "history.h"
#include "signals.h"
#include "execute.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define HISTORY_MAX 20

// Global variables
char history_cmds[HISTORY_MAX][256];
int history_count = 0;
int history_start = 0;
char prev_dir[256] = "";
char shell_home[256]; // Definition of the global shell_home

// Signal handling globals
#ifdef _WIN32
PROCESS_INFORMATION foreground_process = {0};
volatile int has_foreground_process = 0;
#else
pid_t foreground_pid = -1;
#endif

int main() {
    setup_signal_handlers();
    
    initialize_prompt();
    load_history();

    while (1) {
        display_prompt();

        char buffer[256];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            if (feof(stdin)) {
                printf("\nExiting shell...\n");
                cleanup_background_processes();
                break;
            } else if (errno == EINTR) {
                printf("\n");
                errno = 0;
                continue;
            } else {
                perror("fgets");
                break;
            }
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) {
            continue;
        }

        run_command(buffer);
    }

    return 0;
}