#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "execute.h"

int execute_command(command *cmd) {
    if (!cmd || !cmd->argv || cmd->argc == 0) return 0;
    pid_t pid = fork();
    if (pid == 0) {
        exec_external(cmd);
        exit(1);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    } else {
        perror("fork");
    }
    return 0;
}

void exec_external(command *cmd) {
    if (!cmd || !cmd->argv || !cmd->argv[0]) exit(1);
    char *prog = cmd->argv[0];
    if (strchr(prog, '/')) {
        execv(prog, cmd->argv);
        perror("execv");
        exit(1);
    }
    char *path = getenv("PATH");
    if (!path) path = "/bin:/usr/bin";
    char *paths = strdup(path);
    char *saveptr = NULL;
    char *dir = strtok_r(paths, ":", &saveptr);
    char fullpath[PATH_MAX+1];
    while (dir) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, prog);
        execv(fullpath, cmd->argv);
        dir = strtok_r(NULL, ":", &saveptr);
    }
    free(paths);
    fprintf(stderr, "%s: command not found\n", prog);
    exit(1);
}
