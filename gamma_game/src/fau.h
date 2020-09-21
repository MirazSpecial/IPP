/** @file
 * Interface for functions for dealing with fields owners on 
 * find-and-union level.
 */

#ifndef FAU_H
#define FAU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/** @brief Change given field's owner to player
 * Changes field's owner and if necessary joins areas in find-and-union.
 * Complexity O(log n) where n stands for number of fields in joined areas.
 * @param[in] g      - pointer to structure holding game status,
 * @param[in] player - number of a new owner,
 * @param[in] x      - horizontal position on board,
 * @param[in] y      - vertical position on board.
 */
void place(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Frees given field
 * Frees given field and if necessary disjoins areas in find-and-union.
 * Complexity O(n) where n stands for number of fields in disjoined areas. 
 * @param[in] g      - pointer to structure holding game status,
 * @param[in] x      - horizontal position on board,
 * @param[in] y      - vertical position on board.
 */
void delete_field(gamma_t* g, uint32_t x, uint32_t y);

#endif /* FAU_H */