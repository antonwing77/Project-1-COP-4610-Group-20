#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include "execute.h"

int execute_pipeline(pipeline *pl, pid_t *last_pid_out) {
    if (!pl || pl->num_cmds == 0) {
        if (last_pid_out) *last_pid_out = 0;
        return 0;
    }
    if (last_pid_out) *last_pid_out = 0;

    // Single command handle
    if (pl->num_cmds == 1) {
        command *cmd = pl->cmds[0];
        if (!cmd || !cmd->argv || cmd->argc == 0) return 0;

        pid_t pid = fork();
        if (pid == 0) {
            // Set up input redirect
            if (cmd->input_file) {
                int fd = open(cmd->input_file, O_RDONLY);
                if (fd < 0) {
                    perror(cmd->input_file);
                    exit(1);
                }
                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("dup2 input");
                    exit(1);
                }
                close(fd);
            }
            // Set up output redirect
            if (cmd->output_file) {
                int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                if (fd < 0) {
                    perror(cmd->output_file);
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2 output");
                    exit(1);
                }
                close(fd);
            }
            // Execute
            exec_external(cmd);
            perror("exec_external");
            exit(1);
        } else if (pid > 0) {
            if (last_pid_out) *last_pid_out = pid;
            if (!pl->background) {
                waitpid(pid, NULL, 0);
            }
        } else {
            perror("fork");
            return -1;
        }
        return 0;
    } else {
        // Multiple command handle
        int num_pipes = pl->num_cmds - 1;
        int pipefds[2][2];
        // Create pipes
        for (int p = 0; p < num_pipes; p++) {
            if (pipe(pipefds[p]) < 0) {
                perror("pipe");
                return -1;
            }
        }

        // Fork for each command
        pid_t pids[3] = {0};
        for (int c = 0; c < pl->num_cmds; c++) {
            pid_t pid = fork();
            pids[c] = pid;
            if (pid < 0) {
                perror("fork");
                return -1;
            }
            if (pid == 0) {
                // Set up input from previous pipe
                if (c > 0) {
                    if (dup2(pipefds[c - 1][0], STDIN_FILENO) < 0) {
                        perror("dup2 stdin");
                        exit(1);
                    }
                }
                // Set up output to next pipe
                if (c < num_pipes) {
                    if (dup2(pipefds[c][1], STDOUT_FILENO) < 0) {
                        perror("dup2 stdout");
                        exit(1);
                    }
                }
                // Close all file descriptors
                for (int p = 0; p < num_pipes; p++) {
                    close(pipefds[p][0]);
                    close(pipefds[p][1]);
                }
                // Execute
                exec_external(pl->cmds[c]);
                perror("exec_external");
                exit(1);
            }
        }
        // Close pipes
        for (int p = 0; p < num_pipes; p++) {
            close(pipefds[p][0]);
            close(pipefds[p][1]);
        }

        if (last_pid_out) *last_pid_out = pids[pl->num_cmds - 1];
        if (!pl->background) {
            for (int c = 0; c < pl->num_cmds; c++) {
                int status;
                waitpid(pids[c], &status, 0);
            }
        }
        return 0;
    }
}

void exec_external(command *cmd) {
    // Determine command validity
    if (!cmd || !cmd->argv || !cmd->argv[0]) exit(1);
    char *prog = cmd->argv[0];
    if (strchr(prog, '/')) {
        execv(prog, cmd->argv);
        perror("execv");
        exit(1);
    }
    // Search PATH for an executable
    char *path = getenv("PATH");
    if (!path) path = "/bin:/usr/bin";
    char *paths = strdup(path);
    char *saveptr = NULL;
    char *dir = strtok_r(paths, ":", &saveptr);
    char fullpath[PATH_MAX + 1];
    while (dir) {
        // Attempt execution on path
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, prog);
        execv(fullpath, cmd->argv);
        dir = strtok_r(NULL, ":", &saveptr);
    }
    // Clean up
    free(paths);
    fprintf(stderr, "%s: command not found\n", prog);
    exit(1);
}
