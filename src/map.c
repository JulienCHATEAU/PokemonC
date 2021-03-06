#include "map.h"
#include "accountManager.h"
#include "bag.h"
#include "battle.h"
#include "fileManager.h"
#include "pnj.h"
#include "pokemon.h"
#include "print.h"
#include "startMenu.h"

#include "util.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Waits until the user press the 'Enter' key
 */
void waitForEnter() {
  printw("'Enter' pour continuer\n");
  enterKey();
}

/* Clears the console
 */
void clearConsole() {
  clear();
  // printw("\e[1;1H\e[2J");
  // system("clear");
  // window.clrtobot();
}

int getXYPlayer(Player *player) {
  return player->xy * 2 + 3;
}

/* Gets the xy coordinates of the square ifo the player
 * player : the player
 */
int getXYIfoPlayer(Player *player) {
  int xy_in_front_of_player = 0;
  if (player->pos == PLAYER_N) {
    xy_in_front_of_player = getXYPlayer(player) - 30;
  } else if (player->pos == PLAYER_S) {
    xy_in_front_of_player = getXYPlayer(player) + 30;
  } else if (player->pos == PLAYER_E) {
    xy_in_front_of_player = getXYPlayer(player) + 2;
  } else if (player->pos == PLAYER_W) {
    xy_in_front_of_player = getXYPlayer(player) - 2;
  }
  return xy_in_front_of_player;
}

/* Removes an interactive object from the map
 * x_map : the x coordinate of the current map
 * y_map : the y coordinate of the current map
 * player : the player
 * map_structure : an array storing the map
 */
void removeInteractiveObject(Map *map, Player *player, char *map_structure) {
  MapSquare tmp;
  int i = 0;
  while (i < player->item_picked_count) {
    tmp = player->item_picked[i];
    if (tmp.x_map == map->x && tmp.y_map == map->y) {
      if (!isMapSquareEqual(map->pnj.square, tmp)) {
        map_structure[tmp.xy] = ' ';
      }
    }
    i++;
  }
}

/* Fills an array with all the character of the n-th map file
 * map_number : the numero of the map file
 * map_structure : the array to fill representing the map
 */
void loadMap(Map *map, char *map_structure, Player *player) {
  char map_path[20];
  sprintf(map_path, "maps/map%d;%d.txt", map->x, map->y);
  FILE *map_file = openFile(map_path, "r");
  fread(map_structure, sizeof(char), MAP_SIZE, map_file);
  closeFile(map_file);
  findPnj(map->x, map->y, &map->pnj);
  if (map->x == map->pnj.square.x_map && map->y == map->pnj.square.y_map) {
    map_structure[map->pnj.square.xy] = map->pnj.orientation;
  }
  removeInteractiveObject(map, player, map_structure);
}

/* Prints the map on the console
 * map_structure : the array representing a printable map
 * player_pos : the orientation of the player in the map
 * return the array representing the printable map
 */
void createPrintableMap(char *printable_map, char *map_structure,
                        Player player) {
  // printable_map[0] = '\n'; // first empty line (padding)
  printable_map[0] = ' ';  // first space
  printable_map[1] = ' ';  // first space
  printable_map[2] = ' ';  // first space
  int i = 0;
  for (i; i < MAP_SIZE; i++) {
    if (i == player.xy) { // if the player orientation is reached
      printable_map[TO_PRINTABLE_MAP1 * i + TO_PRINTABLE_MAP2] = player.pos;
      player.char_at_pos = map_structure[i];
    } else {
      if (map_structure[i] == '\n') {
        printable_map[TO_PRINTABLE_MAP1 * i + TO_PRINTABLE_MAP2] = ' ';
      } else {
        printable_map[TO_PRINTABLE_MAP1 * i + TO_PRINTABLE_MAP2] = map_structure[i];
      }
    }
    printable_map[TO_PRINTABLE_MAP1 * i + TO_PRINTABLE_MAP2 + 1] = ' ';
  }
  printable_map[PRINTABLE_MAP_SIZE] = '\0';
}

