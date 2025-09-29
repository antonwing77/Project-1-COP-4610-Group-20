#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "shell.h"

tokenlist *expand_tokens(tokenlist *tokens) {
    // Create new list for expanded tokens
    tokenlist *expanded = new_tokenlist();
    char buffer[PATH_MAX + 1];
    // Process each token
    for (int i = 0; i < tokens->size; i++) {
        char *tok = tokens->items[i];
        // Expand env variables
        if (tok[0] == '$') {
            char *value = getenv(tok + 1);
            if (value != NULL) {
                strncpy(buffer, value, sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';
            } else {
                buffer[0] = '\0';
            }
            add_token(expanded, buffer);
        // Expand tilde
        } else if (tok[0] == '~' && (tok[1] == '\0' || tok[1] == '/')) {
            char *home = getenv("HOME");
            if (!home) home = getenv("USERPROFILE");
            if (home) {
                snprintf(buffer, sizeof(buffer), "%s%s", home, tok + 1);
            } else {
                strncpy(buffer, tok, sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';
            }
            add_token(expanded, buffer);
        } else {
            add_token(expanded, tok);
        }
    }
    return expanded;
}

// Initialize history struct w base values
void init_history(history *hist) {
    hist->count = 0;
    hist->index = 0;
    for (int i = 0; i < 3; i++) hist->commands[i] = NULL;
}

void add_to_history(history *hist, const char *cmd) {
    if (strcmp(cmd, "exit") == 0) return;
    if (hist->commands[hist->index]) free(hist->commands[hist->index]); // Free existing command
    hist->commands[hist->index] = strdup(cmd); // Store new command
    hist->index = (hist->index + 1) % 3; // update index & count
    if (hist->count < 3) hist->count++;
}

void free_history(history *hist) {
    for (int i = 0; i < 3; i++) free(hist->commands[i]);
}

void add_job(job *jobs, int *job_count, pid_t pid, const char *cmd_line) {
    if (*job_count >= 10) return;
    // Store job details
    jobs[*job_count].pid = pid;
    jobs[*job_count].job_num = *job_count + 1;
    jobs[*job_count].cmd_line = strdup(cmd_line);
    (*job_count)++;
}

void check_jobs(job *jobs, int *job_count) {
    int i = 0;
    while (i < *job_count) {
        int status;
        // Check job status
        pid_t ret = waitpid(jobs[i].pid, &status, WNOHANG);
        if (ret > 0) {
            // Report the completed job
            printf("[%d]+ done %s\n", jobs[i].job_num, jobs[i].cmd_line);
            free(jobs[i].cmd_line);
            // Shift array to remove the job
            for (int j = i; j < *job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
            (*job_count)--;
        } else if (ret < 0) {
            // Error handle & remove job
            free(jobs[i].cmd_line);
            for (int j = i; j < *job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
            (*job_count)--;
        } else {
            i++;
        }
    }
}

void free_jobs(job *jobs, int job_count) {
    for (int i = 0; i < job_count; i++) free(jobs[i].cmd_line);
}