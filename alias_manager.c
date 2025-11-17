#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <limits.h>

#include "alias_manager.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define ALIAS_FILE ".my_aliases.txt"

// Forward declarations
static bool is_valid_name(const char *name);
static bool get_alias_file_path(char *buf, size_t size);

// Color codes (set at runtime based on TTY detection)
static const char *COLOR_BLUE   = "";
static const char *COLOR_GREEN  = "";
static const char *COLOR_RED    = "";
static const char *COLOR_YELLOW = "";
static const char *COLOR_RESET  = "";

void am_init_colors(void)
{
    static bool initialized = false;
    if (initialized)
        return;

    if (isatty(STDOUT_FILENO) && isatty(STDERR_FILENO))
    {
        COLOR_BLUE   = "\033[0;34m";
        COLOR_GREEN  = "\033[0;32m";
        COLOR_RED    = "\033[0;31m";
        COLOR_YELLOW = "\033[0;33m";
        COLOR_RESET  = "\033[0m";
    }
    else
    {
        COLOR_BLUE = COLOR_GREEN = COLOR_RED = COLOR_YELLOW = COLOR_RESET = "";
    }
    initialized = true;
}

const char *am_color_blue(void)   { return COLOR_BLUE; }
const char *am_color_green(void)  { return COLOR_GREEN; }
const char *am_color_red(void)    { return COLOR_RED; }
const char *am_color_yellow(void) { return COLOR_YELLOW; }
const char *am_color_reset(void)  { return COLOR_RESET; }

static void log_info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    va_end(args);
}

static void log_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%sError:%s ", COLOR_RED, COLOR_RESET);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static void log_success(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("%s✓%s ", COLOR_GREEN, COLOR_RESET);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

static FILE *open_alias_file(const char *mode)
{
    char path[PATH_MAX];
    if (!get_alias_file_path(path, sizeof(path)))
        return NULL;

    FILE *fp = fopen(path, mode);
    if (!fp && errno != ENOENT)
        log_error("Cannot access %s: %s", path, strerror(errno));
    return fp;
}

static ErrorCode write_alias(FILE *fp, const char *name, const char *command)
{
    char escaped_cmd[MAX_LINE_LENGTH];
    size_t j = 0;

    for (const char *src = command; *src && j < MAX_LINE_LENGTH - 5; src++) {
        if (*src == '\'') {
            escaped_cmd[j++] = '\'';
            escaped_cmd[j++] = '\\';
            escaped_cmd[j++] = '\'';
            escaped_cmd[j++] = '\'';
        } else {
            escaped_cmd[j++] = *src;
        }
    }
    escaped_cmd[j] = '\0';

    return fprintf(fp, "alias %s='%s'\n", name, escaped_cmd) > 0 ?
        SUCCESS : ERR_FILE_ACCESS;
}

ErrorCode am_add(const char *name, const char *command)
{
    am_init_colors();

    if (!is_valid_name(name)) {
        log_error("Invalid alias name");
        return ERR_INVALID_ARG;
    }
    if (!command || !*command) {
        log_error("Command cannot be empty");
        return ERR_INVALID_ARG;
    }

    char path[PATH_MAX];
    if (!get_alias_file_path(path, sizeof(path)))
        return ERR_HOME_DIR;

    ErrorCode err = am_remove(name, true);
    if (err != SUCCESS && err != ERR_FILE_ACCESS)
        return err;

    FILE *fp = fopen(path, "a");
    if (!fp) {
        log_error("Cannot write to alias file");
        return ERR_FILE_ACCESS;
    }

    err = write_alias(fp, name, command);
    fclose(fp);

    if (err == SUCCESS)
        log_success("Added alias '%s%s%s' → '%s'", COLOR_YELLOW, name, COLOR_RESET, command);

    return err;
}

ErrorCode am_remove(const char *name, bool force)
{
    am_init_colors();

    if (!is_valid_name(name)) {
        if (!force) log_error("Invalid alias name");
        return ERR_INVALID_ARG;
    }

    char path[PATH_MAX];
    if (!get_alias_file_path(path, sizeof(path)))
        return ERR_HOME_DIR;

    if (access(path, F_OK) != 0)
        return SUCCESS;

    char temp_path[PATH_MAX];
    int written = snprintf(temp_path, sizeof(temp_path), "%s.tmp", path);
    if (written < 0 || written >= (int)sizeof(temp_path))
        return ERR_SYSTEM;

    FILE *src = fopen(path, "r");
    if (!src) return ERR_FILE_ACCESS;

    FILE *dst = fopen(temp_path, "w");
    if (!dst) {
        fclose(src);
        return ERR_FILE_ACCESS;
    }

    char line[MAX_LINE_LENGTH];
    bool found = false;
    size_t name_len = strlen(name);

    while (fgets(line, sizeof(line), src)) {
        if (strstr(line, "alias ") == line &&
            strncmp(line + 6, name, name_len) == 0 &&
            line[6 + name_len] == '=') {
            found = true;
            if (!force) {
                printf("Remove alias: %sAre you sure? [Y/n] ", line);
                char response[10];
                if (fgets(response, sizeof(response), stdin) &&
                    response[0] != 'y' && response[0] != 'Y' && response[0] != '\n') {
                    log_info("Operation cancelled");
                    fclose(src);
                    fclose(dst);
                    remove(temp_path);
                    return SUCCESS;
                }
            }
            continue;
        }
        fputs(line, dst);
    }

    fclose(src);
    fclose(dst);

    if (!found) {
        if (!force) printf("%sNote:%s Alias '%s' not found\n", COLOR_BLUE, COLOR_RESET, name);
        remove(temp_path);
        return SUCCESS;
    }

    struct stat st;
    if (stat(path, &st) == 0)
        chmod(temp_path, st.st_mode);

    if (rename(temp_path, path) != 0) {
        remove(temp_path);
        return ERR_SYSTEM;
    }

    if (!force)
        log_success("Removed alias '%s%s%s'", COLOR_YELLOW, name, COLOR_RESET);

    return SUCCESS;
}

ErrorCode am_list(const char *filter)
{
    am_init_colors();

    FILE *fp = open_alias_file("r");
    if (!fp) {
        if (errno != ENOENT)
            log_error("Cannot read alias file");
        return ERR_FILE_ACCESS;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "alias ") != line) continue;
        if (!filter || strstr(line + 6, filter)) {
            printf("%s", line + 6);
            count++;
        }
    }

    fclose(fp);

    if (count == 0 && filter)
        printf("%sNote:%s No aliases found matching '%s'\n", COLOR_BLUE, COLOR_RESET, filter);

    return SUCCESS;
}

// Utility implementations
const char *error_message(ErrorCode err)
{
    static const char *messages[] = {
        "Success",
        "Invalid arguments",
        "File access error",
        "Home directory not found",
        "System error"
    };

    if (err < 0 || err > ERR_SYSTEM)
        return "Unknown error";

    return messages[err];
}

bool is_valid_name(const char *name)
{
    if (!name || !*name)
        return false;
    if (!isalpha(name[0]) && name[0] != '_')
        return false;

    for (; *name; name++)
        if (!isalnum(*name) && *name != '_')
            return false;

    return true;
}

bool get_alias_file_path(char *buf, size_t size)
{
    const char *home = getenv("HOME");
    if (!home)
        return false;

    int written = snprintf(buf, size, "%s/%s", home, ALIAS_FILE);
    return written > 0 && (size_t)written < size;
}
