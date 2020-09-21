/** @file
 * Functions for dealing with fields owners on find-and-union level.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "borders.h"
#include "fau.h"

/** @brief Finds the main representative in the area in find-and-union.
 * Recursive function with complexity O(log n) where n is the number
 * of fields in the area. 
 * @param[in] board        - pointer to an array of fields,
 * @param[in] field_number - number of an inspected field on a board.
 * @return Number of the field representing given area in find-and-union
 */
static uint64_t main_representative(field* board, uint64_t field_number) {
    uint64_t representative_number = board[field_number].representative;
    if (representative_number == field_number)
        return field_number;
    else
        return(main_representative(board, representative_number));
}

/** @brief Join two areas in find-and-union report if succesful.
 * Finds main representatives of both areas then joins them if possible.
 * Complexity O(log n) where n stands for number of fields in joined areas.
 * @param[in] board  - pointer to an array of fields,
 * @param[in] field1 - number of the first field on the board,
 * @param[in] field2 - number of the second field on the board.
 * @return True if two areas where succesfully joind.
 */
static bool join_areas(field* board, uint64_t field1, uint64_t field2) {
    if (board[field1].owner_number == 0 || board[field2].owner_number == 0)
        return false; // free fields
    if (board[field1].owner_number != board[field2].owner_number)
        return false; // diffrent owners
    field1 = main_representative(board, field1);
    field2 = main_representative(board, field2);
    if (field1 == field2)
        return false; // already in the same area

    if (board[field1].fields_in_area > board[field2].fields_in_area) {
        board[field2].representative = field1;
        board[field1].fields_in_area += board[field2].fields_in_area;
    }
    else {
        board[field1].representative = field2;
        board[field2].fields_in_area += board[field1].fields_in_area;
    }
    return true;
}

/** @brief Sets field when acquired by a player.
 * Changes field's owner and sets default data neede by find-and-union,
 * Complexity O(1).
 * @param[in] board     - pointer to an array of fields,
 * @param[in] board_num - number of the field on the board,
 * @param[in] player    - number of the player acquiring the field.
 */
static void new_field(field* board, uint64_t board_num, uint32_t player) {
    board[board_num].owner_number = player;
    board[board_num].representative = board_num;
    board[board_num].fields_in_area = 1;

}

void place(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    field* board = g->board;
    uint64_t board_num = y * (uint64_t)g->width + x;
    new_field(board, board_num, player);
    uint32_t reduced_areas = 0;
    
    // joins with evry adjacent field  
    if (x > 0)
        reduced_areas += join_areas(board, board_num, board_num - 1);
    if (x < g->width - 1)
        reduced_areas += join_areas(board, board_num, board_num + 1);
    if (y > 0)
        reduced_areas += join_areas(board, board_num, board_num - g->width);
    if (y < g->height - 1)
        reduced_areas += join_areas(board, board_num, board_num + g->width);
    g->players_array[player].used_areas += (1 - reduced_areas);
}

/** @brief Changes representative in whole area to given value.
 * Recursive dfs function that goes throw whole area, assuming that whole
 * find and union set was disjoind and needs to be joind again.
 * Complexity O(n) where n stands for number of fields in the area.
 * @param[in, out] g - pointer to structure holding game status,
 * @param[in] x      - horizontal position on board,
 * @param[in] y      - vertical position on board,
 * @param[in] value  - value set as representative in evry area field
 */
static void set_representative(gamma_t* g, uint32_t x,
                               uint32_t y, int64_t value) {
    // dfs function for setting fields representative in find-and-union to
    // given one (-1 if just dijoining sets)
    uint64_t board_num = y * (uint64_t)g->width + x;
    field* board = g->board;
    uint32_t my_owner = board[board_num].owner_number;
    board[board_num].representative = value;
    if (value >= 0)
        board[value].fields_in_area++;

    // dfs for every adjacent field with the same owner
    if (x > 0 && board[board_num - 1].owner_number == my_owner &&
        board[board_num - 1].representative != value)
        set_representative(g, x - 1, y, value);
    if (x < g->width - 1 && board[board_num + 1].owner_number == my_owner &&
        board[board_num + 1].representative != value)
        set_representative(g, x + 1, y, value);
    if (y > 0 && board[board_num - g->width].owner_number == my_owner &&
        board[board_num - g->width].representative != value)
        set_representative(g, x, y - 1, value);
    if (y < g->height - 1 && 
        board[board_num + g->width].owner_number == my_owner &&
        board[board_num + g->width].representative != value)
        set_representative(g, x, y + 1, value);
}

void delete_field(gamma_t* g, uint32_t x, uint32_t y) {
    uint64_t board_num = y * (uint64_t)g->width + x;
    field* board = g->board;
    uint32_t previous_owner = board[board_num].owner_number;
    g->free_fields++;
    g->players_array[previous_owner].used_fields--;
    g->players_array[previous_owner].free_borders -= add_new_borders(g, x, y);
    unblock_borders(g, x, y);

    board[board_num].owner_number = 0;

    // disjoin all areas adjacent to given field
    if (x > 0 && board[board_num - 1].owner_number == previous_owner) {
        set_representative(g, x - 1, y, -1);
    }
    if (x < g->width - 1 && 
        board[board_num + 1].owner_number == previous_owner) {
        set_representative(g, x + 1, y, -1);
    }
    if (y > 0 && board[board_num - g->width].owner_number == previous_owner) {
        set_representative(g, x, y - 1, -1);
    }
    if (y < g->height - 1 && 
        board[board_num + g->width].owner_number == previous_owner) {
        set_representative(g, x, y + 1, -1);
    } 
    g->players_array[previous_owner].used_areas--;

    // after freeing given field the number of areas of previous_owner
    // has changed, check how many new are there and joins areas again
    if (x > 0 && board[board_num - 1].owner_number == previous_owner &&
        board[board_num - 1].representative == -1) {
        set_representative(g, x - 1, y, board_num - 1);
        g->players_array[previous_owner].used_areas += 1;
    }
    if (x < g->width - 1
        && board[board_num + 1].owner_number == previous_owner &&
        board[board_num + 1].representative == -1) {
        set_representative(g, x + 1, y, board_num + 1);
        g->players_array[previous_owner].used_areas += 1;
    }
    if (y > 0 && board[board_num - g->width].owner_number == previous_owner &&
        board[board_num - g->width].representative == -1) {
        set_representative(g, x, y - 1, board_num - g->width);
        g->players_array[previous_owner].used_areas += 1;
    }
    if (y < g->height - 1 && 
        board[board_num + g->width].owner_number == previous_owner &&
        board[board_num + g->width].representative == -1) {
        set_representative(g, x, y + 1, board_num + g->width);
        g->players_array[previous_owner].used_areas += 1;
    }
}