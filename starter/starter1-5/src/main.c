#define _POSIX_C_SOURCE 200809L
#include "execute.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#define PATH_MAX MAX_PATH
#else
#include <pwd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
#include "shell.h"
#include "lexer.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MAX_LINE 2048

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
	while (1) {
		print_prompt();
		if (!fgets(line, sizeof(line), stdin)) {
			printf("\n");
			break;
		}
		line[strcspn(line, "\n")] = 0;
		if (strlen(line) == 0) continue;
		tokenlist *tokens = get_tokens(line);
		tokenlist *expanded = expand_tokens(tokens);
		command *cmd = parse_command(expanded);
		if (cmd && cmd->argc > 0 && strcmp(cmd->argv[0], "exit") == 0) {
			free_command(cmd);
			free_tokens(tokens);
			free_tokens(expanded);
			break;
		}
		execute_command(cmd);
		free_command(cmd);
		free_tokens(tokens);
		free_tokens(expanded);
	}
	return 0;
}
