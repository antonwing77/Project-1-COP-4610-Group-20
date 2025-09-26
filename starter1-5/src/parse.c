#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "parse.h"

static void free_single_command(command *cmd) {
    if (!cmd) return;
    for (int i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
    free(cmd->argv);
    free(cmd->input_file);
    free(cmd->output_file);
    free(cmd);
}

pipeline *parse_pipeline(tokenlist *tokens) {
    if (!tokens || tokens->size == 0) return NULL;

    pipeline *pl = (pipeline *)calloc(1, sizeof(pipeline));
    bool has_redir = false;
    command *current = NULL;
    int current_argc = 0;
    char **current_argv = NULL;
    bool saw_amp = false;

    for (int i = 0; i < tokens->size; i++) {
        char *tok = tokens->items[i];

        if (strcmp(tok, "&") == 0) {
            if (i != tokens->size - 1) {
                fprintf(stderr, "Error: '&' must be at the end\n");
                goto error;
            }
            pl->background = true;
            saw_amp = true;
            break;
        }

        if (strcmp(tok, "|") == 0) {
            if (current_argc == 0) {
                fprintf(stderr, "Error: empty command in pipeline\n");
                goto error;
            }
            current->argc = current_argc;
            current->argv = (char **)realloc(current->argv, (current_argc + 1) * sizeof(char *));
            if (!current->argv) {
                perror("realloc");
                goto error;
            }
            current->argv[current_argc] = NULL;
            pl->cmds[pl->num_cmds++] = current;

            if (pl->num_cmds > 3) {
                fprintf(stderr, "Error: too many pipes (max 2)\n");
                goto error;
            }

            current = (command *)calloc(1, sizeof(command));
            current_argv = (char **)malloc(tokens->size * sizeof(char *));  // Max possible
            if (!current_argv) {
                perror("malloc");
                goto error;
            }
            current_argc = 0;
            current->argv = current_argv;
            continue;
        }

        if (!current) {
            current = (command *)calloc(1, sizeof(command));
            current_argv = (char **)malloc(tokens->size * sizeof(char *));
            if (!current_argv) {
                perror("malloc");
                goto error;
            }
            current_argc = 0;
            current->argv = current_argv;
        }

        if (strcmp(tok, "<") == 0) {
            if (i + 1 >= tokens->size) {
                fprintf(stderr, "Error: missing input file after '<'\n");
                goto error;
            }
            if (current->input_file != NULL) {
                fprintf(stderr, "Error: multiple input redirections\n");
                goto error;
            }
            current->input_file = strdup(tokens->items[i + 1]);
            has_redir = true;
            i++;
            continue;
        } else if (strcmp(tok, ">") == 0) {
            if (i + 1 >= tokens->size) {
                fprintf(stderr, "Error: missing output file after '>'\n");
                goto error;
            }
            if (current->output_file != NULL) {
                fprintf(stderr, "Error: multiple output redirections\n");
                goto error;
            }
            current->output_file = strdup(tokens->items[i + 1]);
            has_redir = true;
            i++;
            continue;
        }

        current_argv[current_argc] = strdup(tok);
        current_argc++;
    }

    if (current && current_argc > 0) {
        current->argc = current_argc;
        current->argv = (char **)realloc(current->argv, (current_argc + 1) * sizeof(char *));
        if (!current->argv) {
            perror("realloc");
            goto error;
        }
        current->argv[current_argc] = NULL;
        pl->cmds[pl->num_cmds++] = current;
    } else if (current_argc == 0 && !saw_amp) {
        fprintf(stderr, "Error: empty command\n");
        goto error;
    }

    if (pl->num_cmds == 0) goto error;

    if (pl->num_cmds > 1 && has_redir) {
        fprintf(stderr, "Error: I/O redirection not allowed with piping\n");
        goto error;
    }

    return pl;

error:
    for (int j = 0; j < pl->num_cmds; j++) {
        free_single_command(pl->cmds[j]);
    }
    free(pl);
    return NULL;
}

void free_pipeline(pipeline *pl) {
    if (!pl) return;
    for (int i = 0; i < pl->num_cmds; i++) {
        free_single_command(pl->cmds[i]);
    }
    free(pl);
}

void free_command(command *cmd) {
    free_single_command(cmd);
}