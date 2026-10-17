#include "includes.h"
#include <sys/stat.h>

static char dirs[MAX_INCLUDE_DIRS][MAX_PATH];
static int n_dirs = 0;

static char seen[MAX_INCLUDED][MAX_PATH];
static int n_seen = 0;

static const char *pending_dirs[MAX_INCLUDE_DIRS];
static int n_pending = 0;

void reset_include_dirs(void)
{
    n_dirs = 0;
}

void add_include_dir(const char *dir)
{
    if (n_dirs >= MAX_INCLUDE_DIRS)
        return;

    strncpy(dirs[n_dirs], dir, MAX_PATH - 1);
    dirs[n_dirs][MAX_PATH - 1] = '\0';
    n_dirs++;
}

void set_pending_include_dirs(const char *dirs_in[], int n)
{
    if (n > MAX_INCLUDE_DIRS)
        n = MAX_INCLUDE_DIRS;

    n_pending = n;

    for (int i = 0; i < n; i++)
        pending_dirs[i] = dirs_in[i];
}

void apply_pending_include_dirs(void)
{
    for (int i = 0; i < n_pending; i++)
        add_include_dir(pending_dirs[i]);
}

static int file_ok(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

int find_include(const char *name, int quoted,
                 char *out, int out_size)
{
    if (name[0] == '/') {
        if (file_ok(name)) {
            strncpy(out, name, out_size - 1);
            out[out_size - 1] = '\0';
            return 1;
        }
        return 0;
    }

    if (quoted && file_ok(name)) {
        strncpy(out, name, out_size - 1);
        out[out_size - 1] = '\0';
        return 1;
    }

    for (int i = 0; i < n_dirs; i++) {
        char tmp[MAX_PATH];

        strncpy(tmp, dirs[i], MAX_PATH - 1);
        tmp[MAX_PATH - 1] = '\0';

        strcat(tmp, "/");
        strncat(tmp, name, MAX_PATH - strlen(tmp) - 1);

        if (file_ok(tmp)) {
            strncpy(out, tmp, out_size - 1);
            out[out_size - 1] = '\0';
            return 1;
        }
    }

    return 0;
}

int already_included(const char *path)
{
    for (int i = 0; i < n_seen; i++) {
        if (strcmp(seen[i], path) == 0)
            return 1;
    }

    return 0;
}

void mark_included(const char *path)
{
    if (n_seen >= MAX_INCLUDED)
        return;

    strncpy(seen[n_seen], path, MAX_PATH - 1);
    seen[n_seen][MAX_PATH - 1] = '\0';
    n_seen++;
}

void reset_included(void)
{
    n_seen = 0;
}