void printArray(char *array) {
  char last_char = ' ';
  addch('\n');
  for (int i = 0; i < strlen(array); i++) {
    if (last_char != array[i]) {
      switch (array[i]) {
        case '@':
          setColor(TREE_COLOR);
          break;
        case '~':
          setColor(WATER_COLOR);
          break;
        case 'W':
          setColor(GRASS_COLOR);
          break;
        case 'o':
          setColor(POKEBALL_COLOR);
          break;
        case '0':
          setColor(STONE_COLOR);
          break;
        case 'O':
          setColor(PILLAR_COLOR);
          break;
        case '[':
          setColor(DOOR_COLOR);
          break;
        case ']':
          setColor(DOOR_COLOR);
          break;
        case '_':
          setColor(DOOR_COLOR);
          break;
        case 'T':
          setColor(CUTABLE_TREE_COLOR);
          break;
        case '#':
          setColor(HEAL_COLOR);
          break;

        default:
          setColor(CLASSIC_COLOR);
          break;
      }
    }
    if (i % 30 == 29) {
      addch('\n');
    }
    if (array[i] != 13) {
      addch(array[i]);
    }
    last_char = array[i];
  }
  setColor(CLASSIC_COLOR);
}

/* Prints the map on the console
 * printable_map : the array representing a printable map
 * clear_db : true if you want to clear the dialog box, false otherwise
 */
void clearAndPrintMap(char *printable_map, char *dialog_box) {
  clearConsole();
  // //tty_reset();
  printArray(printable_map);
  printw("\n");
  printDialogBox(dialog_box);
  //setRawMode('1');
}

bool isObstacle(char char_at_new_xy) {
  int obstacle_count = 13;
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
  obstacles[9] = PLAYER_N;
  obstacles[10] = PLAYER_S;
  obstacles[11] = PLAYER_E;
  obstacles[12] = PLAYER_W;
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

void clearW8NErase(char *printable_map, char *dialog_box) {
  clearAndPrintMap(printable_map, dialog_box);
  enterKey();
  eraseDialogBoxLines(dialog_box);
}

/* Moves the player
 * player : the player
 * new_pos : the new orientation of the player
 * xy_sub : the new position of the player
 * printable_map : the printable map
 */
void movePlayer(Player *player, char new_pos, int xy_sub, Map *map) {
  player->pos = new_pos;
  player->xy = player->xy - xy_sub;
  char char_at_new_xy =
      map->printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2];
  if (isEqual(char_at_new_xy, HEAL)) {
    resetAllPokemonsStats(player);
    addTextInDialogBox(FRST_LINE_START, HEAL_TEXT, HEAL_TEXT_LGTH,
                       map->dialog_box);
  }
  if (isEqual(char_at_new_xy, SHOP)) {
    manageShopMenu(player);
  }
  if (isObstacle(char_at_new_xy)) {
    player->xy = player->xy + xy_sub;
  } else {
    player->char_at_pos = char_at_new_xy;
    isBattle(player, map);
    handlePnjBattle(map, player);
  }
}

/* Changes the map
 * player : the player
 * x_map_sub : the value to sub to the x new map coordinate
 * y_map_sub : the value to sub to the y new map coordinate
 * new_pos : the new player's orientation
 * xy_player_sub : the value to add to the player xy position
 * map_structure :the array where to load the new map
 * map : the map
 */
void changeMap(Player *player, int x_map_sub, int y_map_sub, char new_pos,
               int xy_player_sub, char *map_structure, Map *map) {
  // set the current map coordinates to the new map ones
  map->x = map->x - x_map_sub;
  map->y = map->y - y_map_sub;

  // change the player data
  player->pos = new_pos;
  player->xy = player->xy - xy_player_sub;

  loadMap(map, map_structure, player);

  // update the char placed originally at the player position
  char char_at_new_xy = map_structure[player->xy];
  player->char_at_pos = char_at_new_xy;

  createPrintableMap(map->printable_map, map_structure, *player);
  isBattle(player, map);
}

void comeBackFirstMap(Player *player, Map *map) {
  char map_structure[MAP_SIZE];
  map->x = 0;
  map->y = 0;
  changeMap(player, 0, 0, PLAYER_N, player->xy - PLAYER_START_POS,
            map_structure, map);
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
  int text_length = 19 + item.name_length; // 19 = 'Vous avez trouve : ' length
  char *text = malloc(sizeof(char) * text_length + 1);
  sprintf(text, "Vous avez trouve : %s", item.name);
  addTextInDialogBox(FRST_LINE_START, text, text_length, dialog_box);
}

