#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alias_manager.h"

#ifndef PATH_MAX
#    define PATH_MAX 4096
#endif

// Command structure
typedef struct
{
    const char *name;
    ErrorCode (*handler)(int argc, char *argv[]);
    int min_args;
} Command;

static void print_usage(void)
{
    printf("%sAlias Manager%s v%s\n", am_color_blue(), am_color_reset(),
           VERSION);
    printf("\nUsage: am <command> [options]\n\n");
    printf("Commands:\n");
    printf("  %sinit%s              Interactive setup\n", am_color_blue(),
           am_color_reset());
    printf("  %sadd%s <name> <cmd>  Add/update an alias\n", am_color_green(),
           am_color_reset());
    printf("  %srm%s <name> [-f]    Remove an alias\n", am_color_red(),
           am_color_reset());
    printf("  %sls%s [filter]       List aliases\n", am_color_yellow(),
           am_color_reset());
    printf("  %sreload%s            Reload aliases in current shell\n",
           am_color_yellow(), am_color_reset());
    printf("  help              Show this help\n");
    printf("  version           Show version\n");
}

static ErrorCode handle_add(int argc, char *argv[])
{
    char command[MAX_LINE_LENGTH] = { 0 };
    size_t offset = 0;

    for (int i = 3; i < argc; i++)
    {
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
    (void)argc;
    (void)argv;
    print_usage();
    return SUCCESS;
}

static ErrorCode handle_version(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    printf("%sAlias Manager%s v%s\n", am_color_blue(), am_color_reset(),
           VERSION);
    return SUCCESS;
}

static ErrorCode handle_init(int argc, char *argv[])
{
    // Check for --hook flag (outputs shell code only, for use in RC files)
    bool hook_mode = false;
    const char *hook_shell = NULL;

    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "--hook") == 0)
        {
            hook_mode = true;
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                hook_shell = argv[i + 1];
            }
            break;
        }
    }

    if (hook_mode)
    {
        // Output shell hook code (for eval)
        const char *shell = hook_shell ? hook_shell : getenv("SHELL");
        if (!shell)
        {
            fprintf(stderr, "Could not detect shell\n");
            return ERR_INVALID_ARG;
        }

        const char *shell_name = strrchr(shell, '/');
        shell_name = shell_name ? shell_name + 1 : shell;

        if (strcmp(shell_name, "bash") == 0 || strcmp(shell_name, "zsh") == 0)
        {
            printf("[[ -f \"${XDG_CONFIG_HOME:-$HOME/.config}/am/aliases.txt\" "
                   "]] && "
                   "source "
                   "\"${XDG_CONFIG_HOME:-$HOME/.config}/am/aliases.txt\"\n");
        }
        else if (strcmp(shell_name, "fish") == 0)
        {
            printf("set -q XDG_CONFIG_HOME; and set -l am_config_dir "
                   "$XDG_CONFIG_HOME; or set -l am_config_dir ~/.config\n");
            printf("test -f $am_config_dir/am/aliases.txt; and source "
                   "$am_config_dir/am/aliases.txt\n");
        }
        else
        {
            fprintf(stderr, "Unsupported shell: %s\n", shell_name);
            return ERR_INVALID_ARG;
        }
        return SUCCESS;
    }

    // Interactive init mode
    const char *home = getenv("HOME");
    if (!home)
    {
        fprintf(stderr, "Error: HOME environment variable not set\n");
        return ERR_HOME_DIR;
    }

    const char *shell =
        argc >= 3 && argv[2][0] != '-' ? argv[2] : getenv("SHELL");
    if (!shell)
    {
        fprintf(stderr,
                "Could not detect shell. Usage: am init [bash|zsh|fish]\n");
        return ERR_INVALID_ARG;
    }

    const char *shell_name = strrchr(shell, '/');
    shell_name = shell_name ? shell_name + 1 : shell;

    const char *rc_file = NULL;
    const char *init_line = NULL;
    char rc_path[PATH_MAX];

    if (strcmp(shell_name, "bash") == 0)
    {
        rc_file = ".bashrc";
        init_line = "eval \"$(am init --hook)\"";
    }
    else if (strcmp(shell_name, "zsh") == 0)
    {
        rc_file = ".zshrc";
        init_line = "eval \"$(am init --hook)\"";
    }
    else if (strcmp(shell_name, "fish") == 0)
    {
        rc_file = ".config/fish/config.fish";
        init_line = "am init --hook fish | source";
    }
    else
    {
        fprintf(stderr, "Unsupported shell: %s\n", shell_name);
        fprintf(stderr, "Supported shells: bash, zsh, fish\n");
        return ERR_INVALID_ARG;
    }

    int written = snprintf(rc_path, sizeof(rc_path), "%s/%s", home, rc_file);
    if (written < 0 || written >= (int)sizeof(rc_path))
    {
        fprintf(stderr, "%sError:%s RC file path too long\n", am_color_red(),
                am_color_reset());
        return ERR_INVALID_ARG;
    }

    // Check if already initialized (aliases.txt exists = already set up)
    char alias_path[PATH_MAX];
    const char *xdg = getenv("XDG_CONFIG_HOME");
    written = xdg
        ? snprintf(alias_path, sizeof(alias_path), "%s/am/aliases.txt", xdg)
        : snprintf(alias_path, sizeof(alias_path), "%s/.config/am/aliases.txt",
                   home);

    if (written > 0 && written < (int)sizeof(alias_path)
        && access(alias_path, F_OK) == 0)
    {
        printf("%s✓%s Already initialized (found %s)\n", am_color_green(),
               am_color_reset(), alias_path);
        return SUCCESS;
    }

    // Interactive confirmation
    printf("%sSetup%s\n", am_color_blue(), am_color_reset());
    printf("Shell:  %s\n", shell_name);
    printf("Config: %s\n", rc_path);
    printf("Line:   %s\n\n", init_line);
    printf("Continue? [Y/n] ");
    fflush(stdout);

    char response[10];
    if (fgets(response, sizeof(response), stdin))
    {
        if (response[0] != '\n' && response[0] != 'y' && response[0] != 'Y')
        {
            printf("Setup cancelled\n");
            return SUCCESS;
        }
    }

    // Append to RC file
    FILE *fp = fopen(rc_path, "a");
    if (!fp)
    {
        fprintf(stderr, "%sError:%s Cannot write to %s\n", am_color_red(),
                am_color_reset(), rc_path);
        return ERR_FILE_ACCESS;
    }

    fprintf(fp, "\n# Alias Manager\n%s\n", init_line);
    fclose(fp);

    printf("\n%s✓%s Done. Run: source %s\n", am_color_green(), am_color_reset(),
           rc_path);

    return SUCCESS;
}

