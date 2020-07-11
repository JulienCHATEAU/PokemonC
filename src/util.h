#include "typedef.h"
#include <stdbool.h>

#ifndef UTIL_H
#define UTIL_H

#define ENTER 10
#define DELETE 127

#define MOVE_Z 'z'
#define MOVE_Q 'q'
#define MOVE_S 's'
#define MOVE_D 'd'

#define CLASSIC_COLOR 1
#define POKEMON_PANE_COLOR 2
#define TREE_COLOR 3
#define WATER_COLOR 4
#define GRASS_COLOR 5
#define POKEBALL_COLOR 6
#define HEAL_COLOR 7
#define CUTABLE_TREE_COLOR 8
#define DOOR_COLOR 9
#define STONE_COLOR 10
#define PILLAR_COLOR 11

void waitNMs(int n); /*n millisecond*/
bool isEqual(char character, char comp);
void textColor(int attr, int fg);
void addArrow(int arrow_xy, char *battle_pane);
void removeArrow(int arrow_xy, char *battle_pane);
void shiftMenuPipes(char *battle_pane, int top_pipe_xy_to_add,
                    int top_pipe_xy_to_erase, int next_line, int pipe_count);
void addStringToArray(char *string, int string_length, int start, char *array);
bool contains(int *tab, int count, int elem);
void setStringToTab(char *tab, char *string);
void freeSkill(Skill skill);
void freePlayer(Player player);
void freePokemon(Pokemon pkmn);
Effect createEffect(int stat_to_change, int change_value, int accuracy);
Stats createStats(int hp_max, int att_max, int def_max, int spd_max);
void setPlayerPseudo(char *pseudo, Player *player);
Player createPlayer(char *pseudo, int xy, char pos, char *pkmn_name);
void copyEffect(Effect effect, Effect *copy);
void copySkill(Skill skill, Skill *copy);
bool isItemUsable(int mode, int usable_time);
void getSpacesString(char *string, int length);
void enterKey();
void addPickedItemPlayer(Player *player, MapSquare square);
bool isVertical(int xy, int xy2);
bool isHorizontal(int xy, int xy2);
void copyMapSquare(MapSquare tmp, MapSquare *item);
bool containsPickedItem(Player *player, MapSquare picked_item, MapSquare *item);
int getColumn(int xy);
int getLine(int xy);
void getUserInput(char *key_pressed);
void initColors();
void setColor(int colorId);

#endif