#include "startMenu.h"
#include "bag.h"
#include "map.h"
#include "pokemon.h"
#include "print.h"

#include "util.h"
#include <stdio.h>
#include <ncurses.h>

/**************/
/* START MENU */
/**************/

/* Creates the start menu (it's situated in the dialog box)
 * dialog_box : the dialog box
 */
void createStartMenu(char *dialog_box) {
  shiftMenuPipes(dialog_box, SM_FRST_PIPE_POS, SM_FRST_PIPE_POS, SM_NEXT_LINE,
                 DIALOG_BOX_LINE_COUNT - 1);
  addStringToArray(SM_POKEMONS, SM_POKEMONS_LGTH, SM_POKEMONS_POS, dialog_box);
  addStringToArray(SM_SAC, SM_SAC_LGTH, SM_SAC_POS, dialog_box);
  addStringToArray(SM_QUITTER, SM_QUITTER_LGTH, SM_QUITTER_POS, dialog_box);
  addStringToArray(SM_ANNULER, SM_ANNULER_LGTH, SM_ANNULER_POS, dialog_box);
  addArrow(SM_POKEMONS_POS - SM_ARROW_LEFT_SHIFT, dialog_box);
}

/* Manages the start menu working when user triggers it
 * printable_map : the printable map
 * dialog_box : the dialog box
 */
int manageStartMenu(Player *player, char *printable_map, char *dialog_box) {
  int targetted_menu = 0;
  int arrows_xys[4] = {SM_POKEMONS_POS - SM_ARROW_LEFT_SHIFT,
                       SM_SAC_POS - SM_ARROW_LEFT_SHIFT,
                       SM_QUITTER_POS - SM_ARROW_LEFT_SHIFT,
                       SM_ANNULER_POS - SM_ARROW_LEFT_SHIFT};
  char key_pressed = 0;
  int key_pressed_status = 0;
  do {
    getUserInput(&key_pressed);
    key_pressed_status = manageSmKeyPressed(
        player, key_pressed, &targetted_menu, arrows_xys, dialog_box);
    if (key_pressed_status != 2) {
      clearAndPrintMap(printable_map, dialog_box);
    }
  } while (key_pressed_status != 1 && key_pressed_status != EXIT);
  return key_pressed_status;
}

/* Manaages the enter key pressed
 * targetted_menu : an integer describing which menu in the start menu is
 * targetted dialog_box : the dialog box
 */
int manageSmEnterKeyPressed(Player *player, int *targetted_menu,
                            char *dialog_box) {
  int key_pressed_status = 0;
  if (*targetted_menu == SM_POKEMONS_ID) {
    int targetted_pkmn = 0;
    managePokemonsMenu(player, 0, &targetted_pkmn);
  } else if (*targetted_menu == SM_SAC_ID) {
    manageBagMenu(player, 1);
  } else if (*targetted_menu == SM_QUITTER_ID) {
    key_pressed_status = EXIT;
  } else if (*targetted_menu == SM_ANNULER_ID) {
    key_pressed_status = 1; // exit start menu
    eraseDialogBoxLines(dialog_box);
    eraseSmPipes(dialog_box);
  }
  return key_pressed_status;
}

/* Changes the targetted menu
* targetted_menu : an integer describing which menu in the start menu is
targetted
// each menu is identifed by an integer
* if_this_menu :
* then_this_one :
* else_if_this_menu :
* then_that_one :
* return 0 if an arrow was moved, 2 otherwise
*/
int changeTargettedMenu(int *targetted_menu, int if_this_menu,
                        int then_this_one, int else_if_this_menu,
                        int then_that_one) {
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
  dialog_box[SM_FRST_PIPE_POS + (DIALOG_BOX_LINE_COUNT - 2) * SM_NEXT_LINE] =
      '_';
}

/* Manages the key pressed in the start menu
 * key_pressed : the key pressed
 * targetted_menu : the current targetted menu
 * arrows_xys : the arrows xy coordinates
 * dialog_box : the dialog box
 */
