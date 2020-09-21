/** @file
 * Interface of functions for adjacent free fields and project structures.
 * Functions for keeping track of number of free fields around every player.
 * Complexity of every function is O(1)
 */

#ifndef BORDERS_H
#define BORDERS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/** @brief Structure representing player.
 * Holds information about player's status in current moment.
 */
typedef struct player {
    bool used_golden; ///< if player already used golden move.
    uint32_t used_areas; ///< how many arreas does the player have.
    uint64_t free_borders; ///< free fields adjacent to player fields.
    uint64_t used_fields; ///< how many fields does the player have.
} player_t;

/** @brief Structure representing field on the board.
 * Holds information about field's status in current moment,
 * including its find-and-union dependents.
 */
typedef struct field {
    uint32_t owner_number; ///< number of field owner or 0 if field is free.
    int64_t representative; ///< representative field in find-and-union.
    uint64_t fields_in_area; ///< used only for area representant.
} field;

/** @brief Structure representing game status.
 * Holds board size, game restrictions, game status in current moment.
 */
typedef struct gamma {
    uint32_t width; ///< board width.
    uint32_t height; ///< board height.
    uint32_t players; ///< maximum number of players in this game.
    uint32_t areas; ///< macimum number of areas for a player.
    uint64_t free_fields; ///< fields not belonging to any player.
    field *board; ///< data of every field on board.
    uint32_t* neighbours; ///< supplementary array for players numbers.
    player_t *players_array; ///< data of every player.
    uint32_t field_print_size; ///< characters needed to print highest player.
} gamma_t;

/** @brief Counts number of digits in given number
 * @param[in] number    - number which digits to count
 * @return Number of those digits
 */
uint32_t count_digits(uint32_t number);

/** @brief Finds players with fields adjacent to given field.
 * Puts numbers of players with fields adjcent to given into g->neighbours.
 * @param[in, out] g - pointer to structure holding game status,
 * @param[in] x      - horizontal position on board,
 * @param[in] y      - vertical position on board.
 */
void find_distinct_neighbours(gamma_t* g, uint32_t x, uint32_t y);

/** @brief Count player's fields adjacent to given field.
 * @param[in] g      - pointer to structure holding game status,
 * @param[in] player - number of inspected player,
 * @param[in] x      - horizontal position on board,
 * @param[in] y      - vertical position on board.
 * @return Number of those players.
 */
uint32_t count_neighbours(gamma_t* g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Count new free fields adjacent for a player.
 * Counts free fields adjacent to a given one owned by a player, that are not
 * adjacent to any other field of this player.
 * @param[in] g      - pointer to structure holding game status,
 * @param[in] x      - horizontal position on board,
 * @param[in] y      - vertical position on board.
 * @return Number of those fields.
 */
uint32_t add_new_borders(gamma_t* g, uint32_t x, uint32_t y);

/** @brief Reduce number of free adjacent fields after aqcuireing this.
 * @param[in] g      - pointer to structure holding game status,
 * @param[in] x      - horizontal position on board,
 * @param[in] y      - vertical position on board.
 */
void block_borders(gamma_t* g, uint32_t x, uint32_t y);

/** @brief Increase number of free adjacent fields after freeing this.
 * @param[in] g      - pointer to structure holding game status,
 * @param[in] x      - horizontal position on board,
 * @param[in] y      - vertical position on board.
 */
void unblock_borders(gamma_t* g, uint32_t x, uint32_t y);

#endif /* BORDERS_H */