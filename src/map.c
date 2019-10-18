#include "map.h"
#include "accountManager.h"
#include "bag.h"
#include "battle.h"
#include "fileManager.h"
#include "pokemon.h"
#include "print.h"
#include "startMenu.h"
#include "ttyraw.h"
#include "util.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Waits until the user press the 'Enter' key
 */
void waitForEnter() {
  printf("'Enter' pour continuer\n");
  getchar();
}

/* Clears the console
 */
void clearConsole() {
  printf("\e[1;1H\e[2J");
  // system("clear");
  // window.clrtobot();
}

/* Gets the xy coordinates of the square ifo the player
 * player : the player
 */
int getXYIfoPlayer(Player *player) {
  int xy_in_front_of_player = 0;
  if (player->pos == PLAYER_N) {
    xy_in_front_of_player = player->xy * 2 + 2 - LINE_SEPARATOR * 2;
  } else if (player->pos == PLAYER_S) {
    xy_in_front_of_player = player->xy * 2 + 2 + LINE_SEPARATOR * 2;
  } else if (player->pos == PLAYER_E) {
    xy_in_front_of_player = player->xy * 2 + 4;
  } else if (player->pos == PLAYER_W) {
    xy_in_front_of_player = player->xy * 2;
  }
  return xy_in_front_of_player;
}

/* Removes an interactive object from the map
 * x_map : the x coordinate of the current map
 * y_map : the y coordinate of the current map
 * player : the player
 * map_structure : an array storing the map
 */
void removeInteractiveObject(int x_map, int y_map, Player *player,
                             char *map_structure) {
  FILE *save_file = openPlayerSaveFile(player, "r");
  int scan_status = 0;
  skipPlayerData(save_file);
  int x_map_tmp;
  int y_map_tmp;
  int xy_tmp;
  while (scan_status != EOF) {
    scan_status =
        fscanf(save_file, "%d;%d %d\n", &x_map_tmp, &y_map_tmp, &xy_tmp);
    if (x_map == x_map_tmp && y_map == y_map_tmp) {
      map_structure[xy_tmp] = ' ';
    }
  }
  closeFile(save_file);
}

/* Fills an array with all the character of the n-th map file
 * map_number : the numero of the map file
 * map_structure : the array to fill representing the map
 */
void loadMap(int x_map, int y_map, char *map_structure, Player *player) {
  char map_path[20];
  sprintf(map_path, "maps/map%d;%d.txt", x_map, y_map);
  FILE *map_file = openFile(map_path, "r");
  fread(map_structure, sizeof(char), MAP_SIZE, map_file);
  closeFile(map_file);
  removeInteractiveObject(x_map, y_map, player, map_structure);
}

/* Prints the map on the console
 * map_structure : the array representing a printable map
 * player_pos : the orientation of the player in the map
 * return the array representing the printable map
 */
void createPrintableMap(char *printable_map, char *map_structure,
                        Player player) {
  printable_map[0] = '\n'; // first empty line (padding)
  printable_map[1] = ' ';  // first space
  int i = 0;
  for (i; i < MAP_SIZE; i++) {
    if (i == player.xy) { // if the player orientation is reached
      printable_map[TO_PRINTABLE_MAP1 * i + TO_PRINTABLE_MAP2] = player.pos;
      player.char_at_pos = map_structure[i];
    } else {
      printable_map[TO_PRINTABLE_MAP1 * i + TO_PRINTABLE_MAP2] =
          map_structure[i];
    }
    printable_map[TO_PRINTABLE_MAP1 * i + TO_PRINTABLE_MAP2 + 1] = ' ';
  }
  printable_map[PRINTABLE_MAP_SIZE] = '\0';
}

void printArray(char *array) {
  char last_char = ' ';
  for (int i = 0; i < strlen(array); i++) {
    if (last_char != array[i]) {
      switch (array[i]) {
      case '@':
        textColor(DIM, GREEN);
        break;
      case '~':
        textColor(BRIGHT, BLUE);
        break;
      case 'W':
        textColor(BRIGHT, GREEN);
        break;
      case 'o':
        textColor(BRIGHT, RED);
        break;
      case '0':
        textColor(DIM, YELLOW);
        break;
      case 'O':
        textColor(DIM, YELLOW);
        break;
      case '[':
        textColor(BRIGHT, BLACK);
        break;
      case ']':
        textColor(BRIGHT, BLACK);
        break;
      case '_':
        textColor(BRIGHT, BLACK);
        break;
      case 'T':
        textColor(DIM, GREEN);
        break;
      case '#':
        textColor(BRIGHT, MAGENTA);
        break;
      default:
        textColor(RESET, WHITE);
        break;
      }
    }
    printf("%c", array[i]);
    last_char = array[i];
  }
  textColor(RESET, WHITE);
}

