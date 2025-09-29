#ifndef PROMPT_H
#define PROMPT_H

void initialize_prompt();    // Call once to set the shell's home dir
void display_prompt();       // Prints <user@host:dir> with ~ substitution

#endif
