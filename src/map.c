#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pokemon.h"
#include "util.h"
#include "bag.h"
#include "map.h"
#include "accountManager.h"
#include "startMenu.h"
#include "battle.h"
#include "ttyraw.h"
#include "fileManager.h"
#include "print.h"

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
}

/* Gets the xy coordinates of the square ifo the player
* player : the player
*/
int getXYIfoPlayer(Player *player) {
  int xy_in_front_of_player = 0;
  if (player->pos == PLAYER_N) {
    xy_in_front_of_player = player->xy*2+3-LINE_SEPARATOR*2;
  } else if (player->pos == PLAYER_S) {
    xy_in_front_of_player = player->xy*2+3+LINE_SEPARATOR*2;
  } else if (player->pos == PLAYER_E) {
    xy_in_front_of_player = player->xy*2+5;
  } else if (player->pos == PLAYER_W) {
    xy_in_front_of_player = player->xy*2+1;
  }
  return xy_in_front_of_player;
}

/* Removes an interactive object from the map
* x_map : the x coordinate of the current map
* y_map : the y coordinate of the current map
* player : the player
* map_structure : an array storing the map
*/
void removeInteractiveObject(int x_map, int y_map, Player *player, char *map_structure) {
  FILE *save_file = openPlayerSaveFile(player, "r");
  int scan_status = 0;
  skipPlayerData(save_file);
  int x_map_tmp;
  int y_map_tmp;
  int xy_tmp;
  while (scan_status != EOF) {
    scan_status = fscanf(save_file, "%d;%d %d\n", &x_map_tmp, &y_map_tmp, &xy_tmp);
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
void createPrintableMap(char *printable_map, char *map_structure, Player player) {
  printable_map[0] = '\n';//first empty line (padding)
  printable_map[1] = ' ';//first space
  printable_map[2] = ' ';//second space
  int i = 0;
  for (i; i < MAP_SIZE; i++) {
    if (i == player.xy) {//if the player orientation is reached
      printable_map[2*i+3] = player.pos;
      player.char_at_pos = map_structure[i];
    } else {
      printable_map[2*i+3] = map_structure[i];
    }
    printable_map[2*i+4] = ' ';
  }
}

void printChaine(char *chaine) {
  for (int i = 0; i < strlen(chaine); i++) {
    switch (chaine[i]) {
      case '@':textColor(DIM, GREEN);break;
      case '~':textColor(BRIGHT, BLUE);break;
      case 'W':textColor(BRIGHT, GREEN);break;
      case 'o':textColor(BRIGHT, RED);break;
      case '0':textColor(DIM, YELLOW);break;
      case 'T':textColor(DIM, GREEN);break;
      case '#':textColor(DIM, MAGENTA);break;
      default:textColor(RESET, WHITE);break;
    }
    printf("%c", chaine[i]);
    textColor(RESET, WHITE);
  }
}


/* Prints the map on the console
* printable_map : the array representing a printable map
* clear_db : true if you want to clear the dialog box, false otherwise
*/
void clearAndPrintMap(char *printable_map, char *dialog_box) {
  clearConsole();
  printChaine(printable_map);
  printf("\n");
  tty_reset();
  printDialogBox(dialog_box);
  setRawMode('1');
}


/* Moves the player
* player : the player
* new_pos : the new orientation of the player
* xy_sub : the new position of the player
* printable_map : the printable map
*/
void movePlayer(Player *player, char new_pos, int xy_sub, char *printable_map, char *dialog_box) {
  player->pos = new_pos;
  player->xy = player->xy - xy_sub;
  char char_at_new_xy = printable_map[2*player->xy+3];
  if (isEqual(char_at_new_xy, HEAL)) {
    resetAllPokemonsStats(player);
    addTextInDialogBox(FRST_LINE_START, HEAL_TEXT, HEAL_TEXT_LGTH, dialog_box);
  }
  if (isEqual(char_at_new_xy, TREE) || isEqual(char_at_new_xy, POKEBALL) || isEqual(char_at_new_xy, CUTABLE_TREE) || isEqual(char_at_new_xy, STONE) || isEqual(char_at_new_xy, WATER)) {
    player->xy = player->xy + xy_sub;
  } else {
    player->char_at_pos = char_at_new_xy;
    isBattle(player);
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
void changeMap(Player *player, int *x_map, int *y_map, int x_map_sub, int y_map_sub, char new_pos, int xy_player_sub, char *printable_map, char *map_structure) {
  //set the current map coordinates to the new map ones
  *x_map = *x_map - x_map_sub;
  *y_map = *y_map - y_map_sub;

  //change the player data
  player->pos = new_pos;
  player->xy = player->xy - xy_player_sub;

  loadMap(*x_map, *y_map, map_structure, player);

  //update the char placed originally at the player position
  char char_at_new_xy = map_structure[player->xy];
  player->char_at_pos = char_at_new_xy;

  createPrintableMap(printable_map, map_structure, *player);
  isBattle(player);
}

/* Save the xy coordinate of an interactive object the player has taken to remember is was taken
* player : the player
* x_map : the x coordinate of the current map
* y_map : the y coordinate of the current map
* xy : xy coordinate of an interactive object
*/
void saveMapSpecificData(Player *player, int x_map, int y_map, int xy) {
  FILE *save_file = openPlayerSaveFile(player, "a");
  fprintf(save_file, "%d;%d %d\n", x_map, y_map, xy);
  closeFile(save_file);
}

/* Check if an interacion is possible
* player : the player
* printable_map : the printable map
* dialog_box : the dialog box
* x_map : the x coordinate of the current map
* y_map : the y coordinate of the current map
* return 2 to clear and print back the map, 0 to do nothing
*/
int checkIfInteractionPossible(Player *player, char *printable_map, char *dialog_box, int *x_map, int *y_map) {
  int interaction_found = 2;//clear and print
  int xy_ifo_player = getXYIfoPlayer(player);
  char char_ifo_player = printable_map[xy_ifo_player];
  if (char_ifo_player == POKEBALL) {
    saveMapSpecificData(player, *x_map, *y_map, (xy_ifo_player-3)/2);
    addBagItemPlayer(player, 0, 1);//pokeball
    printable_map[xy_ifo_player] = ' ';
    addTextInDialogBox(FRST_LINE_START, POKEBALL_FOUND, POKEBALL_FOUND_LGTH, dialog_box);
  } else {
    interaction_found = 0;//do not clear and print
  }
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
int manageKeyPressed(char key_pressed, Player *player, char *dialog_box, char *printable_map, int *x_map, int *y_map) {
  int key_status = 1;//arrow moved
  char map_structure[MAP_SIZE];
  printable_map[2*player->xy+3] = player->char_at_pos;

  if (key_pressed == MOVE_Z) {
    if (player->xy > 0 && player->xy < LINE_SEPARATOR) {
      changeMap(player, x_map, y_map, 0, -1, PLAYER_N, -(MAP_SIZE - LINE_SEPARATOR + 2), printable_map, map_structure);
    } else {
      movePlayer(player, PLAYER_N, LINE_SEPARATOR, printable_map, dialog_box);
    }
  } else if (key_pressed == MOVE_Q) {
    if (player->xy % LINE_SEPARATOR == 0) {
      changeMap(player, x_map, y_map, 1, 0, PLAYER_W, -(LINE_SEPARATOR - 3), printable_map, map_structure);
    } else {
      movePlayer(player, PLAYER_W, 1, printable_map, dialog_box);
    }
  } else if (key_pressed == MOVE_S) {
    if (player->xy < MAP_SIZE && player->xy > (MAP_SIZE-LINE_SEPARATOR)) {
      changeMap(player, x_map, y_map, 0, 1, PLAYER_S, MAP_SIZE-LINE_SEPARATOR+2, printable_map, map_structure);
    } else {
      movePlayer(player, PLAYER_S, -(LINE_SEPARATOR), printable_map, dialog_box);
    }
  } else if (key_pressed == MOVE_D) {
    if (player->xy % LINE_SEPARATOR == 12) {
      changeMap(player, x_map, y_map, -1, 0, PLAYER_E, LINE_SEPARATOR-3, printable_map, map_structure);
    } else {
      movePlayer(player, PLAYER_E, -1, printable_map, dialog_box);
    }
  } else if (key_pressed == 13) {
    key_status = checkIfInteractionPossible(player, printable_map, dialog_box, x_map, y_map);//2 -> interaction found / 0 otherwise
  } else if (key_pressed == EXIT) {
    exit(0);
  } else if (key_pressed == ' ') {
    key_status = 4;//start menu
  } else if (key_pressed != 0) {
    key_status = 0;//wrong key
  }

  printable_map[2*player->xy+3] = player->pos;

  if (key_status == 4) {//start menu pressed
    createStartMenu(dialog_box);
    clearAndPrintMap(printable_map, dialog_box);
    key_status = manageStartMenu(player, printable_map, dialog_box);
  }

  return key_status;
}