/* Prints the map on the console
 * printable_map : the array representing a printable map
 * clear_db : true if you want to clear the dialog box, false otherwise
 */
void clearAndPrintMap(char *printable_map, char *dialog_box) {
  clearConsole();
  tty_reset();
  printArray(printable_map);
  printf("\n");
  printDialogBox(dialog_box);
  setRawMode('1');
}

bool isObstacle(char char_at_new_xy) {
  int obstacle_count = 9;
  char obstacles[obstacle_count];
  obstacles[0] = TREE;
  obstacles[1] = POKEBALL;
  obstacles[2] = CUTABLE_TREE;
  obstacles[3] = STONE;
  obstacles[4] = WATER_;
  obstacles[5] = PILAR;
  obstacles[6] = DOOR_LEFT_PILAR;
  obstacles[7] = DOOR_RIGHT_PILAR;
  obstacles[8] = DOOR;
  bool is_obstacle = false;
  int i = 0;
  while (!is_obstacle && i < obstacle_count) {
    if (obstacles[i] == char_at_new_xy) {
      is_obstacle = true;
    }
    i++;
  }
  return is_obstacle;
}

/* Moves the player
 * player : the player
 * new_pos : the new orientation of the player
 * xy_sub : the new position of the player
 * printable_map : the printable map
 */
void movePlayer(Player *player, char new_pos, int xy_sub, char *printable_map,
                char *dialog_box, int *x_map, int *y_map) {
  player->pos = new_pos;
  player->xy = player->xy - xy_sub;
  char char_at_new_xy =
      printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2];
  if (isEqual(char_at_new_xy, HEAL)) {
    resetAllPokemonsStats(player);
    addTextInDialogBox(FRST_LINE_START, HEAL_TEXT, HEAL_TEXT_LGTH, dialog_box);
  }
  if (isEqual(char_at_new_xy, SHOP)) {
    manageShopMenu(player);
  }
  if (isObstacle(char_at_new_xy)) {
    player->xy = player->xy + xy_sub;
  } else {
    player->char_at_pos = char_at_new_xy;
    isBattle(player, printable_map, x_map, y_map);
  }
}

/* Changes the map
 * player : the player
 * x_map : the x coordinate of the new map
 * y_map : the y coordinate of the new map
 * x_map_sub : the value to sub to the x new map coordinate
 * y_map_sub : the value to sub to the y new map coordinate
 * new_pos : the new player's orientation
 * xy_player_sub : the value to add to the player xy position
 * printable_map : the printable new map
 * map_structure :the array where to load the new map
 */
void changeMap(Player *player, int *x_map, int *y_map, int x_map_sub,
               int y_map_sub, char new_pos, int xy_player_sub,
               char *printable_map, char *map_structure) {
  // set the current map coordinates to the new map ones
  *x_map = *x_map - x_map_sub;
  *y_map = *y_map - y_map_sub;

  // change the player data
  player->pos = new_pos;
  player->xy = player->xy - xy_player_sub;

  loadMap(*x_map, *y_map, map_structure, player);

  // update the char placed originally at the player position
  char char_at_new_xy = map_structure[player->xy];
  player->char_at_pos = char_at_new_xy;

  createPrintableMap(printable_map, map_structure, *player);
  isBattle(player, printable_map, x_map, y_map);
}

void comeBackFirstMap(Player *player, char *printable_map, int *x_map,
                      int *y_map) {
  char map_structure[MAP_SIZE];
  *x_map = 0;
  *y_map = 0;
  changeMap(player, x_map, y_map, 0, 0, PLAYER_N, player->xy - PLAYER_START_POS,
            printable_map, map_structure);
}

/* Save the xy coordinate of an interactive object the player has taken to
 * remember is was taken player : the player x_map : the x coordinate of the
 * current map y_map : the y coordinate of the current map xy : xy coordinate of
 * an interactive object
 */
