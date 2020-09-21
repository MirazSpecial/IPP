/** @file
 * Implementation for interactive mode operation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "borders.h"
#include "fau.h"
#include "gamma.h"
#include "inter_mode.h"

/** @brief Reprints given field.
 * After some field's status has changed it needs to be printed again
 * to update status on the screen, the field can have white background
 * with black letters (if cursor is there), black background with yellow letters
 * (if it is this field's owner turn) or black background with white letters
 * (default). If it's not known who's turn it is, 'active_player' should
 * be set to '0'.
 * @param[in] g             - pointer to structure holding game status,
 * @param[in] x             - horizontal position of field on board,
 * @param[in] y             - vertical position of field on board,
 * @param[in] light_up      - is the cursor on this field,
 * @param[in] active_player - number of a player whos turn is now.
 */
static void update_field(gamma_t* g, uint32_t x, uint32_t y, 
                         bool light_up, uint32_t active_player) {
    uint32_t field_size = g->field_print_size;
    uint64_t board_pos = y * (uint64_t)g->width + x;
    uint32_t player = g->board[board_pos].owner_number;
    if (light_up) {
        // if cursor is on this field it is printed diffrently
        printf("\x1b[47m"); // white background
        printf("\x1b[30m"); // black letters
    }
    else if (active_player != 0 && player == active_player) {
        // if field belongs to players whos turn it is it's printed diffrently
        printf("\x1b[33m"); // yellow letters
    }
    if (field_size == 1) {
        // printing boards with less then 10 players
        printf("\x1b[%d;%df", y + 1, x + 1);
        if (player == 0)
            printf(".");
        else
            printf("%u", player);
    }
    else {
        // printing boards with at least 10 players
        printf("\x1b[%d;%df", y + 1, x * (field_size + 1) + 1);
        int number_length = count_digits(player);
        for (uint32_t i = 0; i < field_size - number_length; i++)
            printf(" ");
        if (player == 0)
            printf(".");
        else
            printf("%u", player);
    }
    printf("\x1b[0m"); // color reset
}

/** @brief Updates fields of a given player.
 * Updates every field on the board which 'owner_number' is 'player', updates
 * fields diffrently if it is 'player''s turn now or not.
 * @param[in] g - Pointer to structure holding game status,
 * @param[in] player - Number of player to check,
 * @param[in] active - If this player's turn is now or if it has just ended.
 */
static void update_player_fields(gamma_t* g, uint32_t player, bool active) {
    for (uint32_t row = 0; row < g->height; row++) {
        for (uint32_t column = 0; column < g->width; column++) {
            uint64_t board_pos = row * (uint64_t)g->height + column;
            if (g->board[board_pos].owner_number == player) {
                if (active)
                    update_field(g, column, row, false, player);
                else
                    update_field(g, column, row, false, 0);
            }
        }
    }
}

/** @brief Prints game board
 * @param[in] g - Pointer to struct holding game status.
 */
static void set_board(gamma_t* g) {
    printf("\x1b[2J");    // Clear the entire screen
    printf("\x1b[%d;%df", 1, 1); // set cursor
    for (uint32_t row = 0; row < g->height; row++)
        for (uint32_t column = 0; column < g->width; column++)
            update_field(g, column, row, false, 0);
}

/** @brief Updates cursor position after pressing arrow.
 * Changes cursor position (if it is possible) updating
 * values '*x_ptr' and '*y_ptr' for arrow specified by
 * move_type. 
 * @param[in] g         - pointer to structure holding game status,
 * @param[in] x_ptr     - pointer to integer holding horizontal cursor position,
 * @param[in] y_ptr     - pointer to integer holding vertical cursor position,
 * @param[in] move_type - last number in ANSI escape code of pressed arrow,
 * @param[in] player    - number of player who's move this is.
 */
