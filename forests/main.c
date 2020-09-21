#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "bst.h"
#include "work.h"
#include "checkinput.h"

int main() {
    char* input_line = NULL;
    size_t input_line_size = 0;
    Tree bst_tree = NULL; // main bst_tree used in this problem
    // evry node of bst_tree represents a forest, it has a pointer (->subtree)
    // to another bst tree representing all trees in that forest.
    // evry node in that bst tree represents a certain tree, and has a
    // pointer (->subtree) to another bst tree representing all animals in
    // that tree. Evry node in that tree represents a certain animal.

    command* current_command = (command*)malloc(sizeof(command)); 
    if (current_command == NULL) exit(1); // failed to allocate memory

    while (getline(&input_line, &input_line_size, stdin) != EOF) {
        if (errno == ENOMEM) exit(1); // failed to allocate memory
        current_command->words[0] = 
        current_command->words[1] = 
        current_command->words[2] = NULL;
        bool proper_command = check_input(input_line, current_command);
        // if proper_command is true, a certain operation on bst_tree
        // needs to be performed
        if (proper_command) operation_handle(&bst_tree, current_command);
    }
    if (errno == ENOMEM) exit(1); // faile to allocate memory
    free(input_line);
    free(current_command);
    freeall(&bst_tree);
    return 0;
}