void saveMapSpecificData(Player *player, int x_map, int y_map, int xy) {
  FILE *save_file = openPlayerSaveFile(player, "a");
  fprintf(save_file, "%d;%d %d\n", x_map, y_map, xy);
  closeFile(save_file);
}

int getMapItemInPokeballID(int *x_map, int *y_map) {
  FILE *map_items = openFile(MAPS_ITEMS_PATH, "r");
  bool trouve = false;
  int x = 100;
  int y = 100;
  int item_id = 0;
  while (fscanf(map_items, "%d;%d", &x, &y) != EOF && !trouve) {
    if (*x_map == x && *y_map == y) {
      fscanf(map_items, " %d", &item_id);
      trouve = true;
    } else {
      fscanf(map_items, "%*[^\n]\n");
    }
  }
  closeFile(map_items);
  return item_id;
}

void notifyItemFound(char *dialog_box, BagItem item) {
  int text_length = 21 + item.name_length;
  char *text = malloc(sizeof(char) * text_length);
  sprintf(text, "Vous avez trouve : %s !", item.name);
  addTextInDialogBox(FRST_LINE_START, text, text_length, dialog_box);
}

void manageItemFound(Player *player, char *dialog_box, int *x_map, int *y_map) {
  int item_id = getMapItemInPokeballID(x_map, y_map);
  // there are more than one pokeball in the 6;-1 map,
  // so this set the content of others pokeballs to Pokeball
  if (item_id == 5) {
    if (possessBagItem(player, item_id) != -1) {
      item_id = 0;
    }
  }
  BagItem item = addBagItemPlayer(player, item_id, 1);
  notifyItemFound(dialog_box, item);
}

void showDBYesNoMenu(char *dialog_box) {
  addTextInDialogBox(DB_YES_POS - 3, "-> Oui", 6, dialog_box);
  addTextInDialogBox(DB_NO_POS, "Non", 3, dialog_box);
}

void setDBArrowYesNo(int *yes, char *dialog_box) {
  int yes_pos = DB_YES_POS - 3;
  int no_pos = DB_NO_POS - 3;
  if (*yes == 1) {
    int tmp = yes_pos;
    yes_pos = no_pos;
    no_pos = tmp;
  }
  addTextInDialogBox(yes_pos, "  ", 2, dialog_box);
  addTextInDialogBox(no_pos, "->", 2, dialog_box);
}

void manageDBYesNoMenu(int *yes, char *printable_map, char *dialog_box) {
  char key_pressed;
  int key_pressed_status;
  do {
    key_pressed = getchar();
    key_pressed_status = manageYesNoKeyPressed(key_pressed, yes);
    if (key_pressed_status == 1) {
      setDBArrowYesNo(yes, dialog_box);
      clearAndPrintMap(printable_map, dialog_box);
    }
  } while (key_pressed_status != 0); //!= enter
}

void manageDestroy(Player *player, char *dialog_box, char *printable_map,
                   char destroyed_object, int xy_ifo_player) {
  int item_id = 4;
  if (destroyed_object == CUTABLE_TREE) {
    item_id = 3;
  }
  if (possessBagItem(player, item_id) != -1) {
    showDBYesNoMenu(dialog_box);
    if (destroyed_object == CUTABLE_TREE) {
      addTextInDialogBox(FRST_LINE_START, "Cet arbre semble vieux et fragile",
                         33, dialog_box);
      addTextInDialogBox(SCD_LINE_START,
                         "Voulez-vous le couper avec votre Cisaille ?", 43,
                         dialog_box);
    } else {
      addTextInDialogBox(FRST_LINE_START, "Ce rocher ne semble pas tres solide",
                         35, dialog_box);
      addTextInDialogBox(SCD_LINE_START,
                         "Voulez-vous le casser avec votre Pioche ?", 41,
                         dialog_box);
    }
    clearAndPrintMap(printable_map, dialog_box);
    int yes = 1;
    manageDBYesNoMenu(&yes, printable_map, dialog_box);
    eraseDialogBoxLines(dialog_box);
    if (yes == 1) {
      printable_map[xy_ifo_player] = ' ';
    }
  } else {
    if (destroyed_object == CUTABLE_TREE) {
      addTextInDialogBox(FRST_LINE_START, "Cet arbre semble vieux et fragile",
                         33, dialog_box);
    } else {
      addTextInDialogBox(FRST_LINE_START, "Ce rocher ne semble pas tres solide",
                         35, dialog_box);
    }
  }
}