int manageSmKeyPressed(Player *player, char key_pressed, int *targetted_menu,
                       int arrows_xys[], char *dialog_box) {
  int key_pressed_status = 0; // keep on start menu
  if (key_pressed == MOVE_Z) {
    removeArrow(arrows_xys[*targetted_menu], dialog_box);
    key_pressed_status =
        changeTargettedMenu(targetted_menu, SM_QUITTER_ID, SM_POKEMONS_ID,
                            SM_ANNULER_ID, SM_SAC_ID);
    addArrow(arrows_xys[*targetted_menu], dialog_box);
  } else if (key_pressed == MOVE_Q) {
    removeArrow(arrows_xys[*targetted_menu], dialog_box);
    key_pressed_status =
        changeTargettedMenu(targetted_menu, SM_SAC_ID, SM_POKEMONS_ID,
                            SM_ANNULER_ID, SM_QUITTER_ID);
    addArrow(arrows_xys[*targetted_menu], dialog_box);
  } else if (key_pressed == MOVE_S) {
    removeArrow(arrows_xys[*targetted_menu], dialog_box);
    key_pressed_status =
        changeTargettedMenu(targetted_menu, SM_POKEMONS_ID, SM_QUITTER_ID,
                            SM_SAC_ID, SM_ANNULER_ID);
    addArrow(arrows_xys[*targetted_menu], dialog_box);
  } else if (key_pressed == MOVE_D) {
    removeArrow(arrows_xys[*targetted_menu], dialog_box);
    key_pressed_status =
        changeTargettedMenu(targetted_menu, SM_POKEMONS_ID, SM_SAC_ID,
                            SM_QUITTER_ID, SM_ANNULER_ID);
    addArrow(arrows_xys[*targetted_menu], dialog_box);
  } else if (key_pressed == ENTER) {
    key_pressed_status =
        manageSmEnterKeyPressed(player, targetted_menu, dialog_box);
  } else if (key_pressed == 127) {
    key_pressed_status = 1; // exit start menu
    eraseDialogBoxLines(dialog_box);
    eraseSmPipes(dialog_box);
  } else {
    key_pressed_status = 2; // does nothing
  }
  return key_pressed_status;
}

/*****************/
/* POKEMONS PANE */
/*****************/

/* Prints the pokemons pane
* player : the player
* targetted_pkmn : an integer describing the targetted pokemon (or the 'Annuler'
case)
* mode : the print mode (the actions unabled in the pokemon menu are not the
same from where you triggers it) mode = 0 -> triggered from start menu mode = 1
-> triggered from battle pane main menu mode = 2 -> triggered when a pokemon is
ko mode = 3 -> triggered from bag item
*/
void printPokemonsPane(Player *player, int targetted_pkmn, int mode) {
  //tty_reset();
  clearConsole();
  printw("\n\n          Vos Pokémons :\n\n\n");
  for (int i = 0; i < player->pkmn_count; i++) {
    if (i == targetted_pkmn) {
      printw("  ->  lvl %d - %s  %d/%d Pdv\n\n", player->pkmns[i].lvl,
             player->pkmns[i].name, player->pkmns[i].stats.hp,
             player->pkmns[i].stats.hp_max);
    } else {
      printw("      lvl %d - %s  %d/%d Pdv\n\n", player->pkmns[i].lvl,
             player->pkmns[i].name, player->pkmns[i].stats.hp,
             player->pkmns[i].stats.hp_max);
    }
  }
  if (mode == 1 || mode == 2) {
    printw("\n\n\n    ----------------      ----------------------\n");
    printw("   | 1. Description |    | 2. Envoyer au combat |\n");
    printw("    ----------------      ----------------------\n\n\n");
  } else if (mode == 3) {
    printw("\n\n\n    -------------\n");
    printw("   | 1. Utiliser |\n");
    printw("    -------------\n\n\n");
  } else {
    printw("\n\n\n    ----------------      -----------------------------\n");
    printw("   | 1. Description |    | 2. Mettre en première place |\n");
    printw("    ----------------      -----------------------------\n\n\n");
  }
  if (mode != 2) {
    if (targetted_pkmn == player->pkmn_count) {
      printw("\n  ->  Annuler\n");
    } else {
      printw("\n      Annuler\n");
    }
  }
  //setRawMode('1');
}

/* Prints the given pokemon detailed description
 * pkmn : the pokemon
 */
