#include "alias_manager.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef PATH_MAX
#    define PATH_MAX 4096
#endif

#define XDG_CONFIG_DIR "am"
#define XDG_ALIAS_FILE "aliases.txt"

static bool is_valid_name(const char *name);
static bool mkdirp(const char *path);

static const char *COLOR_BLUE = "";
static const char *COLOR_GREEN = "";
static const char *COLOR_RED = "";
static const char *COLOR_YELLOW = "";
static const char *COLOR_RESET = "";

void am_init_colors(void)
{
    static bool initialized = false;
    if (initialized)
        return;

    if (isatty(STDOUT_FILENO) && isatty(STDERR_FILENO))
    {
        COLOR_BLUE = "\033[0;34m";
        COLOR_GREEN = "\033[0;32m";
        COLOR_RED = "\033[0;31m";
        COLOR_YELLOW = "\033[0;33m";
        COLOR_RESET = "\033[0m";
    }
    initialized = true;
}

const char *am_color_blue(void)
{
    return COLOR_BLUE;
}
const char *am_color_green(void)
{
    return COLOR_GREEN;
}
const char *am_color_red(void)
{
    return COLOR_RED;
}
const char *am_color_yellow(void)
{
    return COLOR_YELLOW;
}
const char *am_color_reset(void)
{
    return COLOR_RESET;
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
    if (!am_get_path(path, sizeof(path)))
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

    for (const char *src = command; *src && j < MAX_LINE_LENGTH - 5; src++)
    {
        if (*src == '\'')
        {
            escaped_cmd[j++] = '\'';
            escaped_cmd[j++] = '\\';
            escaped_cmd[j++] = '\'';
            escaped_cmd[j++] = '\'';
        }
        else
        {
            escaped_cmd[j++] = *src;
        }
    }
    escaped_cmd[j] = '\0';

    return fprintf(fp, "alias %s='%s'\n", name, escaped_cmd) > 0
        ? SUCCESS
        : ERR_FILE_ACCESS;
}

ErrorCode am_add(const char *name, const char *command)
{
    am_init_colors();

    if (!is_valid_name(name))
    {
        log_error("Invalid alias name");
        return ERR_INVALID_ARG;
    }
    if (!command || !*command)
    {
        log_error("Command cannot be empty");
        return ERR_INVALID_ARG;
    }

    char path[PATH_MAX];
    if (!am_get_path(path, sizeof(path)))
        return ERR_HOME_DIR;

    ErrorCode err = am_remove(name, true);
    if (err != SUCCESS && err != ERR_FILE_ACCESS)
        return err;

    if (!mkdirp(path))
    {
        log_error("Cannot create directory for alias file: %s",
                  strerror(errno));
        return ERR_FILE_ACCESS;
    }

    FILE *fp = fopen(path, "a");
    if (!fp)
    {
        log_error("Cannot write to alias file");
        return ERR_FILE_ACCESS;
    }

    err = write_alias(fp, name, command);
    fclose(fp);

    if (err == SUCCESS)
    {
        log_success("Added alias '%s%s%s' → '%s'", COLOR_YELLOW, name,
                    COLOR_RESET, command);
        printf("%shint:%s Run '%seval \"$(am reload)\"%s' to use it in this "
               "shell\n",
               COLOR_BLUE, COLOR_RESET, COLOR_GREEN, COLOR_RESET);
    }

    return err;
}

