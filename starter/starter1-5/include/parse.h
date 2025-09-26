#pragma once
#include "lexer.h"
typedef enum { CMD_SIMPLE } cmd_type;
typedef struct command {
	cmd_type type;
	char **argv;
	int argc;
} command;
command *parse_command(tokenlist *tokens);
void free_command(command *cmd);
