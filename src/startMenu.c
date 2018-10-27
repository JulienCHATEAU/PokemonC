#include <stdio.h>
#include "print.h"
#include "startMenu.h"
#include "ttyraw.h"
#include "pokemon.h"
#include "util.h"
#include "map.h"

/**************/
/* START MENU */
/**************/

/* Creates the start menu (it's situated in the dialog box)
* dialog_box : the dialog box
*/
void createStartMenu(char *dialog_box) {
  shiftMenuPipes(dialog_box, SM_FRST_PIPE_POS, SM_FRST_PIPE_POS, SM_NEXT_LINE, DIALOG_BOX_LINE_COUNT-1);
  addStringToArray(SM_POKEMONS, SM_POKEMONS_LGTH, SM_POKEMONS_POS, dialog_box);
  addStringToArray(SM_SAC, SM_SAC_LGTH, SM_SAC_POS, dialog_box);
  addStringToArray(SM_QUITTER, SM_QUITTER_LGTH, SM_QUITTER_POS, dialog_box);
  addStringToArray(SM_ANNULER, SM_ANNULER_LGTH, SM_ANNULER_POS, dialog_box);
  addArrow(SM_POKEMONS_POS-SM_ARROW_LEFT_SHIFT, dialog_box);
}

/* Manages the start menu working when user triggers it
* printable_map : the printable map
* dialog_box : the dialog box
*/
int manageStartMenu(Player *player, char *printable_map, char *dialog_box) {
  int targetted_menu = 0;
  int arrows_xys[4] = {SM_POKEMONS_POS-SM_ARROW_LEFT_SHIFT, SM_SAC_POS-SM_ARROW_LEFT_SHIFT, SM_QUITTER_POS-SM_ARROW_LEFT_SHIFT, SM_ANNULER_POS-SM_ARROW_LEFT_SHIFT};
  char key_pressed = 0;
  int key_pressed_status = 0;
  do {
    key_pressed = getchar();
    key_pressed_status = manageSmKeyPressed(player, key_pressed, &targetted_menu, arrows_xys, dialog_box);
    if (key_pressed_status != 2) {
      clearAndPrintMap(printable_map, dialog_box);
    }
  } while(key_pressed_status != 1 && key_pressed_status != 1337);
  return key_pressed_status;
}

/* Manaages the enter key pressed
* targetted_menu : an integer describing which menu in the start menu is targetted
* dialog_box : the dialog box
*/
int manageSmEnterKeyPressed(Player *player, int *targetted_menu, char *dialog_box) {
  int key_pressed_status = 0;
  if (*targetted_menu == SM_POKEMONS_ID) {
    managePokemonsMenu(player, 0);
  } else if (*targetted_menu == SM_SAC_ID) {

  } else if (*targetted_menu == SM_QUITTER_ID) {
    key_pressed_status = 1337;
  } else if (*targetted_menu == SM_ANNULER_ID) {
    key_pressed_status = 1;//exit start menu
    eraseDialogBoxLines(dialog_box);
    eraseSmPipes(dialog_box);
  }
  return key_pressed_status;
}

/* Changes the targetted menu
* targetted_menu : an integer describing which menu in the start menu is targetted
// each menu is identifed by an integer
* if_this_menu :
* then_this_one :
* else_if_this_menu :
* then_that_one :
* return 0 if an arrow was moved, 2 otherwise
*/
int changeTargettedMenu(int *targetted_menu, int if_this_menu, int then_this_one, int else_if_this_menu, int then_that_one) {
  int key_pressed_status = 0;
  if (*targetted_menu == if_this_menu) {
    *targetted_menu = then_this_one;
  } else if (*targetted_menu == else_if_this_menu) {
    *targetted_menu = then_that_one;
  } else {
    key_pressed_status = 2;
  }
  return key_pressed_status;
}

/* Erases the start menu pipes
* dialog_box : the dialog box
*/
void eraseSmPipes(char *dialog_box) {
  dialog_box[SM_FRST_PIPE_POS] = ' ';
  dialog_box[SM_FRST_PIPE_POS + (DIALOG_BOX_LINE_COUNT-2)*SM_NEXT_LINE] = '_';
}

/* Manages the key pressed in the start menu
* key_pressed : the key pressed
* targetted_menu : the current targetted menu
* arrows_xys : the arrows xy coordinates
* dialog_box : the dialog box
*/
int manageSmKeyPressed(Player *player, char key_pressed, int *targetted_menu, int arrows_xys[], char *dialog_box) {
  int key_pressed_status = 0;//keep on start menu
  if (key_pressed == MOVE_Z) {
    removeArrow(arrows_xys[*targetted_menu], dialog_box);
    key_pressed_status = changeTargettedMenu(targetted_menu, SM_QUITTER_ID, SM_POKEMONS_ID, SM_ANNULER_ID, SM_SAC_ID);
    addArrow(arrows_xys[*targetted_menu], dialog_box);
  } else if (key_pressed == MOVE_Q) {
    removeArrow(arrows_xys[*targetted_menu], dialog_box);
    key_pressed_status = changeTargettedMenu(targetted_menu, SM_SAC_ID, SM_POKEMONS_ID, SM_ANNULER_ID, SM_QUITTER_ID);
    addArrow(arrows_xys[*targetted_menu], dialog_box);
  } else if (key_pressed == MOVE_S) {
    removeArrow(arrows_xys[*targetted_menu], dialog_box);
    key_pressed_status = changeTargettedMenu(targetted_menu, SM_POKEMONS_ID, SM_QUITTER_ID, SM_SAC_ID, SM_ANNULER_ID);
    addArrow(arrows_xys[*targetted_menu], dialog_box);
  } else if (key_pressed == MOVE_D) {
    removeArrow(arrows_xys[*targetted_menu], dialog_box);
    key_pressed_status = changeTargettedMenu(targetted_menu, SM_POKEMONS_ID, SM_SAC_ID, SM_QUITTER_ID, SM_ANNULER_ID);
    addArrow(arrows_xys[*targetted_menu], dialog_box);
  } else if (key_pressed == 13) {
    key_pressed_status = manageSmEnterKeyPressed(player, targetted_menu, dialog_box);
  } else if (key_pressed == 127) {
    key_pressed_status = 1;//exit start menu
    eraseDialogBoxLines(dialog_box);
    eraseSmPipes(dialog_box);
  } else {
    key_pressed_status = 2;//does nothing
  }
  return key_pressed_status;
}