static void change_position(gamma_t* g, uint32_t* x_ptr, 
                     uint32_t* y_ptr, int move_type, uint32_t player) {
    uint32_t x = *x_ptr;
    uint32_t y = *y_ptr;
    if (move_type == 65 && y > 0) {
        // arrow up
        update_field(g, x, y, false, player);
        (*y_ptr)--;
        update_field(g, x, --y, true, player);
    }
    if (move_type == 66 && y < g->height - 1) {
        // arrow down
        update_field(g, x, y, false, player);
        (*y_ptr)++;
        update_field(g, x, ++y, true, player);
    }
    if (move_type == 67 && x < g->width - 1) {
        // arrow right
        update_field(g, x, y, false, player);
        (*x_ptr)++;
        update_field(g, ++x, y, true, player);
    }
    if (move_type == 68 && x > 0) {
        // arrow left
        update_field(g, x, y, false, player);
        (*x_ptr)--;
        update_field(g, --x, y, true, player);
    }
}

/** @brief Prints information about given player below the board.
 * Prints player number, and results of functions
 * gamma_busy_fields, gamma_free_fields, gamma_golden_possible for them.
 * @param[in] g         - Pointer to structure holding game status,
 * @param[in] player    - Player to describe.
 */
static void print_player_description(gamma_t* g, uint32_t player) {
    printf("\x1b[%d;%df", g->height + 1, 1); // Set cursor below board
    printf("\x1b[2K");    // Clear cursor line 

    printf("PLAYER");
    printf("\x1b[33m %u \x1b[0m", player); // yellow letters
    
    printf("POINTS");
    printf("\x1b[32m %lu \x1b[0m", gamma_busy_fields(g, player));

    printf("POSSIBLE MOVES");
    printf("\x1b[31m %lu \x1b[0m", gamma_free_fields(g, player));

    if (gamma_golden_possible(g, player))
        printf("POSSIBLE GOLDEN");
    printf("\n");
}

/** @brief Finds number of next player which can maka a move in given game.
 * For given player checks who can make a move after him and returns his
 * number or 0 if there are no moves possible.
 * @param[in] g         - Pointer to structure holding game status,
 * @param[in] player    - Player that have just moved.
 * @return Returns number of player of 0 if there are no moves possible.
 */
static uint32_t next_player(gamma_t* g, uint32_t player) {
    uint32_t new_player = player;
    uint32_t passed = 0;
    while (passed < g->players) {
        passed++;
        new_player++;
        if (new_player > g->players)
            new_player = 1;
        if (gamma_free_fields(g, new_player) > 0 || 
            gamma_golden_possible(g, new_player)) {
            update_player_fields(g, player, false);
            update_player_fields(g, new_player, true);
            return new_player; // found somebody who can move
        }
    }
    update_player_fields(g, player, false);
    return 0; // no moves possible
}

/** @brief Prints information about all players after game ends.
 * Prints description of every players taken fields below the board.
 * @param[in] g - Pointer to structure holding game status. 
 */
static void summary(gamma_t* g) {
    printf("\x1b[%d;%df", g->height + 1, 1);
    printf("\x1b[2K"); // clears line after board
    for (uint32_t i = 1; i <= g->players; i++) {
        printf("PLAYER");
        printf("\x1b[33m %u \x1b[0m", i);
        printf("POINTS");
        printf("\x1b[32m %lu \x1b[0m", gamma_busy_fields(g, i));
        printf("\n");
    }
}

/** @brief Prints board and prepers proper key reading.
 * Enables input readin without enter key by changing to raw mode, 
 * prints board, prints player description, hides cursor.
 * @param[in] g             - Pointer to structure holding game status,
 * @param[in] termios_ptr   - Pointer to structure with terminal settings.
 */
static void prepare_board(gamma_t* g, struct termios* termios_ptr) {
    // get terminal settings
    if (tcgetattr(STDIN_FILENO, termios_ptr) == -1)
        exit(1); // error while reading terminal settings
    struct termios raw = *termios_ptr;
    raw.c_lflag &= ~(ECHO | ICANON); // change terminal settings
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        exit(1); // error while changing terminal settings
  
    set_board(g);
    printf("\x1b[?25l"); // hide the cursor
    print_player_description(g, 1);
}

