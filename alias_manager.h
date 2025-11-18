#ifndef ALIAS_MANAGER_H
#define ALIAS_MANAGER_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_LINE_LENGTH 4096

typedef enum
{
    SUCCESS = 0,
    ERR_INVALID_ARG,
    ERR_FILE_ACCESS,
    ERR_HOME_DIR,
    ERR_SYSTEM
} ErrorCode;

void am_init_colors(void);
const char *am_color_blue(void);
const char *am_color_green(void);
const char *am_color_red(void);
const char *am_color_yellow(void);
const char *am_color_reset(void);
ErrorCode am_add(const char *name, const char *command);
ErrorCode am_remove(const char *name, bool force);
ErrorCode am_list(const char *filter);
bool am_get_path(char *buf, size_t size);
const char *error_message(ErrorCode err);

#endif
