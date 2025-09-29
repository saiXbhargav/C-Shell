# C-shell

A simple custom shell written in C for Windows and Unix-like systems.

## Features
- Displays a prompt in the format `<username@hostname:dir>`
- Uses `~` for the shell's home directory
- Supports `cd <directory>` to change directories
- Supports `exit` to quit the shell

## Windows Compatibility
- Uses Windows API and environment variables for username and hostname
- Uses `_getcwd` and `_chdir` for directory operations
- Replaces Unix-specific headers/functions with Windows alternatives


## File Descriptions and Functions

### main.c
**Purpose:** Entry point and main loop of the shell.

**Functions:**
- `main()`: Initializes the prompt, reads user input, handles `cd` and `exit` commands, and calls prompt display functions.
	- Uses `fgets` to read input, `strcspn` to strip newline, `strcmp` and `strncmp` for command checks, and `chdir` to change directories.

### prompt.c / prompt.h
**Purpose:** Handles prompt initialization and display.

**Functions:**
- `initialize_prompt()`: Sets the shell's home directory at startup using `getcwd`.
- `display_prompt()`: Prints the prompt in the format `<user@host:dir>`, substituting `~` for the home directory. Uses platform-specific code to get username and hostname.

### utils.c / utils.h
**Purpose:** Utility functions for path formatting.

**Functions:**
- `get_relative_path(const char *cwd, const char *home, char *out)`: If the current directory is inside the shell's home, replaces the home part with `~`. Otherwise, prints the full path.

### Makefile
**Purpose:** Build instructions for compiling the shell.

**Targets:**
- `all` (default): Builds the shell executable (`prompt_test` or `prompt_test.exe`).
- `clean`: Removes object files and the executable.


## Build Instructions
### On Windows (using MinGW)
```sh
mingw32-make
```
This creates `prompt_test.exe`.

### On Unix-like systems
```sh
make
```
This creates `prompt_test`.

## Run Instructions
### On Windows (PowerShell/Command Prompt)
```sh
./prompt_test.exe
```
### On Git Bash
```sh
winpty ./prompt_test.exe
```

## Usage
- The shell displays a prompt.
- Type `cd <directory>` to change directories.
- Type `exit` to quit.
- Any other input redisplays the prompt.

## Recent Updates
- Added Windows compatibility for prompt and directory functions
- Fixed missing includes (`<string.h>`, `<direct.h>`, etc.)
- Replaced `<pwd.h>` and related Unix code with Windows alternatives
- Updated Makefile for MinGW