void manageDoorOpenning(Player *player, char *dialog_box, char *printable_map) {
  if (possessAllKeys(player)) {
    showDBYesNoMenu(dialog_box);
    addTextInDialogBox(FRST_LINE_START, "Cette enorme porte est fermee", 29,
                       dialog_box);
    addTextInDialogBox(SCD_LINE_START, "Voulez-vous l'ouvrir ?", 22,
                       dialog_box);
    clearAndPrintMap(printable_map, dialog_box);
    int yes = 1;
    manageDBYesNoMenu(&yes, printable_map, dialog_box);
    eraseDialogBoxLines(dialog_box);
    if (yes == 1) {
      printable_map[TO_PRINTABLE_MAP1 * MIDDLE_DOOR_POS + TO_PRINTABLE_MAP2] =
          ' ';
    }
  } else {
    addTextInDialogBox(FRST_LINE_START,
                       "Cette enorme porte est fermee mais possede 5 serrures",
                       53, dialog_box);
  }
  clearAndPrintMap(printable_map, dialog_box);
}

void fishing(Player *player, char *printable_map, char *dialog_box, int *x_map,
             int *y_map) {
  srand(time(NULL));
  int r = rand() % 100;
  if (r < 5 && itemCount(player, 10) == 4) {
    addBagItemPlayer(player, 10, 1);
    addTextInDialogBox(FRST_LINE_START,
                       "Enorme prise ! Vous recevez un Fragement de Clef !", 50,
                       dialog_box);
  } else if (r < 10) {
    addBagItemPlayer(player, 0, 1);
    addTextInDialogBox(FRST_LINE_START,
                       "Cela a mordu ! Vous recevez une Pokeball !", 42,
                       dialog_box);
  } else if (r < 50) {
    goForBattle(player, printable_map, x_map, y_map);
  } else {
    addTextInDialogBox(FRST_LINE_START, "Cela ne mord pas...", 19, dialog_box);
  }
  clearAndPrintMap(printable_map, dialog_box);
}

void manageFishing(Player *player, char *dialog_box, char *printable_map,
                   int *x_map, int *y_map) {
  if (possessBagItem(player, 2) != -1) {
    showDBYesNoMenu(dialog_box);
    addTextInDialogBox(FRST_LINE_START, "Cette eau est d'un bleu etincellant",
                       35, dialog_box);
    addTextInDialogBox(SCD_LINE_START, "Voulez-vous pecher ?", 20, dialog_box);
    clearAndPrintMap(printable_map, dialog_box);
    int yes = 1;
    manageDBYesNoMenu(&yes, printable_map, dialog_box);
    eraseDialogBoxLines(dialog_box);
    if (yes == 1) {
      fishing(player, printable_map, dialog_box, x_map, y_map);
    }
  } else {
    addTextInDialogBox(FRST_LINE_START,
                       "Vous appercevez des silhouettes dans l'eau", 42,
                       dialog_box);
  }
  clearAndPrintMap(printable_map, dialog_box);
}

/* Check if an interacion is possible
 * player : the player
 * printable_map : the printable map
 * dialog_box : the dialog box
 * x_map : the x coordinate of the current map
 * y_map : the y coordinate of the current map
 * return 2 to clear and print back the map, 0 to do nothing
 */
