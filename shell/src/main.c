#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include "shell.h"
#include "lexer.h"
#include "execute.h"
#include "parse.h"

#define MAX_LINE 2048
#define MAX_JOBS 10

void print_prompt() { // Display prompt
    char cwd[PATH_MAX];
    char hostname[256] = "localhost";
    // Get username
    char *user = getenv("USER");
    if (!user) user = getenv("USERNAME");
    if (!user) user = "user";
    // Get hostname
    gethostname(hostname, sizeof(hostname));
    // Get current directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        strcpy(cwd, "?");
    }
    printf("%s@%s:%s> ", user, hostname, cwd);
    fflush(stdout);
}

int main() {
    // Initialize buffers and structures
    char line[MAX_LINE];
    history hist;
    job jobs[MAX_JOBS];
    int job_count = 0;
    init_history(&hist);

    while (1) {
        // Check completed jobs
        check_jobs(jobs, &job_count); 
        print_prompt();
        // Get user input
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        // Tokenize input
        tokenlist *tokens = get_tokens(line);
        tokenlist *expanded = expand_tokens(tokens); // Expand env variables & tildes
        pipeline *pl = parse_pipeline(expanded); // Parse into pipeline
        if (pl) add_to_history(&hist, line);

        // Internal command handle
        if (pl && pl->num_cmds == 1) {
            command *cmd = pl->cmds[0];
            if (cmd && cmd->argc > 0) {
                if (strcmp(cmd->argv[0], "exit") == 0) {
                    // Wait for background jobs
                    while (job_count > 0) {
                        check_jobs(jobs, &job_count);
                        sleep(0.1);
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
                    // Clean up
                    free_pipeline(pl);
                    free_tokens(tokens);
                    free_tokens(expanded);
                    free_history(&hist);
                    free_jobs(jobs, job_count);
                    return 0;
                } else if (strcmp(cmd->argv[0], "cd") == 0) {
                    // cd command handle
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
                    // Clean up
                    free_pipeline(pl);
                    free_tokens(tokens);
                    free_tokens(expanded);
                    continue;
                } else if (strcmp(cmd->argv[0], "jobs") == 0) {
                    // List active background jobs
                    if (job_count == 0) {
                        printf("No active background processes.\n");
                    } else {
                        for (int i = 0; i < job_count; i++) {
                            printf("[%d]+ %d %s\n", jobs[i].job_num, jobs[i].pid, jobs[i].cmd_line);
                        }
                    }
                    // Clean up
                    free_pipeline(pl);
                    free_tokens(tokens);
                    free_tokens(expanded);
                    continue;
                }
            }
        }

        // External command executions
        if (pl) {
            pid_t last_pid = 0;
            execute_pipeline(pl, &last_pid);
            // Track background jobs
            if (pl->background && last_pid > 0) {
                char cmd_line[MAX_LINE];
                strncpy(cmd_line, line, MAX_LINE - 1);
                cmd_line[MAX_LINE - 1] = '\0';
                add_job(jobs, &job_count, last_pid, cmd_line);
                printf("[%d] %d\n", jobs[job_count - 1].job_num, last_pid);
            }
        // Clean up
            free_pipeline(pl);
        }
        free_tokens(tokens);
        free_tokens(expanded);
    }

    // Clean up and exit
    free_history(&hist);
    free_jobs(jobs, job_count);
    return 0;
}
