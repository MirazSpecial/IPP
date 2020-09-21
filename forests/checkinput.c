#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bst.h"
#include "work.h"
#include "checkinput.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static bool char_error(char* word) {
    // checks if word consists of proper ascii 
    int word_length = strlen(word);
    for (int i = 0; i < word_length; i++)
        if (0 <= (int)word[i] && (int)word[i] < 33)
            return true;
    return false;
}

static bool command_error(char* first_word, int word_count) {
    // checks if first word is proper command name and 
    // if command has enough arguments
    if ((!strcmp(first_word, "ADD") && word_count > 0 && word_count < 4) || 
        (!strcmp(first_word, "DEL") && word_count < 4) ||
        (!strcmp(first_word, "PRINT") && word_count < 3) || 
        (!strcmp(first_word, "CHECK") && word_count > 0 && word_count < 4))
        return false;
    else 
        return true;
}


bool check_input(char* input_line, command* command_pointer) {
    if (input_line[0] == '#') return false; // comment
    bool end_of_file = false;
    if (input_line[(int)strlen(input_line) - 1] != '\n')
        end_of_file = true;
    const char whitespace[] = " \t\v\f\r\n";

    // get the first word from input_line
    char* first_word = strtok(input_line, whitespace);
    if (first_word == NULL) return false; // empty line
    if (end_of_file) {
        fprintf(stderr, "ERROR\n");
        return false; // non-empty line not ended with '\n'
    }
    
    int word_count = 0;
    char* words[3];

    // devide rest of the input_line into words
    char* one_word = strtok(NULL, whitespace);
    while (one_word != NULL) { 
        words[MIN(word_count, 2)] = one_word;
        word_count++;
        one_word = strtok(NULL, whitespace);
    }


    if (command_error(first_word, word_count)) {
        fprintf(stderr, "ERROR\n");
        return false; // command error
    }
    for (int i = 0; i < word_count; i++) {
        if (char_error(words[i])) {
            fprintf(stderr, "ERROR\n");
            return false; // char outside of ascii
        }
    }
    command_pointer->oprtype = first_word[0]; 
    // command code is its first letter
    
    if (command_pointer->oprtype == 'C' 
        && !strcmp(words[word_count - 1], "*")) {
        fprintf(stderr, "ERROR\n");
        return false; // CHECK last argument is *
    }
    for (int i = 0; i < word_count; i++)
        command_pointer->words[i] = words[i];
    return true;
}