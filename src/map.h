#include "typedef.h"
#include <stdbool.h>

#ifndef MAP_H
#define MAP_H

#define MAPS_ITEMS_PATH "data/maps_items.txt"

#define EXIT 1337
#define LONG_GRASS 'W'
#define STONE '0'
#define CUTABLE_TREE 'T'
#define FINAL_DOOR '-'
#define SHOP '$'
#define PILAR 'O'
#define DOOR_LEFT_PILAR '['
#define DOOR_RIGHT_PILAR ']'
#define DOOR '_'
#define MIDDLE_DOOR_POS 6
#define HEAL '#'
#define TREE '@'
#define POKEBALL 'o'
#define WATER_ '~'
#define MAP_SIZE 178
#define TO_PRINTABLE_MAP1 2
#define TO_PRINTABLE_MAP2 2
#define PRINTABLE_MAP_SIZE MAP_SIZE * 2 + 1
#define PLAYER_START_POS 156

#define PLAYER_N '^'
#define PLAYER_W '<'
#define PLAYER_S 'v'
#define PLAYER_E '>'

#define LINE_SEPARATOR 15

#define RESET 0
#define BRIGHT 1
#define DIM 2

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define WHITE 7

#define DB_YES_POS 179
#define DB_NO_POS 242

void waitForEnter();
void clearConsole();
int getXYIfoPlayer(Player *player);
void removeInteractiveObject(Map *map, Player *player, char *map_structure);
void loadMap(Map *map, char *map_structure, Player *player);
void createPrintableMap(char *printable_map, char *map_structure,
                        Player player);
void clearAndPrintMap(char *printable_map, char *dialog_box);
void printArray(char *array);
void movePlayer(Player *player, char new_pos, int xy_sub, Map *map);
void saveMapSpecificData(Player *player, int x_map, int y_map, int xy);
int checkIfInteractionPossible(Player *player, Map *map);
int manageKeyPressed(char key_pressed, Player *player, Map *map);
void manageDoorOpenning(Player *player, char *dialog_box, char *printable_map);
void comeBackFirstMap(Player *player, Map *map);
void blinkScreen(char *printable_map, char *dialog_box);
bool isMapSquareEqual(MapSquare ms1, MapSquare ms2);
void clearW8NErase(char *printable_map, char *dialog_box);

#endif