ErrorCode am_remove(const char *name, bool force)
{
    am_init_colors();

    if (!is_valid_name(name))
    {
        if (!force)
            log_error("Invalid alias name");
        return ERR_INVALID_ARG;
    }

    char path[PATH_MAX];
    if (!am_get_path(path, sizeof(path)))
        return ERR_HOME_DIR;

    if (access(path, F_OK) != 0)
        return SUCCESS;

    char temp_path[PATH_MAX];
    int written = snprintf(temp_path, sizeof(temp_path), "%s.tmp", path);
    if (written < 0 || written >= (int)sizeof(temp_path))
        return ERR_SYSTEM;

    FILE *src = fopen(path, "r");
    if (!src)
        return ERR_FILE_ACCESS;

    FILE *dst = fopen(temp_path, "w");
    if (!dst)
    {
        fclose(src);
        return ERR_FILE_ACCESS;
    }

    char line[MAX_LINE_LENGTH];
    bool found = false;
    size_t name_len = strlen(name);

    while (fgets(line, sizeof(line), src))
    {
        if (strstr(line, "alias ") == line
            && strncmp(line + 6, name, name_len) == 0
            && line[6 + name_len] == '=')
        {
            found = true;
            if (!force)
            {
                printf("Remove alias: %sAre you sure? [Y/n] ", line);
                char response[10];
                if (fgets(response, sizeof(response), stdin)
                    && response[0] != 'y' && response[0] != 'Y'
                    && response[0] != '\n')
                {
                    printf("Operation cancelled\n");
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

    if (!found)
    {
        if (!force)
            printf("%sNote:%s Alias '%s' not found\n", COLOR_BLUE, COLOR_RESET,
                   name);
        remove(temp_path);
        return SUCCESS;
    }

    struct stat st;
    if (stat(path, &st) == 0)
        chmod(temp_path, st.st_mode);

    if (rename(temp_path, path) != 0)
    {
        remove(temp_path);
        return ERR_SYSTEM;
    }

    if (!force)
    {
        log_success("Removed alias '%s%s%s'", COLOR_YELLOW, name, COLOR_RESET);
        printf("%shint:%s Run '%sunalias %s%s' to remove it from this shell\n",
               COLOR_BLUE, COLOR_RESET, COLOR_GREEN, name, COLOR_RESET);
    }

    return SUCCESS;
}

ErrorCode am_list(const char *filter)
{
    am_init_colors();

    FILE *fp = open_alias_file("r");
    if (!fp)
    {
        if (errno == ENOENT)
        {
            // No aliases file exists yet - this is normal for first-time users
            if (!filter)
            {
                char path[PATH_MAX];
                printf("%sNo aliases yet.%s Add one with: %sam add <name> "
                       "<command>%s\n",
                       COLOR_BLUE, COLOR_RESET, COLOR_GREEN, COLOR_RESET);
                if (am_get_path(path, sizeof(path)))
                    printf("File: %s\n", path);
            }
            return SUCCESS;
        }
        log_error("Cannot read alias file");
        return ERR_FILE_ACCESS;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "alias ") != line)
            continue;
        if (!filter || strstr(line + 6, filter))
        {
            printf("%s", line + 6);
            count++;
        }
    }

    fclose(fp);

    if (count == 0 && filter)
        printf("%sNote:%s No aliases found matching '%s'\n", COLOR_BLUE,
               COLOR_RESET, filter);

    return SUCCESS;
}

// Utility implementations
const char *error_message(ErrorCode err)
{
    static const char *messages[] = { "Success", "Invalid arguments",
                                      "File access error",
                                      "Home directory not found",
                                      "System error" };

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

static bool mkdirp(const char *file_path)
{
    const char *dir_end = strrchr(file_path, '/');
    if (!dir_end)
        return true;

    char dir[PATH_MAX];
    size_t len = dir_end - file_path;
    if (len >= sizeof(dir))
        return false;

    memcpy(dir, file_path, len);
    dir[len] = '\0';

    char *p = dir[0] == '/' ? dir + 1 : dir;
    for (char *s; (s = strchr(p, '/')); p = s + 1)
    {
        *s = '\0';
        if (mkdir(dir, 0755) != 0 && errno != EEXIST)
            return false;
        *s = '/';
    }
    if (mkdir(dir, 0755) != 0 && errno != EEXIST)
        return false;
    return true;
}

bool am_get_path(char *buf, size_t size)
{
    const char *custom = getenv("AM_ALIAS_FILE");
    if (custom)
    {
        int n = snprintf(buf, size, "%s", custom);
        return n > 0 && (size_t)n < size;
    }

    const char *home = getenv("HOME");
    if (!home)
        return false;

    const char *xdg = getenv("XDG_CONFIG_HOME");
    int n = xdg
        ? snprintf(buf, size, "%s/%s/%s", xdg, XDG_CONFIG_DIR, XDG_ALIAS_FILE)
        : snprintf(buf, size, "%s/.config/%s/%s", home, XDG_CONFIG_DIR,
                   XDG_ALIAS_FILE);

    return n > 0 && (size_t)n < size;
}