void printPokemonDescripton(Pokemon pkmn) {
  //tty_reset();
  clearConsole();
  printPokemon(pkmn);
  printw("\n  ->  Retour\n");
  //setRawMode('1');
}

/* Manages the pokemon menu
* player : the player
* mode : the pokemon pane mode (the actions unabled in the pokemon menu are not
the same from where you triggers it) mode = 0 -> triggered from start menu mode
= 1 -> triggered from battle pane main menu mode = 2 -> triggered when a pokemon
is ko mode = 3 -> triggered from bag item
*/
int managePokemonsMenu(Player *player, int mode, int *targetted_pkmn) {
  char key_pressed = 0;
  int key_pressed_status = 0;
  do {
    if (key_pressed_status != 2) {
      printPokemonsPane(player, *targetted_pkmn, mode);
    }
    getUserInput(&key_pressed);
    key_pressed_status =
        managePmKeyPressed(player, key_pressed, targetted_pkmn, mode);
  } while (
      key_pressed_status != 1 &&
      key_pressed_status !=
          3); // 1 -> come back to start menu | 3 -> swap pokemon during battle
  return key_pressed_status;
}

/* Manages the key pressed in the pokemons menu
* player : the player
* key_pressed : the key pressed by the user
* targetted_pkmn : the targetted pokemon
* mode : the pokemon pane mode (the actions unabled in the pokemon menu are not
the same from where you triggers it) mode = 0 -> triggered from start menu mode
= 1 -> triggered from battle pane main menu mode = 2 -> triggered when a pokemon
is ko mode = 3 -> triggered from bag item
*/
int managePmKeyPressed(Player *player, char key_pressed, int *targetted_pkmn,
                       int mode) {
  int key_pressed_status = 0; // keep on pokemon menu
  if (key_pressed == MOVE_S) {
    int list_item_count =
        (mode == 2) ? player->pkmn_count : player->pkmn_count + 1;
    *targetted_pkmn = (*targetted_pkmn + 1) % list_item_count;
  } else if (key_pressed == MOVE_Z) {
    *targetted_pkmn = *targetted_pkmn - 1;
    if (*targetted_pkmn == -1) {
      *targetted_pkmn =
          (mode == 2) ? player->pkmn_count - 1 : player->pkmn_count;
    }
  } else if (key_pressed == 127 && mode != 2) {
    key_pressed_status = 1;
  } else if (key_pressed == ENTER) {
    if (mode != 2) {
      if (*targetted_pkmn == player->pkmn_count) {
        key_pressed_status = 1; // come back to menu
      } else {
        key_pressed_status = 2; // does nothing
      }
    }
  } else if (key_pressed == '1') {
    if (*targetted_pkmn != player->pkmn_count) {
      if (mode == 3) {
        key_pressed_status = 1; // come back to bag
      } else {
        printPokemonDescripton(player->pkmns[*targetted_pkmn]);
        char key_pressed = 0;
        while (key_pressed != ENTER &&
               key_pressed != DELETE) { // 13 ->'Enter' | 127 -> 'Return'
          getUserInput(&key_pressed);
        }
      }

    } else {
      key_pressed_status = 2; // does nothing
    }
  } else if (key_pressed == '2') {
    if (*targetted_pkmn != player->pkmn_count && *targetted_pkmn != 0) {
      if (player->pkmns[*targetted_pkmn].stats.hp > 0) {
        swapPokemon(player, *targetted_pkmn, 0);
        if (mode == 1 || mode == 2) {
          key_pressed_status =
              3; // swap pokemon during the battle from the main menu
        }
      } else {
        key_pressed_status = 2;
      }
    } else {
      key_pressed_status = 2; // does nothing
    }
  } else {
    key_pressed_status = 2;
  }
  return key_pressed_status;
}

/*******/
/* BAG */
/*******/

/* Prints the bag pane
 * player : the player
 * mode : 0 if in battle, 1 if out of battle
 */
