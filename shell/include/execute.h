#pragma once
#include "parse.h"
#include <stdbool.h>
#include <sys/types.h>

void exec_external(command *cmd); // Executes external command
int execute_pipeline(pipeline *pl, pid_t *last_pid_out); // Executes pipeline of commands