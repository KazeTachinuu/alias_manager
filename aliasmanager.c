#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#include "alias_manager.h"

// ANSI color codes
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_RESET   "\033[0m"

static void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    va_end(args);
}

static void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%sError:%s ", COLOR_RED, COLOR_RESET);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static void log_success(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("%s✓%s ", COLOR_GREEN, COLOR_RESET);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

static FILE *open_alias_file(const char *mode)
{
    char *path = get_alias_file_path();
    if (!path)
    {
        return NULL;
    }
    
    FILE *fp = fopen(path, mode);
    if (!fp && errno != ENOENT)
    {
        log_error("Cannot access %s: %s", path, strerror(errno));
    }
    free(path);
    return fp;
}

static ErrorCode write_alias(FILE *fp, const char *name, const char *command)
{
    char clean_cmd[MAX_LINE_LENGTH] = {0};
    size_t j = 0;
    
    // Replace quotes with spaces
    for (const char *src = command; *src && j < MAX_LINE_LENGTH - 1; src++) {
        clean_cmd[j++] = (*src == '\'') ? ' ' : *src;
    }
    
    return fprintf(fp, "alias %s='%s'\n", name, clean_cmd) > 0 ? 
        SUCCESS : ERR_FILE_ACCESS;
}

ErrorCode am_add(const char *name, const char *command)
{
    if (!is_valid_name(name))
    {
        log_error("Invalid alias name");
        return ERR_INVALID_ARG;
    }
    if (!command || strlen(command) == 0)
    {
        log_error("Command cannot be empty");
        return ERR_INVALID_ARG;
    }

    char *path = get_alias_file_path();
    if (!path)
    {
        return ERR_HOME_DIR;
    }

    // Remove existing alias first
    ErrorCode err = am_remove(name, true);
    if (err != SUCCESS && err != ERR_FILE_ACCESS)
    {
        free(path);
        return err;
    }

    FILE *fp = fopen(path, "a");
    if (!fp)
    {
        log_error("Cannot write to alias file");
        free(path);
        return ERR_FILE_ACCESS;
    }

    err = write_alias(fp, name, command);
    fclose(fp);
    free(path);
    
    if (err == SUCCESS)
    {
        log_success("Added alias '%s%s%s' → '%s'", COLOR_YELLOW, name, COLOR_RESET, command);
    }
    return err;
}

ErrorCode am_remove(const char *name, bool force)
{
    if (!is_valid_name(name))
    {
        if (!force) log_error("Invalid alias name");
        return ERR_INVALID_ARG;
    }

    char *path = get_alias_file_path();
    if (!path)
    {
        return ERR_HOME_DIR;
    }

    if (access(path, F_OK) != 0)
    {
        free(path);
        return SUCCESS;
    }

    char temp_path[MAX_LINE_LENGTH + 5];
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", path);

    FILE *src = fopen(path, "r");
    if (!src)
    {
        free(path);
        return ERR_FILE_ACCESS;
    }

    FILE *dst = fopen(temp_path, "w");
    if (!dst)
    {
        fclose(src);
        free(path);
        return ERR_FILE_ACCESS;
    }

    char line[MAX_LINE_LENGTH];
    bool found = false;

    while (fgets(line, sizeof(line), src))
    {
        if (strstr(line, "alias ") == line && 
            strncmp(line + 6, name, strlen(name)) == 0 &&
            line[6 + strlen(name)] == '=')
        {
            found = true;
            if (!force)
            {
                printf("Remove alias: %s", line);
                printf("Are you sure? [Y/n] ");
                char response[10];
                if (fgets(response, sizeof(response), stdin))
                {
                    if (response[0] != 'y' && response[0] != 'Y' && response[0] != '\n')
                    {
                        log_info("Operation cancelled");
                        fclose(src);
                        fclose(dst);
                        remove(temp_path);
                        free(path);
                        return SUCCESS;
                    }
                }
            }
            continue;
        }
        fputs(line, dst);
    }

    fclose(src);
    fclose(dst);

    if (!found)
    {
        if (!force) printf("%sNote:%s Alias '%s' not found\n", COLOR_BLUE, COLOR_RESET, name);
        remove(temp_path);
        free(path);
        return SUCCESS;
    }

    struct stat st;
    if (stat(path, &st) == 0)
    {
        chmod(temp_path, st.st_mode);
    }

    if (rename(temp_path, path) != 0)
    {
        remove(temp_path);
        free(path);
        return ERR_SYSTEM;
    }

    if (!force)
    {
        log_success("Removed alias '%s%s%s'", COLOR_YELLOW, name, COLOR_RESET);
    }
    free(path);
    return SUCCESS;
}

ErrorCode am_list(const char *filter)
{
    FILE *fp = open_alias_file("r");
    if (!fp)
    {
        if (errno != ENOENT)
        {
            log_error("Cannot read alias file");
        }
        return ERR_FILE_ACCESS;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "alias ") != line) continue;
        char *alias_def = line + 6;
        if (!filter || strstr(alias_def, filter))
        {
            printf("%s", alias_def);
            count++;
        }
    }
    
    fclose(fp);
    
    if (count == 0 && filter)
    {
        printf("%sNote:%s No aliases found matching '%s'\n", COLOR_BLUE, COLOR_RESET, filter);
    }
    
    return SUCCESS;
}

// Utility implementations
const char *error_message(ErrorCode err)
{
    static const char *messages[] = { "Success",
                                      "Invalid arguments",
                                      "File access error",
                                      "Memory allocation failed",
                                      "Home directory not found",
                                      "System error" };
    return messages[err];
}

bool is_valid_name(const char *name)
{
    if (!name || !*name)
        return false;
    if (!isalpha(name[0]) && name[0] != '_')
        return false;

    for (; *name; name++)
    {
        if (!isalnum(*name) && *name != '_')
            return false;
    }
    return true;
}

char *get_alias_file_path(void)
{
    const char *home = getenv("HOME");
    if (!home)
        return NULL;

    char *path = malloc(strlen(home) + strlen(ALIAS_FILE) + 2);
    if (!path)
        return NULL;

    sprintf(path, "%s/%s", home, ALIAS_FILE);
    return path;
}
