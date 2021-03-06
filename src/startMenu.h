#ifndef STARTMENU_H
#define STARTMENU_H

#include "typedef.h"

#define SM_ARROW_LEFT_SHIFT 4
#define SM_FRST_PIPE_POS 90
#define SM_NEXT_LINE 63

#define SM_POKEMONS "POKEMONS"
#define SM_SAC "SAC"
#define SM_QUITTER "QUITTER"
#define SM_ANNULER "ANNULER"

#define SM_POKEMONS_LGTH 8
#define SM_SAC_LGTH 3
#define SM_QUITTER_LGTH 7
#define SM_ANNULER_LGTH 7

#define SM_POKEMONS_POS 161
#define SM_SAC_POS 176
#define SM_QUITTER_POS 287
#define SM_ANNULER_POS 302

#define POKEBALL_PRICE 2500
#define POTION_PRICE 1000
#define TOTAL_SOIN_PRICE 2500
#define X_KEY_PRICE 10000

#define SM_POKEMONS_ID 0
#define SM_SAC_ID 1
#define SM_QUITTER_ID 2
#define SM_ANNULER_ID 3

void createStartMenu(char *dialog_box);
int manageStartMenu(Player *player, char *printable_map, char *dialog_box);
int changeTargettedMenu(int *targetted_menu, int int1, int int2, int int3,
                        int int4);
void eraseSmPipes(char *dialog_box);
int manageSmEnterKeyPressed(Player *player, int *targetted_menu,
                            char *dialog_box);
int manageSmKeyPressed(Player *player, char key_pressed, int *targetted_menu,
                       int arrows_xys[], char *dialog_box);
void printPokemonsPane(Player *player, int targetted_pkmn, int mode);
int managePokemonsMenu(Player *player, int mode, int *targetted_pkmn);
int managePmKeyPressed(Player *player, char key_pressed, int *targetted_pkmn,
                       int mode);
void printBagPane(Player *player, int targetted_item, int mode);
int manageBagMenuKeyPressed(char key_pressed, int *targetted_item,
                            Player *player, int mode);
int manageBagMenu(Player *player, int mode);
void removeItem(Player *player, int item_index, int count);
int manageShopMenu(Player *player);

#endif