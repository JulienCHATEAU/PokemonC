#include "typedef.h"

#ifndef MAP_H
#define MAP_H

#define EXIT 1337
#define LONG_GRASS 'W'
#define HEAL '#'
#define TREE '@'
#define POKEBALL 'o'
#define MAP_SIZE 178
#define PRINTABLE_MAP_SIZE 360

#define PLAYER_N '^'
#define PLAYER_W '<'
#define PLAYER_S 'v'
#define PLAYER_E '>'

#define LINE_SEPARATOR 15


void waitForEnter();
void clearConsole();
int getXYIfoPlayer(Player *player);
void removeInteractiveObject(int x_map, int y_map, Player *player, char *map_structure);
void loadMap(int x_map, int y_map, char *map_structure, Player *player);
void createPrintableMap(char *printable_map, char *map_structure, Player player);
void clearAndPrintMap(char *printable_map, char *dialog_box);
void movePlayer(Player *player, char new_pos, int xy_sub, char *printable_map, char *dialog_box);
void changeMap(Player *player, int *x_map, int *y_map, int x_map_sub, int y_map_sub, char new_pos, int xy_player_sub, char *printable_map, char *map_structure);
void saveMapSpecificData(Player *player, int x_map, int y_map, int xy);
int checkIfInteractionPossible(Player *player, char *printable_map, char *dialog_box, int *x_map, int *y_map);
int manageKeyPressed(char key_pressed, Player *player, char *dialog_box, char *printable_map, int *x_map, int *y_map);

#endif