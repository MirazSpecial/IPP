/** @file
 * Gamma game main functions implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "borders.h"
#include "fau.h"
#include "gamma.h"

/** @brief Finds characters needed to fit the number.
 * Applies opperation: 1 + floor(log10(number))
 * @param[in] number - the number of player to put in string,
 * @return result the those operation.
 */
static uint32_t find_number_characters(uint32_t number) {
    uint32_t result = 0;
    while (number) {
        result++;
        number /= 10;
    }
    return result;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (width < 1 || height < 1 || players < 1 || areas < 1) 
        return NULL;

    player_t* players_array = calloc((uint64_t)players + 1, sizeof(player_t));
    uint64_t board_size = width * (uint64_t)height;
    field* board = calloc(board_size, sizeof(field));
    uint32_t* neighbours = calloc(4, sizeof(uint32_t));
    gamma_t* game = malloc(sizeof(gamma_t));

    if (players_array == NULL || board == NULL || neighbours == NULL ||
        game == NULL) { // could not allocate memory
        free(players_array);
        free(board);
        free(neighbours);
        free(game);
        return NULL;
    }

    game->width = width;
    game->height = height;
    game->players = players;
    game->areas = areas;
    game->free_fields = width * height;
    game->board = board;
    game->players_array = players_array;
    game->neighbours = neighbours;
    game->field_print_size = find_number_characters(game->players);
    return game;
} 

void gamma_delete(gamma_t *g) {
    if (g != NULL) {
        free(g->board);
        free(g->players_array);
        free(g->neighbours);
        free(g);
    }
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (g == NULL || player < 1 || g->players < player ||
        x >= g-> width || y >= g->height)
        return false; // incorrect parameter
    if (g->board[y * g->width + x].owner_number != 0)
        return false; // field already occupied

    if (!count_neighbours(g, player, x, y) &&
        g->players_array[player].used_areas == g->areas)
        return false; // new area while maximum areas is reached

    place(g, player, x, y);
    g->players_array[player].free_borders += add_new_borders(g, x, y);
    block_borders(g, x, y);
    g->players_array[player].used_fields++;
    g->free_fields--;
    return true;    
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (g == NULL || player < 1 || g->players < player ||
        x >= g-> width || y >= g->height)
        return false; // incorrect parameter
    if (g->players_array[player].used_golden)
        return false; // golden_move already performed

    uint64_t board_num = y * (uint64_t)g->width + x;
    uint32_t previous_owner = g->board[board_num].owner_number;
    if (previous_owner == 0 || previous_owner == player)
        return false; // field free or belongs to player
    delete_field(g, x, y);

    if (g->players_array[previous_owner].used_areas > g->areas) {
        // golden_move would create too many areas for previous_owner
        // so field is given back to previous_owner
        gamma_move(g, previous_owner, x, y);
        return false; 
    }
    if (gamma_move(g, player, x, y)) {
        // golden_move is possible so field is acquired by player
        g->players_array[player].used_golden = true;
        return true;
    }
    else {
        // maximum number of areas of player would be surpassed
        gamma_move(g, previous_owner, x, y);
        return false;
    }
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (g == NULL || player < 1 || g->players < player)
        return 0; // incorrect parameter
    else {
        player_t analysed_player = g->players_array[player];
        return analysed_player.used_fields;
    }
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if (g == NULL || player < 1 || g->players < player)
        return 0; // incorrect parameter
    else {
        player_t analysed_player = g->players_array[player];
        if (analysed_player.used_areas == g->areas)
            return analysed_player.free_borders;
        else
            return g->free_fields;
    }
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    if (g == NULL || player < 1 || g->players < player)
        return false; // incorrest parameter
    player_t analysed_player = g->players_array[player];
    if (analysed_player.used_golden)
        return false; // golden move already used
    
    bool field_found = false;
    // Now checking for every field if golden move can be performed on it
    for (uint32_t y = 0; y < g->height; y++) {
        for (uint32_t x = 0; x < g->width; x++) {
            uint64_t board_num = y * (uint64_t)g->width + x;
            uint32_t previous_owner = g->board[board_num].owner_number;
            if (previous_owner == 0 || previous_owner == player)
                continue; // field free or belongs to player

            delete_field(g, x, y);
            
            if (g->players_array[previous_owner].used_areas > g->areas || 
                (!count_neighbours(g, player, x, y) &&
                g->players_array[player].used_areas == g->areas)) {
                // golden_move on this field would create too many areas for 
                // 'player' or for 'previous_owner'
                field_found = false;
            }
            else
                field_found = true;
                
            gamma_move(g, previous_owner, x, y);
            if (field_found)
                return true;
        }
    }
    return false;
}

