#ifndef ALIAS_MANAGER_H
#define ALIAS_MANAGER_H

#include <stdbool.h>

#define VERSION "2.0.0"
#define MAX_LINE_LENGTH 4096
#define ALIAS_FILE ".my_aliases.txt"

typedef enum
{
    SUCCESS = 0,
    ERR_INVALID_ARG,
    ERR_FILE_ACCESS,
    ERR_MEMORY,
    ERR_HOME_DIR,
    ERR_SYSTEM
} ErrorCode;

// Command handler function type
typedef ErrorCode (*CommandHandler)(int argc, char *argv[]);

// Command structure
typedef struct {
    const char *name;
    const char *description;
    CommandHandler handler;
    int min_args;
} Command;

// Core functionality
ErrorCode am_add(const char *name, const char *command);
ErrorCode am_remove(const char *name, bool force);
ErrorCode am_list(const char *filter);
ErrorCode am_help(void);
ErrorCode am_version(void);

// Command handlers
ErrorCode handle_add(int argc, char *argv[]);
ErrorCode handle_remove(int argc, char *argv[]);
ErrorCode handle_list(int argc, char *argv[]);
ErrorCode handle_help(int argc, char *argv[]);
ErrorCode handle_version(int argc, char *argv[]);

// Utility functions
const char *error_message(ErrorCode err);
bool is_valid_name(const char *name);
char *get_alias_file_path(void);
void print_usage(void);

#endif