/*****************/
/* POKEMONS PANE */
/*****************/

/* Prints the pokemons pane
* player : the player
* targetted_pkmn : an integer describing the targetted pokemon (or the 'Annuler' case)
* mode : the print mode (the actions unabled in the pokemon menu are not the same from where you triggers it)
  mode = 0 -> triggered from start menu
  mode = 1 -> triggered from battle pane main menu
*/
void printPokemonsPane(Player *player, int targetted_pkmn, int mode) {
  tty_reset();
  clearConsole();
  printf("\n\n          Vos Pokémons :\n\n\n");
  for (int i = 0; i < player->pkmn_count; i++) {
    if (i == targetted_pkmn) {
      printf("  ->  lvl %d - %s  %d/%d Pdv\n\n", player->pkmns[i].lvl, player->pkmns[i].name, player->pkmns[i].stats.hp, player->pkmns[i].stats.hp_max);
    } else {
      printf("      lvl %d - %s  %d/%d Pdv\n\n", player->pkmns[i].lvl, player->pkmns[i].name, player->pkmns[i].stats.hp, player->pkmns[i].stats.hp_max);
    }
  }
  if (mode == 1 || mode == 2) {
    printf("\n   1. Description     2. Envoyer au combat\n\n\n");
  } else {
    printf("\n   1. Description     2. Mettre en première place\n\n\n");
  }
  if (mode != 2) {
    if (targetted_pkmn == player->pkmn_count) {
      printf("\n  ->  Annuler\n");
    } else {
      printf("\n      Annuler\n");
    }
  }
  setRawMode('1');
}

/* Prints the given pokemon detailed description
* pkmn : the pokemon
*/
void printPokemonDescripton(Pokemon pkmn) {
  tty_reset();
  clearConsole();
  printPokemon(pkmn);
  printf("\n\n\n  ->  Retour\n");
  setRawMode('1');
}

/* Manages the pokemon menu
* player : the player
* mode : the pokemon pane mode (the actions unabled in the pokemon menu are not the same from where you triggers it)
  mode = 0 -> triggered from start menu
  mode = 1 -> triggered from battle pane main menu
*/
int managePokemonsMenu(Player *player, int mode) {
  int targetted_pkmn = 0;
  char key_pressed = 0;
  int key_pressed_status = 0;
  do {
    if (key_pressed_status != 2) {
      printPokemonsPane(player, targetted_pkmn, mode);
    }
    key_pressed = getchar();
    key_pressed_status = managePmKeyPressed(player, key_pressed, &targetted_pkmn, mode);
  } while(key_pressed_status != 1 && key_pressed_status != 3);//1 -> come back to start menu | 3 -> swap pokemon during battle
  return key_pressed_status;
}

/* Manages the key pressed in the pokemons menu
* player : the player
* key_pressed : the key pressed by the user
* targetted_pkmn : the targetted pokemon
* mode : the pokemon pane mode (the actions unabled in the pokemon menu are not the same from where you triggers it)
  mode = 0 -> triggered from start menu
  mode = 1 -> triggered from battle pane main menu
*/
int managePmKeyPressed(Player *player, char key_pressed, int *targetted_pkmn, int mode) {
  int key_pressed_status = 0;//keep on pokemon menu
  if (key_pressed == MOVE_S) {
    int list_item_count = (mode == 2) ? player->pkmn_count : player->pkmn_count+1;
    *targetted_pkmn = (*targetted_pkmn + 1) % list_item_count;
  } else if (key_pressed == MOVE_Z) {
    *targetted_pkmn = *targetted_pkmn - 1;
    if (*targetted_pkmn == -1) {
      *targetted_pkmn = (mode == 2) ? player->pkmn_count-1 : player->pkmn_count;
    }
  } else if (key_pressed == 127 && mode != 2) {
    key_pressed_status = 1;
  } else if (key_pressed == 13) {
    if (mode != 2) {
      if (*targetted_pkmn == player->pkmn_count) {
        key_pressed_status = 1;//come back to start menu
      } else {
        key_pressed_status = 2;//does nothing
      }
    }
  } else if (key_pressed == '1') {
    if (*targetted_pkmn != player->pkmn_count) {
      printPokemonDescripton(player->pkmns[*targetted_pkmn]);
      char key_pressed = 0;
      while (key_pressed != 13 && key_pressed != 127) {//13 ->'Enter' | 127 -> 'Return'
        key_pressed = getchar();
      }
    } else {
      key_pressed_status = 2;//does nothing
    }
  } else if (key_pressed == '2') {
    if (*targetted_pkmn != player->pkmn_count && *targetted_pkmn != 0) {
      if (player->pkmns[*targetted_pkmn].stats.hp > 0) {
        swapPokemon(player, *targetted_pkmn, 0);
        if (mode == 1  || mode == 2) {
          key_pressed_status = 3;//swap pokemon during the battle from the main menu
        }
      } else {
        key_pressed_status = 2;
      }
    } else {
      key_pressed_status = 2;//does nothing
    }
  } else {
    key_pressed_status = 2;
  }
  return key_pressed_status;
}