void printBagPane(Player *player, int targetted_item, int mode) {
  //tty_reset();
  clearConsole();
  printw("\n\n          Votre Sac :\n\n\n");
  int i = 0;
  while (i < player->bag_item_count) {
    if (targetted_item == i) {
      printw("  ->  %s : x%d\n", player->bag[i].name, player->bag[i].count);
    } else {
      printw("      %s : x%d\n", player->bag[i].name, player->bag[i].count);
    }
    printw("        Description : %s\n\n", player->bag[i].description);
    i++;
  }
  if (player->bag_item_count == 0) {
    printw("      Vide\n\n");
  }

  int usable_time = (int)player->bag[targetted_item].usable_time;
  /*if not on Cancel field and if targetted item is currently usable*/
  if (player->bag_item_count != targetted_item &&
      isItemUsable(mode, usable_time)) {
    printw("\n\n    -------------\n");
    printw("   | 1. Utiliser |\n");
    printw("    -------------\n\n");
  } else {
    printw("\n\n\n\n\n\n");
  }

  printw("\n      Argent : %d $teamy\n\n", player->money);

  if (player->bag_item_count == targetted_item) {
    printw("\n  ->  Annuler\n");
  } else {
    printw("\n      Annuler\n");
  }
  //setRawMode('1');
}

int manageBagMenuKeyPressed(char key_pressed, int *targetted_item,
                            Player *player, int mode) {
  int key_pressed_status = 0; // refresh bag menu
  if (key_pressed == MOVE_S) {
    *targetted_item = (*targetted_item + 1) % (player->bag_item_count + 1);
  } else if (key_pressed == MOVE_Z) {
    *targetted_item = *targetted_item - 1;
    if (*targetted_item == -1) {
      *targetted_item = player->bag_item_count;
    }
  } else if (key_pressed == DELETE) {
    key_pressed_status = 4; // come back to start menu
  } else if (key_pressed == ENTER) {
    if (*targetted_item == player->bag_item_count) {
      key_pressed_status = 4; // come back to start menu
    } else {
      key_pressed_status = 2; // does nothing
    }
  } else if (key_pressed == '1') {
    if (isItemUsable(mode, (int)player->bag[*targetted_item].usable_time)) {
      if (mode == 1) { // bag triggered from start menu
        int targetted_pkmn = 0;
        managePokemonsMenu(player, 3, &targetted_pkmn);
        if (player->bag[*targetted_item].id == POTION_ID) {
          usePotion(player, targetted_pkmn);
        }
        if (player->bag[*targetted_item].id == TOTAL_SOIN_ID) {
          useTotalSoin(player, targetted_pkmn);
        }
        key_pressed_status = 3; // use an item
      } else {
        key_pressed_status = 1; // come back to menu
      }
    } else {
      key_pressed_status = 2; // does nothing
    }
  } else {
    key_pressed_status = 2; // does nothing
  }
  return key_pressed_status;
}

/* Manages the bag menu
 * player : the player
 * mode : 0 if in battle, 1 if out battle
 * return -1 if nothing happened, otherwise the index of the item that was used
 */
int manageBagMenu(Player *player, int mode) {
  char key_pressed = 0;
  int key_pressed_status = 0;
  int targetted_item = 0;
  do {
    if (key_pressed_status == 0 || key_pressed_status == 3) {
      printBagPane(player, targetted_item, mode);
    }
    getUserInput(&key_pressed);
    key_pressed_status =
        manageBagMenuKeyPressed(key_pressed, &targetted_item, player, mode);
  } while (!(key_pressed_status == 1 || key_pressed_status == 4));
  if (key_pressed_status == 4) { // if nothing happened
    targetted_item = -1;
  }
  return targetted_item;
}

/********/
/* SHOP */
/********/

int initShopItems(Player *player, BagItem *shop_items) {
  int shop_size = 3;
  shop_items[0] = createBagItem(POKEBALL_ID);
  shop_items[0].price = POKEBALL_PRICE;
  shop_items[1] = createBagItem(POTION_ID);
  shop_items[1].price = POTION_PRICE;
  shop_items[2] = createBagItem(TOTAL_SOIN_ID);
  shop_items[2].price = TOTAL_SOIN_PRICE;
  int index = shop_size;
  if (possessBagItem(player, X_KEY_ID) == -1) {
    shop_items[index] = createBagItem(X_KEY_ID);
    shop_items[index].price = X_KEY_PRICE;
    shop_size++;
    index++;
  }
  if (possessBagItem(player, SHARP_KEY_ID) == -1) {
    shop_items[index] = createBagItem(SHARP_KEY_ID);
    shop_size++;
  }
  return shop_size;
}