/** @brief Disables earlier key reading and prints summary.
 * Disables reading input without enter key, prints summary and shows cursor.
 * @param[in] g             - Pointer to structure holding game status,
 * @param[in] termios_ptr   - Pointer to structure with terminal settings.
 */
static void finish_interactive(gamma_t* g, struct termios* termios_ptr) {
    // disable reading input without need for enter key
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, termios_ptr) == -1)
        exit(1); // error while changing terminal settings

    printf("\x1b[?25h"); // show the cursor
    summary(g);
}

/** @brief Checks if terminal window is large enough to perform this game.
 * @param[in] g -   Pointer to structure holding game status.
 */
static bool proper_terminal_size(gamma_t* g) {
    struct winsize terminal_window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_window);
    uint32_t terminal_height = terminal_window.ws_row;
    uint32_t terminal_width = terminal_window.ws_col;

    // Check if terminal can show all rows plus one description row
    if (terminal_height <= g->height + 1)
        return false;

    // Check if terminal can show all columns
    if (g->players < 10) { // for less then 10 players
        if (terminal_width < g->width)
            return false;
    }
    else { // for at leas 10 players (so including spaces between fields)
        if ((uint64_t)terminal_width < g->width * ((uint64_t)g->field_print_size + 1))
            return false;
    }

    // Check if terminal can show description line
    const uint32_t MIN_WIDTH = 55;
    if (terminal_width < MIN_WIDTH)
        return false;

    return true;
}

void run_interactive(gamma_t* g) {
    if (!proper_terminal_size(g)) {
        printf("Terminal not large enough!\n");
        return;
    }
    struct termios original_termios;
    prepare_board(g, &original_termios);
    uint32_t x = g->width / 2;
    uint32_t y = g->height / 2;
    int player = 1;
    update_field(g, x, y, true, 0);
    // now board and first players description are printed
    // starts listening for pressed keys

    const int READY_TO_READ = 1000;
    int input = READY_TO_READ;
    // Every place where 'input' is set to 'READY_TO_READ' means that some 
    // pressed key was processed properly and program is listening to next key.
    // When 'input' is set to something else that means there was
    // some unfinished escape sequence so program will process parts of it
    while (true) {
        if (input == READY_TO_READ)
            input = getchar();

        if (input == 4) { // ctrl + D 
            update_player_fields(g, player, false);
            update_field(g, x, y, false, 0);
            break; // ctrl + D pressed game ends
        }
        else if (input == ' ') { // SPACE
            if (gamma_move(g, player, x, y)) {
                player = next_player(g, player);
                update_field(g, x, y, true, player);
            }
            input = READY_TO_READ;
        }
        else if (input == 'G' || input == 'g') { // g or G
            if (gamma_golden_move(g, player, x, y)) {
                player = next_player(g, player);
                update_field(g, x, y, true, player);
            }
            input = READY_TO_READ;
        }
        else if (input == 'C' || input == 'c') { 
            player = next_player(g, player);
            update_field(g, x, y, true, player);
            input = READY_TO_READ;
        }
        else if (input == 27) { // ESC so some escape code starts
            int second_input = getchar();
            if (second_input == '[') { // ] so code continoues
                int move_type = getchar();
                if (move_type >= 65 && move_type <= 68) { // proper arror code
                    change_position(g, &x, &y, move_type, player);
                    input = READY_TO_READ; 
                }
                else // not finished escape sequence ESC + ?
                    input = move_type;
            }
            else // not finised escape sequence ESC + ] + ?
                input = second_input;
        }
        else // key not supported
            input = READY_TO_READ;
        if (player == 0) {
            update_field(g, x, y, false, 0);
            break; // function find_next_player didn't find player that can move
        }
        print_player_description(g, player);
    }
    // game finished 
    finish_interactive(g, &original_termios);  
}