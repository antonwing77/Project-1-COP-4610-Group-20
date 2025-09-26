#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

tokenlist *expand_tokens(tokenlist *tokens) {
    tokenlist *expanded = new_tokenlist();
    char buffer[PATH_MAX + 1];
    for (int i = 0; i < tokens->size; i++) {
        char *tok = tokens->items[i];
        if (tok[0] == '$') {
            char *value = getenv(tok + 1);
            if (value != NULL) {
                strncpy(buffer, value, sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';
            } else {
                buffer[0] = '\0';
            }
            add_token(expanded, buffer);
        } else if (tok[0] == '~' && (tok[1] == '\0' || tok[1] == '/')) {
            char *home = getenv("HOME");
            if (!home) home = getenv("USERPROFILE");
            if (home) {
                snprintf(buffer, sizeof(buffer), "%s%s", home, tok + 1);
            } else {
                strncpy(buffer, tok, sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';
            }
            add_token(expanded, buffer);
        } else {
            add_token(expanded, tok);
        }
    }
    return expanded;
}
