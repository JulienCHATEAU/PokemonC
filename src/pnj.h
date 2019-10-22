#include "typedef.h"
#include <stdbool.h>

#ifndef PNJ_H
#define PNJ_H

#define PNJS_PATH "data/maps_pnjs.txt"

void managePnjOrientation(char *printable_map, char player_orientation,
                          int xy_ifo_player);
void pnjDialog(char *printable_map, char *dialog_box, Pnj *pnj);
bool findPnj(MapSquare pnj_pos, Pnj *pnj);
void printPnj(Pnj *pnj);

#endif