int checkIfInteractionPossible(Player *player, char *printable_map,
                               char *dialog_box, int *x_map, int *y_map) {
  printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
      player->pos;
  int interaction_found = 2; // clear and print
  int xy_ifo_player = getXYIfoPlayer(player);
  char char_ifo_player = printable_map[xy_ifo_player];
  if (char_ifo_player == POKEBALL) {
    if (*x_map == 2 && *y_map == -3) { // trapped pokeball
      bool lost = goForBattle(player, printable_map, x_map, y_map);
      if (!lost) {
        addTextInDialogBox(
            FRST_LINE_START,
            "Le Pokemon s'enfuit en laissant une clef par terre.", 51,
            dialog_box);
        addTextInDialogBox(SCD_LINE_START,
                           "Bravo, vous avez obtenu une Clef de Diamand !", 45,
                           dialog_box);
        addBagItemPlayer(player, 7, 1);
        clearAndPrintMap(printable_map, dialog_box);
        saveMapSpecificData(player, *x_map, *y_map,
                            (xy_ifo_player - TO_PRINTABLE_MAP2) / 2);
        printable_map[xy_ifo_player] = ' ';
      } else {
        eraseDialogBoxLines(dialog_box);
      }
    } else {
      saveMapSpecificData(player, *x_map, *y_map,
                          (xy_ifo_player - TO_PRINTABLE_MAP2) / 2);
      printable_map[xy_ifo_player] = ' ';
      manageItemFound(player, dialog_box, x_map, y_map);
    }
  } else if (char_ifo_player == STONE) {
    manageDestroy(player, dialog_box, printable_map, char_ifo_player,
                  xy_ifo_player);
  } else if (char_ifo_player == CUTABLE_TREE) {
    manageDestroy(player, dialog_box, printable_map, char_ifo_player,
                  xy_ifo_player);
  } else if (char_ifo_player == DOOR) {
    manageDoorOpenning(player, dialog_box, printable_map);
  } else if (char_ifo_player == WATER_) {
    manageFishing(player, dialog_box, printable_map, x_map, y_map);
  } else {
    interaction_found = 0; // do not clear and print
  }
  printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
      player->char_at_pos;
  return interaction_found;
}

/* Manages the key pressed by the user
 * key_pressed : the key pressed
 * player : the player
 * printable_map : the printable map
 * x_map : the x coordinate of the current map
 * y_map : the y coordinate of the current map
 * return true if a right key has been pressed, false otherwise
 */
int manageKeyPressed(char key_pressed, Player *player, char *dialog_box,
                     char *printable_map, int *x_map, int *y_map) {
  int key_status = 1; // arrow moved
  char map_structure[MAP_SIZE];
  printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
      player->char_at_pos;

  if (key_pressed == MOVE_Z) {
    if (player->xy > 0 && player->xy < LINE_SEPARATOR) {
      changeMap(player, x_map, y_map, 0, -1, PLAYER_N, -(11 * LINE_SEPARATOR),
                printable_map, map_structure);
    } else {
      movePlayer(player, PLAYER_N, LINE_SEPARATOR, printable_map, dialog_box,
                 x_map, y_map);
    }
  } else if (key_pressed == MOVE_Q) {
    if (player->xy % LINE_SEPARATOR == 0) {
      changeMap(player, x_map, y_map, 1, 0, PLAYER_W, -(LINE_SEPARATOR - 3),
                printable_map, map_structure);
    } else {
      movePlayer(player, PLAYER_W, 1, printable_map, dialog_box, x_map, y_map);
    }
  } else if (key_pressed == MOVE_S) {
    if (player->xy > (MAP_SIZE - LINE_SEPARATOR) && player->xy < MAP_SIZE) {
      changeMap(player, x_map, y_map, 0, 1, PLAYER_S, 11 * LINE_SEPARATOR,
                printable_map, map_structure);
    } else {
      movePlayer(player, PLAYER_S, -(LINE_SEPARATOR), printable_map, dialog_box,
                 x_map, y_map);
    }
  } else if (key_pressed == MOVE_D) {
    if (player->xy % LINE_SEPARATOR == 12) {
      changeMap(player, x_map, y_map, -1, 0, PLAYER_E, LINE_SEPARATOR - 3,
                printable_map, map_structure);
    } else {
      movePlayer(player, PLAYER_E, -1, printable_map, dialog_box, x_map, y_map);
    }
  } else if (key_pressed == 13) {
    key_status = checkIfInteractionPossible(
        player, printable_map, dialog_box, x_map,
        y_map); // 2 -> interaction found / 0 otherwise
  } else if (key_pressed == EXIT) {
    exit(0);
  } else if (key_pressed == ' ') {
    key_status = 4; // start menu
  } else if (key_pressed != 0) {
    key_status = 0; // wrong key
  }

  printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
      player->pos;

  if (key_status == 4) { // start menu pressed
    createStartMenu(dialog_box);
    clearAndPrintMap(printable_map, dialog_box);
    key_status = manageStartMenu(player, printable_map, dialog_box);
  }

  return key_status;
}
