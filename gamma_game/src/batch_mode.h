/** @file
 * Interface for batch mode operations including
 * parsing input lines and finding commands and running those commandss
 */

#ifndef BATCH_MODE_H
#define BATCH_MODE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/** @brief Structure representing potentially valid command in batch mode.
 * Input represents input line that can be interpreted as single char 
 * and up to four integers or is a commant.
 * comment is a valid command too.
 * command_type can be '#' (for empty lines or commants), 'B', 'I',
 * 'm', 'g', 'b', 'f', 'q', 'p'. 
 */
typedef struct command {
    char command_type; ///< what action command represents (# if comment).
    uint32_t* args; ///< up to 4 integer parameters.
    int arguments_number; ///< number of parameters
} command;

/** @brief Checks if single line can be interpreted as valid command
 * Checks if line is empty or starts with '#' what means it is a comment
 * If line can be interpreted as some command checks if this command
 * is valid, so if number of parameters suits command_type.
 * If line can be interpreted as valid command, sets command values
 * at pointed command (my_command).
 * Diffrent commands can be valid depending on activation of batch_mode.
 * @param[in, out] input_line   - inputed string to parse,
 * @param[in, out] my_command   - pointed to a command that will be modified,
 * @param[in] batch_mode        - if batch mode is active.
 * @return Returns true if line can be interpreted as valid command,
 *         returns false otherwise
 */
bool parse_line(char* input_line, command* my_command, bool batch_mode);

/** @brief Performs gamma function represented in command reports if succesful
 * For given command runs one of gamma functions (including gamma_new)
 * If commend means creating batch_mode game or interactive_mode game
 * and game is created succesfully modifies pointer *g_pointer, 
 * (in batch_mode game also prints OK line).
 * @param[in] my_command    - given command,
 * @param[in] g_pointer     - pointer to gamma_t pointer used in gamma_main,
 * @param[in] line_number   - number of input line where command is given. 
 * @return Reports if commend was succesfully performed
 */
bool run_command(command* my_command, gamma_t** g_pointer, int line_number);

#endif /* BATCH_MODE_H */
