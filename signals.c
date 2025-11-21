#include "signals.h"
#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#include <signal.h>
// External globals from main.c
extern PROCESS_INFORMATION foreground_process;
extern volatile int has_foreground_process;
#else
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
// External globals from main.c
extern pid_t foreground_pid;
#endif

#ifdef _WIN32
BOOL WINAPI ctrl_handler(DWORD dwCtrlType) {
    switch(dwCtrlType) {
        case CTRL_C_EVENT:
            if (has_foreground_process) {
                GenerateConsoleCtrlEvent(CTRL_C_EVENT, foreground_process.dwProcessId);
                has_foreground_process = 0;
            }
            return TRUE;
        case CTRL_BREAK_EVENT:
            return TRUE;
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            cleanup_background_processes();
            return FALSE;
        default:
            return FALSE;
    }
}

void setup_signal_handlers() {
    SetConsoleCtrlHandler(ctrl_handler, TRUE);
}

void cleanup_background_processes() {
    // Windows handles process cleanup automatically
}

#else
void sigint_handler(int signo) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
    }
}

void sigchld_handler(int signo) {
    int saved_errno = errno;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        // Process cleaned up
    }
    errno = saved_errno;
}

void setup_signal_handlers() {
    struct sigaction sa_int, sa_chld;
    
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa_chld, NULL);
}

void cleanup_background_processes() {
    // Handled by SIGCHLD handler
}
#endif
