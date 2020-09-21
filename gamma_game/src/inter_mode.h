/** @file
 * Interface for interactive mode operation
 */

#ifndef INTER_MODE_H
#define INTER_MODE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/** @brief Prints gamma_board, listens to key pressing and acts accordingly.
 * Prints board on screen and for any proper action updates board accordingly
 * and prints necessary changes. After game is finished (Ctrl+D or no moves
 * possible) prints summery below board.
 * @param[in, out] g    - Pointer to structure holding game status.
 */
void run_interactive(gamma_t* g);

#endif /* INTER_MODE_H */
