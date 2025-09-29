#pragma once
#include "lexer.h"
#include <stdbool.h>

typedef enum { CMD_SIMPLE } cmd_type;

// Structure representing a single command
typedef struct command {
    cmd_type type;
    char **argv;
    int argc;
    char *input_file;   // For < file
    char *output_file;  // For > file
    bool background;
} command;

// Structure representing a pipeline of commands
typedef struct pipeline {
    command *cmds[3];
    int num_cmds;
    bool background;
} pipeline;

pipeline *parse_pipeline(tokenlist *tokens); // Parses tokens into a pipeline structure
void free_pipeline(pipeline *pl); // Frees memory for pipeline
void free_command(command *cmd); // Frees memory for single command
