#pragma once
#include "parse.h"
#include <stdbool.h>
#include <sys/types.h>  // For pid_t

void exec_external(command *cmd);
int execute_pipeline(pipeline *pl, pid_t *last_pid_out);