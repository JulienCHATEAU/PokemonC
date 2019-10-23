#include "typedef.h"
#include <stdbool.h>

#ifndef PNJ_H
#define PNJ_H

#define PNJS_PATH "data/maps_pnjs.txt"

void managePnjOrientation(Map *map, char player_orientation, int xy_ifo_player);
void pnjDialog(char *printable_map, char *dialog_box, Pnj *pnj);
bool findPnj(int x_map, int y_map, Pnj *pnj);
bool pnjWantsBattle(Map *map);
void handlePnjBattle(Map *map, Player *player);
void printPnj(Pnj *pnj);
bool pnjBattle(Map *map, Player *player);

#endif