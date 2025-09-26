#pragma once
#include "lexer.h"
#include <sys/types.h>

typedef struct {
    pid_t pid;
    int job_num;
    char *cmd_line;
} job;

typedef struct {
    char *commands[3];
    int count;
    int index;
} history;

tokenlist *expand_tokens(tokenlist *tokens);
void add_to_history(history *hist, const char *cmd);
void free_history(history *hist);
void init_history(history *hist);
void add_job(job *jobs, int *job_count, pid_t pid, const char *cmd_line);
void check_jobs(job *jobs, int *job_count);
void free_jobs(job *jobs, int job_count);
