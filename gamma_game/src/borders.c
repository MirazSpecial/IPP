/** @file
 * Implementation of functions for adjacent free fields.
 * Functions for keeping track of number of free fields around every player.
 * Complexity of every function is O(1)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "borders.h"

uint32_t count_digits(uint32_t number) {
    if (number == 0)
        return 1;
    uint32_t result = 0;
    while(number != 0) {
        result++;
        number /= 10;
    }
    return result;
}

void find_distinct_neighbours(gamma_t* g, uint32_t x, uint32_t y) {
    uint32_t neighbours[4] = {0, 0, 0, 0}; 
    uint64_t board_num = y * (uint64_t)g->width + x;

    // finds neighbours numbers
    if (x > 0) 
        neighbours[0] = g->board[board_num - 1].owner_number;
    if (x < g->width - 1)
        neighbours[1] = g->board[board_num + 1].owner_number;
    if (y > 0)
        neighbours[2] = g->board[board_num - g->width].owner_number;
    if (y < g->height - 1)
        neighbours[3] = g->board[board_num + g->width].owner_number;
    int distinct_neighbours = 0;

    // takes each neighbour only once
    for (int i = 0; i < 4; i++) {
        g->neighbours[i] = 0;
        if (neighbours[i] != 0) {
            bool distinct = true;
            for (int j = 0; j < i; j++)
                if (neighbours[i] == neighbours[j])
                    distinct = false;
            if (distinct) {
                g->neighbours[distinct_neighbours] = neighbours[i];
                distinct_neighbours++;
            }
        }
    }
}

uint32_t count_neighbours(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    field* board = g->board;
    uint64_t board_num = y * (uint64_t)g->width + x;
    uint32_t result = 0;

    // for every adjacent field check if player is it's owner
    if (x > 0 && board[board_num - 1].owner_number == player)
        result++;
    if (x < g->width - 1 && board[board_num + 1].owner_number == player)
        result++;
    if (y > 0 && board[board_num - g->width].owner_number == player)
        result++;
    if (y < g->height - 1 && board[board_num + g->width].owner_number == player)
        result++;
    return result;
}

uint32_t add_new_borders(gamma_t* g, uint32_t x, uint32_t y) {
    field* board = g->board;
    uint64_t board_num = y * (uint64_t)g->width + x;
    uint32_t player = board[board_num].owner_number;
    uint32_t result = 0;
    
    // for every adjecent field check if it is a new free adjacent field
    // or if it was counter already
    if (x > 0 && board[board_num - 1].owner_number == 0 &&
        2 > count_neighbours(g, player, x - 1, y))
        result++;
    if (x < g->width - 1 && board[board_num + 1].owner_number == 0 &&
        2 > count_neighbours(g, player, x + 1, y))
        result++;
    if (y > 0 && board[board_num - g->width].owner_number == 0 &&
        2 > count_neighbours(g, player, x, y - 1))
        result++;
    if (y < g->height - 1 && board[board_num + g->width].owner_number == 0 &&
        2 > count_neighbours(g, player, x, y + 1))
        result++;
    return result;
}

void block_borders(gamma_t* g, uint32_t x, uint32_t y) {
    find_distinct_neighbours(g, x, y);
    for (int i = 0; i < 4; i++) {
        if (g->neighbours[i] != 0)
            g->players_array[g->neighbours[i]].free_borders--;
    }
}

void unblock_borders(gamma_t* g, uint32_t x, uint32_t y) {
    find_distinct_neighbours(g, x, y);
    for (int i = 0; i < 4; i++) {
        if (g->neighbours[i] != 0)
            g->players_array[g->neighbours[i]].free_borders++;
    }
}