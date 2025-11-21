#ifndef HISTORY_H
#define HISTORY_H

void load_history();
void save_history();
void add_to_history(const char *cmd);
void show_history(); // Declaration is also in builtins.h, which is fine

#endif
