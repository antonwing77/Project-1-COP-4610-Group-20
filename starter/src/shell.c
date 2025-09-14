#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

void print_shell_prompt(void) {
    char *user = getenv("USER");
    char *machine = getenv("MACHINE"); // May not exist; clarify with instructor
    char *pwd = getenv("PWD");

    // Fallbacks for NULL values
    if (user == NULL) user = "unknown";
    if (machine == NULL) machine = "localhost"; // Fallback if $MACHINE not set
    if (pwd == NULL) pwd = "unknown";

    printf("%s@%s:%s> ", user, machine, pwd);
    fflush(stdout); // Ensure prompt is displayed immediately
}

tokenlist *expand_env_variables(tokenlist *tokens) {
    tokenlist *expanded = new_tokenlist();
    char buffer[256]; // Buffer for expanded values

    for (int i = 0; i < tokens->size; i++) {
        if (tokens->items[i][0] == '$') {
            char *value = getenv(tokens->items[i] + 1); // Skip the '$'
            if (value != NULL) {
                strncpy(buffer, value, sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0'; // Ensure null-termination
            } else {
                buffer[0] = '\0'; // Empty string if variable not found
            }
        } else {
            strncpy(buffer, tokens->items[i], sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        }
        add_token(expanded, buffer);
    }

    return expanded;
}