static ErrorCode handle_reload(int argc, char *argv[])
{
    const char *shell = argc >= 3 ? argv[2] : getenv("SHELL");

    if (!shell)
    {
        fprintf(stderr,
                "Could not detect shell. Usage: am reload [bash|zsh|fish]\n");
        return ERR_INVALID_ARG;
    }

    const char *shell_name = strrchr(shell, '/');
    shell_name = shell_name ? shell_name + 1 : shell;

    if (strcmp(shell_name, "bash") == 0 || strcmp(shell_name, "zsh") == 0)
    {
        printf("source \"${XDG_CONFIG_HOME:-$HOME/.config}/am/aliases.txt\"\n");
    }
    else if (strcmp(shell_name, "fish") == 0)
    {
        printf("set -q XDG_CONFIG_HOME; and set -l am_config_dir "
               "$XDG_CONFIG_HOME; "
               "or set -l am_config_dir ~/.config\n");
        printf("source $am_config_dir/am/aliases.txt\n");
    }
    else
    {
        fprintf(stderr, "Unsupported shell: %s\n", shell_name);
        fprintf(stderr, "Supported shells: bash, zsh, fish\n");
        return ERR_INVALID_ARG;
    }

    return SUCCESS;
}

// Command table
static const Command COMMANDS[] = {
    { "add", handle_add, 4 },         { "rm", handle_remove, 3 },
    { "ls", handle_list, 2 },         { "init", handle_init, 2 },
    { "reload", handle_reload, 2 },   { "help", handle_help, 1 },
    { "version", handle_version, 1 }, { NULL, NULL, 0 } // Sentinel
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
    if (cmd_name[0] == '-' && cmd_name[1] == '-')
    {
        if (strcmp(cmd_name + 2, "help") == 0)
            cmd_name = "help";
        else if (strcmp(cmd_name + 2, "version") == 0)
            cmd_name = "version";
        else if (strcmp(cmd_name + 2, "init") == 0)
            cmd_name = "init";
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
                fprintf(stderr, "%sError:%s %s\n", am_color_red(),
                        am_color_reset(), error_message(err));
            }
            return err;
        }
        cmd++;
    }

    fprintf(stderr, "%sError:%s Unknown command '%s'\n", am_color_red(),
            am_color_reset(), cmd_name);
    fprintf(stderr, "Try '%sam help%s' for usage\n", am_color_blue(),
            am_color_reset());
    return ERR_INVALID_ARG;
}
