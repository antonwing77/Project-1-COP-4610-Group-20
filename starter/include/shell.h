#pragma once

#include "lexer.h"

void print_shell_prompt(void);
tokenlist *expand_env_variables(tokenlist *tokens);