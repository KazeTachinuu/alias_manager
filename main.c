#include <stdio.h>
#include <string.h>

#include "alias_manager.h"

// ANSI color codes
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_RESET   "\033[0m"

void print_usage(void)
{
    printf("%sAlias Manager%s v%s\n", COLOR_BLUE, COLOR_RESET, VERSION);
    printf("\nUsage: am <command> [options]\n\n");
    printf("Commands:\n");
    printf("  %sadd%s <name> <command>  Add/update an alias\n", COLOR_GREEN, COLOR_RESET);
    printf("  %srm%s <name> [-f]        Remove an alias (force to suppress errors)\n", COLOR_RED, COLOR_RESET);
    printf("  %sls%s [filter]           List aliases (optionally filtered)\n", COLOR_YELLOW, COLOR_RESET);
    printf("  help                  Show this help\n");
    printf("  version               Show version\n");
}

// Command handlers
ErrorCode handle_add(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "%sError:%s Missing arguments for 'add'\n", COLOR_RED, COLOR_RESET);
        return ERR_INVALID_ARG;
    }

    // Join all remaining arguments with spaces as the command
    char command[MAX_LINE_LENGTH] = {0};
    for (int i = 3; i < argc; i++)
    {
        if (i > 3) strcat(command, " ");
        strcat(command, argv[i]);
    }

    return am_add(argv[2], command);
}

ErrorCode handle_remove(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "%sError:%s Missing alias name\n", COLOR_RED, COLOR_RESET);
        return ERR_INVALID_ARG;
    }
    bool force = (argc >= 4 && strcmp(argv[3], "-f") == 0);
    return am_remove(argv[2], force);
}

ErrorCode handle_list(int argc, char *argv[])
{
    return am_list(argc >= 3 ? argv[2] : NULL);
}

ErrorCode handle_help(int argc, char *argv[])
{
    (void)argc; // Unused
    (void)argv; // Unused
    print_usage();
    return SUCCESS;
}

ErrorCode handle_version(int argc, char *argv[])
{
    (void)argc; // Unused
    (void)argv; // Unused
    printf("%sAlias Manager%s v%s\n", COLOR_BLUE, COLOR_RESET, VERSION);
    return SUCCESS;
}

// Command table
static const Command COMMANDS[] = {
    {"add",     "Add/update an alias",                handle_add,     4},
    {"rm",      "Remove an alias",                    handle_remove,  3},
    {"ls",      "List aliases (optionally filtered)", handle_list,    2},
    {"help",    "Show this help",                     handle_help,    1},
    {"version", "Show version information",           handle_version, 1},
    {NULL, NULL, NULL, 0} // Sentinel
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage();
        return SUCCESS;
    }

    const char *cmd_name = argv[1];
    const Command *cmd = COMMANDS;

    // Find the command
    while (cmd->name)
    {
        if (strcmp(cmd_name, cmd->name) == 0)
        {
            if (argc < cmd->min_args)
            {
                fprintf(stderr, "%sError:%s Not enough arguments for '%s'\n", COLOR_RED, COLOR_RESET, cmd_name);
                fprintf(stderr, "Try '%sam help%s' for usage\n", COLOR_BLUE, COLOR_RESET);
                return ERR_INVALID_ARG;
            }
            ErrorCode err = cmd->handler(argc, argv);
            if (err != SUCCESS)
            {
                fprintf(stderr, "%sError:%s %s\n", COLOR_RED, COLOR_RESET, error_message(err));
            }
            return err;
        }
        cmd++;
    }

    fprintf(stderr, "%sError:%s Unknown command '%s'\n", COLOR_RED, COLOR_RESET, cmd_name);
    fprintf(stderr, "Try '%sam help%s' for usage\n", COLOR_BLUE, COLOR_RESET);
    return ERR_INVALID_ARG;
}