void manageItemFound(Player *player, char *dialog_box, int *x_map, int *y_map) {
  int item_id = getMapItemInPokeballID(x_map, y_map);
  // There are more than one pokeball in the 6;-1 map,
  // so this set the content of others pokeballs to Pokeball
  if (item_id == CRISTAL_KEY_ID) {
    if (possessBagItem(player, item_id) != -1) {
      item_id = POKEBALL_ID;
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
    getUserInput(&key_pressed);
    key_pressed_status = manageYesNoKeyPressed(key_pressed, yes);
    if (key_pressed_status == 1) {
      setDBArrowYesNo(yes, dialog_box);
      clearAndPrintMap(printable_map, dialog_box);
    }
  } while (key_pressed_status != 0); //!= enter
}

void manageDestroy(Player *player, char *dialog_box, char *printable_map,
                   char destroyed_object, int xy_ifo_player) {
  int item_id = PIOCHE_ID;
  if (destroyed_object == CUTABLE_TREE) {
    item_id = CISAILLE_ID;
  }
  if (destroyed_object == CUTABLE_TREE) {
    addTextInDialogBox(FRST_LINE_START, "Cet arbre semble vieux et fragile", 33,
                       dialog_box);
  } else {
    addTextInDialogBox(FRST_LINE_START, "Ce rocher ne semble pas tres solide",
                       35, dialog_box);
  }
  if (possessBagItem(player, item_id) != -1) {
    showDBYesNoMenu(dialog_box);
    if (destroyed_object == CUTABLE_TREE) {
      addTextInDialogBox(SCD_LINE_START,
                         "Voulez-vous le couper avec votre Cisaille ?", 43,
                         dialog_box);
    } else {
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

void fishing(Player *player, Map *map) {
  srand(time(NULL));
  int r = rand() % 100;
  if (r < 5 && itemCount(player, KEY_FRAG_ID) == 4) {
    addBagItemPlayer(player, KEY_FRAG_ID, 1);
    addTextInDialogBox(FRST_LINE_START,
                       "Enorme prise ! Vous recevez un Fragement de Clef !", 50,
                       map->dialog_box);
  } else if (r < 10) {
    addBagItemPlayer(player, POTION_ID, 1);
    addTextInDialogBox(FRST_LINE_START,
                       "Cela a mordu ! Vous recevez une Potion !", 40,
                       map->dialog_box);
  } else if (r < 50) {
    goForBattle(player, map, NORMAL_BATTLE);
  } else {
    addTextInDialogBox(FRST_LINE_START, "Cela ne mord pas...", 19,
                       map->dialog_box);
  }
  clearAndPrintMap(map->printable_map, map->dialog_box);
}

void manageFishing(Player *player, Map *map) {
  if (possessBagItem(player, CANNE_ID) != -1) {
    showDBYesNoMenu(map->dialog_box);
    addTextInDialogBox(FRST_LINE_START, "Cette eau est d'un bleu etincellant",
                       35, map->dialog_box);
    addTextInDialogBox(SCD_LINE_START, "Voulez-vous pecher ?", 20,
                       map->dialog_box);
    clearAndPrintMap(map->printable_map, map->dialog_box);
    int yes = 1;
    manageDBYesNoMenu(&yes, map->printable_map, map->dialog_box);
    eraseDialogBoxLines(map->dialog_box);
    if (yes == 1) {
      fishing(player, map);
    }
  } else {
    addTextInDialogBox(FRST_LINE_START,
                       "Vous appercevez des silhouettes dans l'eau", 42,
                       map->dialog_box);
  }
  clearAndPrintMap(map->printable_map, map->dialog_box);
}

void pickPokeball(Player *player, Map *map, int xy_ifo_player) {
  if (map->x == 2 && map->y == -3) { // trapped pokeball
    bool lost = goForBattle(player, map, HARD_BATTLE);
    if (!lost) {
      addTextInDialogBox(FRST_LINE_START,
                         "Le Pokemon s'enfuit en laissant une clef par terre.",
                         51, map->dialog_box);
      addTextInDialogBox(SCD_LINE_START,
                         "Bravo, vous avez obtenu une Clef de Diamand !", 45,
                         map->dialog_box);
      addBagItemPlayer(player, DIAMOND_KEY_ID, 1);
      clearAndPrintMap(map->printable_map, map->dialog_box);
      MapSquare square = {map->x, map->y,
                          (xy_ifo_player - TO_PRINTABLE_MAP2) / 2};
      addPickedItemPlayer(player, square);
      map->printable_map[xy_ifo_player] = ' ';
    } else {
      eraseDialogBoxLines(map->dialog_box);
    }
  } else {
    MapSquare square = {map->x, map->y,
                        (xy_ifo_player - TO_PRINTABLE_MAP2) / 2};
    addPickedItemPlayer(player, square);
    map->printable_map[xy_ifo_player] = ' ';
    manageItemFound(player, map->dialog_box, &map->x, &map->y);
  }
}

/* Check if an interacion is possible
 * player : the player
 * map : the map
 * return 2 to clear and print back the map, 0 to do nothing
 */
int checkIfInteractionPossible(Player *player, Map *map) {
  map->printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
      player->pos;
  int interaction_found = 2; // clear and print
  int xy_ifo_player = getXYIfoPlayer(player);
  char char_ifo_player = map->printable_map[xy_ifo_player];
  switch (char_ifo_player) {
  case POKEBALL:
    pickPokeball(player, map, xy_ifo_player);
    break;

  case STONE:
    manageDestroy(player, map->dialog_box, map->printable_map, char_ifo_player,
                  xy_ifo_player);
    break;

  case CUTABLE_TREE:
    manageDestroy(player, map->dialog_box, map->printable_map, char_ifo_player,
                  xy_ifo_player);
    break;

  case DOOR:
    manageDoorOpenning(player, map->dialog_box, map->printable_map);
    break;

  case WATER_:
    manageFishing(player, map);
    break;

  case PLAYER_N: // or
  case PLAYER_S: // or
  case PLAYER_E: // or
  case PLAYER_W:
    managePnjOrientation(map, player->pos, xy_ifo_player);
    if (pnjWantsBattle(map, player)) {
      pnjBattle(map, player);
    } else {
      pnjDialog(map->printable_map, map->dialog_box, &map->pnj);
    }
    break;

  default:
    interaction_found = 0; // do not clear and print
    break;
  }
  map->printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
      player->char_at_pos;
  return interaction_found;
}

/* Manages the key pressed by the user
 * key_pressed : the key pressed
 * player : the player
 * map : the map
 * return true if a right key has been pressed, false otherwise
 */
int manageKeyPressed(char key_pressed, Player *player, Map *map) {
  int key_status = 1; // arrow moved
  char map_structure[MAP_SIZE];
  map->printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
      player->char_at_pos;

  if (key_pressed == MOVE_Z) {
    if (player->xy > 0 && player->xy < LINE_SEPARATOR) {
      changeMap(player, 0, -1, PLAYER_N, -(11 * LINE_SEPARATOR), map_structure,
                map);
    } else {
      movePlayer(player, PLAYER_N, LINE_SEPARATOR, map);
    }
  } else if (key_pressed == MOVE_Q) {
    if (player->xy % LINE_SEPARATOR == 0) {
      changeMap(player, 1, 0, PLAYER_W, -(LINE_SEPARATOR - 3), map_structure,
                map);
    } else {
      movePlayer(player, PLAYER_W, 1, map);
    }
  } else if (key_pressed == MOVE_S) {
    if (player->xy > (MAP_SIZE - LINE_SEPARATOR) && player->xy < MAP_SIZE) {
      changeMap(player, 0, 1, PLAYER_S, 11 * LINE_SEPARATOR, map_structure,
                map);
    } else {
      movePlayer(player, PLAYER_S, -(LINE_SEPARATOR), map);
    }
  } else if (key_pressed == MOVE_D) {
    if (player->xy % LINE_SEPARATOR == 12) {
      changeMap(player, -1, 0, PLAYER_E, LINE_SEPARATOR - 3, map_structure,
                map);
    } else {
      movePlayer(player, PLAYER_E, -1, map);
    }
  } else if (key_pressed == ENTER) {
    key_status = checkIfInteractionPossible(
        player, map); // 2 -> interaction found / 0 otherwise
  } else if (key_pressed == EXIT) {
    exit(0);
  } else if (key_pressed == ' ') {
    key_status = 4; // start menu
  } else if (key_pressed != 0) {
    key_status = 0; // wrong key
  }

  map->printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
      player->pos;

  if (key_status == 4) { // start menu pressed
    createStartMenu(map->dialog_box);
    clearAndPrintMap(map->printable_map, map->dialog_box);
    key_status = manageStartMenu(player, map->printable_map, map->dialog_box);
  }

  return key_status;
}

bool isMapSquareEqual(MapSquare ms1, MapSquare ms2) {
  return ms1.x_map == ms2.x_map && ms1.y_map == ms2.y_map && ms1.xy == ms2.xy;
}