/** @brief Puts given number into a string at given position
 * The char array that is going to be created by gamma_board needs to 
 * numbers of players owning every field and some representation of
 * free fields (zeros). For evry number there is a certain number of chars.
 * @param[in] number       - the number of player to put in string,
 * @param[in] size         - number of chars to fill,
 * @param[in, out] buffor  - string to change ,
 * @param[in] number_begin - index of the beginning of number space in string. 
 */
static void put_in_string(uint32_t number, uint32_t size,
                          char* buffor, uint64_t number_begin) {
    if (number == 0) {
        buffor[number_begin + size - 1] = '.';
        for (int32_t i = size - 2; i >= 0; i--)
            buffor[number_begin + i] = (char)32;
        return; // one '.' for free field
    }
    for (int32_t i = size - 1; i >= 0; i--) {
        if (number != 0)
            buffor[number_begin + i] = '0' + (number % 10);
        else
            buffor[number_begin + i] = (char)32; // fill space with spaces
        number /= 10;
    }
}

/** @brief Generates gamma board for less then 10 players
 * Simplier version og gamma_board when g->players is less then 10
 * @param[in] g         - pointer to structure holding game status,
 * @param[in] buffor    - pointer to resulting string
 */
void few_players_board(gamma_t *g, char* buffor) {
    for (uint32_t line = 0; line < g->height; line++) {
        uint64_t line_begin = (g->height - 1 - line) * (uint64_t)(g->width + 1);
        for (uint32_t j = 0; j < g->width; j++) {
            put_in_string(g->board[line * (uint64_t)g->width + j].owner_number,
                          1, buffor, line_begin + j);
        }
        buffor[line_begin + g->width] = '\n';
    }
    buffor[(g->width + 1) * (uint64_t)g->height] = '\0';
}

char* gamma_board(gamma_t *g) {
    if (g == NULL) 
        return NULL;
    if (g->players < 10) {
        // characters needed for whole board
        uint64_t line_characters = (uint64_t)g->width + 1;
        uint64_t total_characters = line_characters * (uint64_t)g->height;
        char* buffor = malloc(sizeof(char) * total_characters + 1);
        if (buffor == NULL)
            return NULL; // failed to allocate memory
        else {
            few_players_board(g, buffor);
            return buffor;
        }
    }
    // characters needed for single number
    uint32_t number_characters = g->field_print_size;
    // characters needed for one line
    uint64_t line_characters = g->width * (uint64_t)(number_characters + 1);
    // characters needed for whole board
    uint64_t total_characters = line_characters * (uint64_t)g->height;
    char* buffor = malloc(sizeof(char) * total_characters  + 1);
    if (buffor == NULL)
        return NULL; // failed to allocate memory
    buffor[total_characters] = '\0';
    field *board = g->board;

    for (uint32_t line = 0; line < g->height; line++) {
        uint64_t line_begin = (g->height - 1 - line) * line_characters;
        for (uint32_t j = 0; j < g->width; j++) {
            uint64_t number_begin = line_begin + j * (number_characters + 1);
            uint32_t number = board[line * (uint64_t)g->width + j].owner_number;
            put_in_string(number, number_characters, buffor, number_begin);
            // character ' ' separates fields
            if (j < g->width - 1)
                buffor[line_begin + (j + 1) * (number_characters + 1) - 1] = ' ';
            else {
                buffor[line_begin + line_characters - 1] = '\n';
            }
        }
    }
    return buffor;
}  