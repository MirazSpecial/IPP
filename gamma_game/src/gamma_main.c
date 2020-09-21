/** @file
 * Gamma game main file
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "borders.h"
#include "fau.h"
#include "gamma.h"
#include "batch_mode.h"
#include "inter_mode.h"

/** @brief Prints ERROR and line number for standard error output.
 * @param[in] line_number   - line where the error occured.
 */
static void print_error(int line_number) {
    fprintf(stderr, "ERROR %d\n", line_number);
}

/** @brief Allocates memory for reading commands and returns pointer to it.
 * @return Returns pointer to allocated command structure.
 */
static command* prepare_command_reader() {
    uint32_t* command_args = malloc(sizeof(uint32_t) * 4);
    command* my_command = malloc(sizeof(command));
    if (my_command == NULL || command_args == NULL) {
        free(my_command);
        free(command_args);
        exit(1);
    }
    my_command->args = command_args;
    return my_command;
}

/** @brief Main gamma game function.
 * Reads input and activates batch or interactive mode if called.
 */
int main() {
    bool batch_mode = false, inter_mode = false;
    char* input_line = NULL;
    size_t input_line_size;
    gamma_t* g = NULL;
    int line_number = 0;
    command* my_command = prepare_command_reader();

    // Program will read line after line of input till input file ends
    // or interactive mode starts
    while(!inter_mode && getline(&input_line, &input_line_size, stdin) != EOF) {
        bool proper_line = true, proper_command = true;
        line_number++;
        proper_line = parse_line(input_line, my_command, batch_mode);
        // proper_line means that input line can be interpreted as some command

        if (proper_line && my_command->command_type != '#') {
            // in this point my_command is a valid command, not a comment
            proper_command = run_command(my_command, &g, line_number);
            if (proper_command) {
                // the command was fully valid and ERROR will NOT be printed
                if (my_command->command_type == 'B')
                    batch_mode = true;
                if (my_command->command_type == 'I')
                    inter_mode = true;
            }
        }
        if (!proper_line || !proper_command)
            print_error(line_number);
    }
    free(input_line);
    free(my_command->args);
    free(my_command);

    if (inter_mode) // running interactive mode
        run_interactive(g);

    gamma_delete(g);
    
    return 0;
}