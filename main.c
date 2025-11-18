#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alias_manager.h"

// Command structure
typedef struct {
    const char *name;
    const char *description;
    ErrorCode (*handler)(int argc, char *argv[]);
    int min_args;
} Command;

static void print_usage(void)
{
    printf("%sAlias Manager%s v%s\n", am_color_blue(), am_color_reset(), VERSION);
    printf("\nUsage: am <command> [options]\n\n");
    printf("Commands:\n");
    printf("  %sadd%s <name> <command>  Add/update an alias\n", am_color_green(), am_color_reset());
    printf("  %srm%s <name> [-f]        Remove an alias (force to suppress errors)\n", am_color_red(), am_color_reset());
    printf("  %sls%s [filter]           List aliases (optionally filtered)\n", am_color_yellow(), am_color_reset());
    printf("  %sinit%s [shell]          Output shell configuration\n", am_color_blue(), am_color_reset());
    printf("  help                  Show this help\n");
    printf("  version               Show version\n");
    printf("\nSetup:\n");
    printf("  eval \"$(am init)\"       # Auto-detect shell and setup\n");
    printf("  eval \"$(am init bash)\"  # Setup for specific shell\n");
}

static ErrorCode handle_add(int argc, char *argv[])
{
    char command[MAX_LINE_LENGTH] = {0};
    size_t offset = 0;

    for (int i = 3; i < argc; i++) {
        int written = snprintf(command + offset, MAX_LINE_LENGTH - offset,
                              "%s%s", i > 3 ? " " : "", argv[i]);
        if (written < 0 || offset + (size_t)written >= MAX_LINE_LENGTH)
            return ERR_INVALID_ARG;
        offset += (size_t)written;
    }

    return am_add(argv[2], command);
}

static ErrorCode handle_remove(int argc, char *argv[])
{
    bool force = argc >= 4 && strcmp(argv[3], "-f") == 0;
    return am_remove(argv[2], force);
}

static ErrorCode handle_list(int argc, char *argv[])
{
    return am_list(argc >= 3 ? argv[2] : NULL);
}

static ErrorCode handle_help(int argc, char *argv[])
{
    (void)argc; (void)argv;
    print_usage();
    return SUCCESS;
}

static ErrorCode handle_version(int argc, char *argv[])
{
    (void)argc; (void)argv;
    printf("%sAlias Manager%s v%s\n", am_color_blue(), am_color_reset(), VERSION);
    return SUCCESS;
}

static ErrorCode handle_init(int argc, char *argv[])
{
    const char *shell = argc >= 3 ? argv[2] : getenv("SHELL");

    if (!shell) {
        fprintf(stderr, "Could not detect shell. Usage: am init [bash|zsh|fish]\n");
        return ERR_INVALID_ARG;
    }

    const char *shell_name = strrchr(shell, '/');
    shell_name = shell_name ? shell_name + 1 : shell;

    if (strcmp(shell_name, "bash") == 0 || strcmp(shell_name, "zsh") == 0) {
        printf("# Add this to your ~/.%src or run: eval \"$(am init)\"\n", shell_name);
        printf("[[ -f \"${XDG_CONFIG_HOME:-$HOME/.config}/am/aliases.txt\" ]] && source \"${XDG_CONFIG_HOME:-$HOME/.config}/am/aliases.txt\"\n");
        printf("[[ -f ~/.my_aliases.txt ]] && source ~/.my_aliases.txt\n");
    } else if (strcmp(shell_name, "fish") == 0) {
        printf("# Add this to your ~/.config/fish/config.fish\n");
        printf("set -q XDG_CONFIG_HOME; and set -l am_config_dir $XDG_CONFIG_HOME; or set -l am_config_dir ~/.config\n");
        printf("test -f $am_config_dir/am/aliases.txt; and source $am_config_dir/am/aliases.txt\n");
        printf("test -f ~/.my_aliases.txt && source ~/.my_aliases.txt\n");
    } else {
        fprintf(stderr, "Unsupported shell: %s\n", shell_name);
        fprintf(stderr, "Supported shells: bash, zsh, fish\n");
        return ERR_INVALID_ARG;
    }

    return SUCCESS;
}

// Command table
static const Command COMMANDS[] = {
    {"add",     "Add/update an alias",                handle_add,     4},
    {"rm",      "Remove an alias",                    handle_remove,  3},
    {"ls",      "List aliases (optionally filtered)", handle_list,    2},
    {"init",    "Output shell configuration",         handle_init,    2},
    {"help",    "Show this help",                     handle_help,    1},
    {"version", "Show version information",           handle_version, 1},
    {NULL, NULL, NULL, 0} // Sentinel
};

int main(int argc, char *argv[])
{
    am_init_colors();

    if (argc < 2)
    {
        print_usage();
        return SUCCESS;
    }

    const char *cmd_name = argv[1];

    // Map long options to short commands
    if (cmd_name[0] == '-' && cmd_name[1] == '-') {
        if (strcmp(cmd_name + 2, "help") == 0) cmd_name = "help";
        else if (strcmp(cmd_name + 2, "version") == 0) cmd_name = "version";
        else if (strcmp(cmd_name + 2, "init") == 0) cmd_name = "init";
    }

    const Command *cmd = COMMANDS;

    // Find the command
    while (cmd->name)
    {
        if (strcmp(cmd_name, cmd->name) == 0)
        {
            if (argc < cmd->min_args)
            {
                fprintf(stderr, "%sError:%s Not enough arguments for '%s'\n",
                        am_color_red(), am_color_reset(), cmd_name);
                fprintf(stderr, "Try '%sam help%s' for usage\n",
                        am_color_blue(), am_color_reset());
                return ERR_INVALID_ARG;
            }
            ErrorCode err = cmd->handler(argc, argv);
            if (err != SUCCESS)
            {
                fprintf(stderr, "%sError:%s %s\n",
                        am_color_red(), am_color_reset(), error_message(err));
            }
            return err;
        }
        cmd++;
    }

    fprintf(stderr, "%sError:%s Unknown command '%s'\n",
            am_color_red(), am_color_reset(), cmd_name);
    fprintf(stderr, "Try '%sam help%s' for usage\n",
            am_color_blue(), am_color_reset());
    return ERR_INVALID_ARG;
}
