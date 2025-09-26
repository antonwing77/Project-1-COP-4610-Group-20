#pragma once
#include "lexer.h"
#include <stdbool.h>

typedef enum { CMD_SIMPLE } cmd_type;

typedef struct command {
    cmd_type type;
    char **argv;
    int argc;
    char *input_file;   // For < file
    char *output_file;  // For > file
    bool background;    // Not used per-command; for pipeline
} command;

typedef struct pipeline {
    command *cmds[3];
    int num_cmds;
    bool background;
} pipeline;

pipeline *parse_pipeline(tokenlist *tokens);
void free_pipeline(pipeline *pl);
void free_command(command *cmd);  // Keep for internal use
