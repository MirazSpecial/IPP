/** @file 
 * Implementation for batch mode operations including
 * parsing input lines and finding commands and running those commands.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "borders.h"
#include "fau.h"
#include "gamma.h"
#include "batch_mode.h"

/** @brief Translates number written is string into integer
 * Check if given string can be interpreted as uint32_t
 * returns this integer if yes or return -1 if no.
 * @param[in] word  - checked string.
 * @return Returns found number or -1.
 */
static int64_t interpret_number(char* word) {
    if (word[0] == '-')
        return -1; // can't be negative number
    char* last = NULL;
    uint64_t result = strtoull(word, &last, 10);
    if (result > UINT32_MAX)
        return -1; // number out of range
    // if number of digits in 'result' differs from 'word' length then  
    // not whole 'word' was interpreted so result is not valid
    if (count_digits(result) != strlen(word))
        return -1;
    // if 'result' is 0 then it is possible that 'word' was a single
    // non-numeric char in this case result in not valid 
    if (result == 0 && word[0] != '0')
        return -1;
    return (int64_t)result;
}

/** @brief checks if command can be performed in this moment
 * For given moment in (before of after starting batchmode game) checks
 * if command_type is proper and suits number of parameters.
 * @param[in] my_command    - command to check,
 * @param[in] batch_mode    - has batch mode game already started.
 * @return if command can be performed.
 */
static bool check_command_type(command* my_command, bool batch_mode) {
    if (batch_mode) { // commands possible only after starting batch mode game
        if (my_command->command_type == 'm' || my_command->command_type == 'g')
            if (my_command->arguments_number == 3)
                return true;
        if (my_command->command_type == 'b' || my_command->command_type == 'f' ||
            my_command->command_type == 'q')
            if (my_command->arguments_number == 1)
                return true;
        if (my_command->command_type == 'p')
            if (my_command->arguments_number == 0)
                return true;
    }
    else { // commands possible only before starting batch mode game
        if (my_command->command_type == 'B' && my_command->arguments_number == 4)
            return true;
        if (my_command->command_type == 'I' && my_command->arguments_number == 4)
            return true;
    }
    return false;
}

bool parse_line(char* input_line, command* my_command, bool batch_mode) {
    if (input_line[0] == '#' || input_line[0] == '\n') {
        my_command->command_type = '#';
        return true; // line is a comment or is just single '\n' character
    }
    const char whitespace[] = " \t\v\f\r\n";

    if (strchr(whitespace, input_line[0]) != NULL)
        return false; // non-empty line starts with white character
    if (input_line[strlen(input_line) - 1] != '\n')
        return false; // line not ended with '\n'
    
    char* first_word = strtok(input_line, whitespace);
    if (first_word == NULL)
        return false; // non-empty line with only white characters
    if (strlen(first_word) != 1)
        return false; // first word is not single char

    my_command->command_type = first_word[0];

    int number_count = 0;
    char* one_word = strtok(NULL, whitespace);
    while (one_word != NULL) { 
        if (number_count > 3)
            return false; // too many parameters
        int64_t number = interpret_number(one_word);
        if (number == -1)
            return false; // number error
        my_command->args[number_count] = (uint32_t)number;
        number_count++;
        one_word = strtok(NULL, whitespace);
    }
    my_command->arguments_number = number_count;
    if (check_command_type(my_command, batch_mode))
        return true;
    else 
        return false;
}

/** @brief Runs given command on given gamma game
 * For command which does not need memory allocation runs it and 
 * prints its result.
 * @param[in] my_command    - given command to run,
 * @param[in, out] g        - pointer to structure holding game status.
 */
static void run_in_batch_mode(command* my_command, gamma_t* g) {
    if (my_command->command_type == 'm') {
        bool succes = gamma_move(g, my_command->args[0], my_command->args[1],
                                    my_command->args[2]);
        if (succes) // if gamma_move was succesfully performed
            printf("1\n");
        else
            printf("0\n");
    }
    if (my_command->command_type == 'g') {
        bool succes = gamma_golden_move(g, my_command->args[0], my_command->args[1],
                                    my_command->args[2]);
        if (succes) // if gamma_golden_move was succesfully performed
            printf("1\n");
        else
            printf("0\n");
    }
    if (my_command->command_type == 'q') {
        bool succes = gamma_golden_possible(g, my_command->args[0]);
        if (succes)
            printf("1\n");
        else
            printf("0\n");
    }
    if (my_command->command_type == 'b')
        printf("%lu\n", gamma_busy_fields(g, my_command->args[0]));
    if (my_command->command_type == 'f')
        printf("%lu\n", gamma_free_fields(g, my_command->args[0]));
}

bool run_command(command* my_command, gamma_t** g_pointer, int line_number) {
    if (my_command->command_type == '#')
        return true; // commend is valid but no actions need to be taken
    if (my_command->command_type == 'B') {
        gamma_t* g = gamma_new(my_command->args[0], my_command->args[1],
                               my_command->args[2], my_command->args[3]);
        if (g == NULL)
            return false; // failed to allocate memory to create new game
        else {
            *g_pointer = g;
            printf("OK %d\n", line_number);
            return true;
        }
    }
    if (my_command->command_type == 'I') {
        gamma_t* g = gamma_new(my_command->args[0], my_command->args[1],
                               my_command->args[2], my_command->args[3]);
        if (g == NULL)
            return false; // failed to allocate memory to create new game
        else {
            *g_pointer = g;
            return true;
        }
    }
    if (my_command->command_type == 'p') {
        char* board = gamma_board(*g_pointer);
        if (board == NULL)
            return false; // failed to allocate memory to perform gamma_board
        else {
            printf("%s", board);
            free(board);
            return true;
        }
    } 
    else // other possible commands that don't allocate heap memory
        run_in_batch_mode(my_command, *g_pointer);
    return true;    
}