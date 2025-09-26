#define _POSIX_C_SOURCE 200809L
#include "execute.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifdef _WIN32
#include <windows.h>
#define PATH_MAX MAX_PATH
#else
#include <pwd.h>
#include <limits.h>
#endif
#include "shell.h"
#include "lexer.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MAX_LINE 2048
#define MAX_JOBS 10

void print_prompt() {
    char cwd[PATH_MAX];
    char hostname[256] = "localhost";
    char *user = getenv("USER");
    if (!user) user = getenv("USERNAME");
    if (!user) user = "user";
    gethostname(hostname, sizeof(hostname));
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        strcpy(cwd, "?");
    }
    printf("%s@%s:%s> ", user, hostname, cwd);
    fflush(stdout);
}

int main() {
    char line[MAX_LINE];
    history hist;
    job jobs[MAX_JOBS];
    int job_count = 0;
    init_history(&hist);

    while (1) {
        check_jobs(jobs, &job_count);  // Check before prompt for better timing
        print_prompt();
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;

        tokenlist *tokens = get_tokens(line);
        tokenlist *expanded = expand_tokens(tokens);
        pipeline *pl = parse_pipeline(expanded);
        if (pl) add_to_history(&hist, line);

        if (pl && pl->num_cmds == 1) {
            command *cmd = pl->cmds[0];
            if (cmd && cmd->argc > 0) {
                if (strcmp(cmd->argv[0], "exit") == 0) {
                    // Wait for background jobs
                    while (job_count > 0) {
                        check_jobs(jobs, &job_count);
                        sleep(0.1);  // 0.1s poll to avoid busy loop
                    }
                    // Print history
                    if (hist.count == 0) {
                        printf("No valid commands executed.\n");
                    } else {
                        int start = (hist.index - hist.count + 3) % 3;
                        for (int i = 0; i < hist.count; i++) {
                            int idx = (start + i) % 3;
                            if (hist.commands[idx]) printf("%s\n", hist.commands[idx]);
                        }
                    }
                    free_pipeline(pl);
                    free_tokens(tokens);
                    free_tokens(expanded);
                    free_history(&hist);
                    free_jobs(jobs, job_count);
                    return 0;
                } else if (strcmp(cmd->argv[0], "cd") == 0) {
                    if (cmd->argc > 2) {
                        fprintf(stderr, "Error: too many arguments to cd\n");
                    } else {
                        char *path = (cmd->argc == 1) ? getenv("HOME") : cmd->argv[1];
                        if (path == NULL) {
                            fprintf(stderr, "Error: HOME not set\n");
                        } else if (chdir(path) != 0) {
                            perror("cd");
                        }
                    }
                    free_pipeline(pl);
                    free_tokens(tokens);
                    free_tokens(expanded);
                    continue;
                } else if (strcmp(cmd->argv[0], "jobs") == 0) {
                    if (job_count == 0) {
                        printf("No active background processes.\n");
                    } else {
                        for (int i = 0; i < job_count; i++) {
                            printf("[%d]+ %d %s\n", jobs[i].job_num, jobs[i].pid, jobs[i].cmd_line);
                        }
                    }
                    free_pipeline(pl);
                    free_tokens(tokens);
                    free_tokens(expanded);
                    continue;
                }
            }
        }

        if (pl) {
            pid_t last_pid = 0;
            execute_pipeline(pl, &last_pid);
            if (pl->background && last_pid > 0) {
                char cmd_line[MAX_LINE];
                strncpy(cmd_line, line, MAX_LINE - 1);
                cmd_line[MAX_LINE - 1] = '\0';
                add_job(jobs, &job_count, last_pid, cmd_line);
                printf("[%d] %d\n", jobs[job_count - 1].job_num, last_pid);
            }
            free_pipeline(pl);
        }
        free_tokens(tokens);
        free_tokens(expanded);
    }

    free_history(&hist);
    free_jobs(jobs, job_count);
    return 0;
}