void printShopPane(BagItem *shop_items, int shop_size, Player *player,
                   int targetted_item) {
  //tty_reset();
  clearConsole();
  printw("\n\n          Boutique :\n\n\n");
  int i = 0;
  while (i < shop_size) {
    if (targetted_item == i) {
      printw("  ->  %s :\n", shop_items[i].name);
    } else {
      printw("      %s :\n", shop_items[i].name);
    }
    printw("        Description : %s\n", shop_items[i].description);
    if (shop_items[i].id == 9) {
      printw("        Prix : 5 Fragments de Clef\n\n");
    } else {
      printw("        Prix : %d $teamy\n\n", shop_items[i].price);
    }
    i++;
  }

  bool canBuy = false;
  if (shop_size != targetted_item) {
    if (shop_items[targetted_item].id == SHARP_KEY_ID) {
      if (itemCount(player, KEY_FRAG_ID) == 5) {
        canBuy = true;
      }
    } else {
      if (shop_items[targetted_item].price <= player->money) {
        canBuy = true;
      }
    }
  }

  if (canBuy) {
    printw("\n\n    ------------\n");
    printw("   | 1. Acheter |\n");
    printw("    ------------\n\n");
  } else {
    printw("\n\n\n\n\n\n");
  }

  printw("\n\n      Argent : %d $teamy\n\n", player->money);

  if (shop_size == targetted_item) {
    printw("\n  ->  Annuler\n");
  } else {
    printw("\n      Annuler\n");
  }
  //setRawMode('1');
}

int manageShopMenuKeyPressed(BagItem *shop_items, int shop_size,
                             char key_pressed, int *targetted_item,
                             Player *player) {
  int key_pressed_status = 0; // refresh bag menu
  if (key_pressed == MOVE_S) {
    *targetted_item = (*targetted_item + 1) % (shop_size + 1);
  } else if (key_pressed == MOVE_Z) {
    *targetted_item = *targetted_item - 1;
    if (*targetted_item == -1) {
      *targetted_item = shop_size;
    }
  } else if (key_pressed == DELETE) {
    key_pressed_status = 1; // come back to start menu
  } else if (key_pressed == ENTER) {
    if (*targetted_item == shop_size) {
      key_pressed_status = 1; // come back to start menu
    } else {
      key_pressed_status = 2; // does nothing
    }
  } else if (key_pressed == '1') {
    if (*targetted_item == shop_size) {
      key_pressed_status = 2; // does nothing
    } else {
      if (shop_items[*targetted_item].id == SHARP_KEY_ID) {
        if (itemCount(player, KEY_FRAG_ID) == 5) {
          removeItem(player, KEY_FRAG_ID, 5);
          addBagItemPlayer(player, SHARP_KEY_ID, 1);
          *targetted_item = 0;
        } else {
          key_pressed_status = 2; // does nothing
        }
      } else {
        if (shop_items[*targetted_item].price <= player->money) {
          addBagItemPlayer(player, shop_items[*targetted_item].id, 1);
          player->money -= shop_items[*targetted_item].price;
          // *targetted_item = 0;
        }
      }
    }
  } else {
    key_pressed_status = 2; // does nothing
  }
  return key_pressed_status;
}

int manageShopMenu(Player *player) {
  int shop_items_length = 5;
  BagItem shop_items[shop_items_length];
  int shop_size = initShopItems(player, shop_items);
  char key_pressed = 0;
  int key_pressed_status = 0;
  int targetted_item = 0;
  do {
    if (key_pressed_status == 0) {
      printShopPane(shop_items, shop_size, player, targetted_item);
    }
    getUserInput(&key_pressed);
    key_pressed_status = manageShopMenuKeyPressed(
        shop_items, shop_size, key_pressed, &targetted_item, player);
    shop_size = initShopItems(player, shop_items);
  } while (key_pressed_status != 1 && key_pressed_status != 3);
  if (targetted_item == player->bag_item_count ||
      key_pressed_status != 3) { // if nothing happened
    targetted_item = -1;
  }
  return targetted_item;
}
