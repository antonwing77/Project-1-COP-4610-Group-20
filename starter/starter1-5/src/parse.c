#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "parse.h"

command *parse_command(tokenlist *tokens) {
	if (!tokens || tokens->size == 0) return NULL;
	command *cmd = (command *)calloc(1, sizeof(command));
	cmd->type = CMD_SIMPLE;
	cmd->argc = 0;
	cmd->argv = (char **)malloc((tokens->size + 1) * sizeof(char *));
	int i = 0;
	while (i < tokens->size) {
		char *tok = tokens->items[i];
		cmd->argv[cmd->argc++] = strdup(tok);
		i++;
	}
	cmd->argv[cmd->argc] = NULL;
	return cmd;
}

void free_command(command *cmd) {
	if (!cmd) return;
	for (int i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
	free(cmd->argv);
	free(cmd);
}
