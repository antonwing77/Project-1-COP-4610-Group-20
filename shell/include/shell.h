#pragma once
#include "lexer.h"
#include <sys/types.h>

// Structure to track background jobs
typedef struct {
    pid_t pid;
    int job_num;
    char *cmd_line;
} job;

// Structure to store command history
typedef struct {
    char *commands[3];
    int count;
    int index;
} history;


tokenlist *expand_tokens(tokenlist *tokens); // Expand tokens for env variables and tildes
void add_to_history(history *hist, const char *cmd); // Add command to history
void free_history(history *hist); // Free memory for history
void init_history(history *hist); // Initialize history struct
void add_job(job *jobs, int *job_count, pid_t pid, const char *cmd_line); // Add background job to job list
void check_jobs(job *jobs, int *job_count); // Check for completed jobs
void free_jobs(job *jobs, int job_count); // Free memory for jobs
