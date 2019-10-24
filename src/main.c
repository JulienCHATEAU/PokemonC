#include "accountManager.h"
#include "battlePane.h"
#include "map.h"
#include "pnj.h"
#include "pokemon.h"
#include "pokemonPane.h"
#include "print.h"
#include "ttyraw.h"
#include "util.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  if (*argv[1] == '0') {
    setRawMode('0');
    exit(0);
  }
  managePokemonPaneMenu();
  char pseudo[PSEUDO_MAX_LENGTH];
  manageConnexionMenu(pseudo);
  if (argc > 1) {
    if (*argv[1] == '1') {
      setRawMode(*argv[1]);
    } else {
      fprintf(stderr, "Wrong parameters !\n");
      exit(3);
    }
  } else {
    setRawMode('0');
  }
  srand(time(NULL));
  Map map;
  map.x = 0;
  map.y = 0;
  map.printable_map = malloc(sizeof(char) * PRINTABLE_MAP_SIZE);
  map.dialog_box = malloc(sizeof(char) * DIALOG_BOX_LENGTH);
  Player player;
  setPlayerPseudo(pseudo, &player);
  // setPlayerPseudo("admin", &player);
  loadPlayerData(&map.x, &map.y, &player);
  printf("%d;%d", map.x, map.y);
  int xy_temp = player.xy; // to check if the player has moved or not
  int pos_temp =
      player.pos; // to check if the player has changed his orientation or not
  bool first_print = true;
  char key_pressed = 0;
  char map_structure[MAP_SIZE];
  loadMap(&map, map_structure, &player);
  loadDialogBox(map.dialog_box);
  createPrintableMap(map.printable_map, map_structure, player);
  int key_status;
  clearAndPrintMap(map.printable_map, map.dialog_box);
  do {
    key_pressed = getchar();
    key_status = manageKeyPressed(key_pressed, &player, &map);
    if (key_status != 0) { // right key
      /*this test optimize the refresh of the console :
      when the user keep going into a tree -> don't need to refresh
      when the user press enter but no interaction possible ->  don't need to
      refresh*/
      if (key_status == 2 || xy_temp != player.xy ||
          (xy_temp == player.xy && pos_temp != player.pos) ||
          first_print) { // key_status == 2 means an interacion happened
        clearAndPrintMap(map.printable_map, map.dialog_box);
        first_print = false;
      }
      xy_temp = player.xy;
      pos_temp = player.pos;
    }
    eraseDialogBoxLines(map.dialog_box);
  } while (key_status != EXIT);
  savePlayerData(map.x, map.y, &player);
  return 0;
}

// int main(int argc, char const *argv[]) {
//   int x_map = 0;
//   int y_map = 0;
//   Player player;
//   char map_structure[MAP_SIZE];
//   setPlayerPseudo("admin", &player);
//   loadPlayerData(&x_map, &y_map, &player);
//   loadMap(x_map, y_map, map_structure, &player);
//   // for (int i = 0; i < MAP_SIZE; i++) {
//   //   printf("%d\n", map_structure[i]);
//   // }
//   char dialog_box[DIALOG_BOX_LENGTH];
//   loadDialogBox(dialog_box);
//   char printable_map[PRINTABLE_MAP_SIZE];
//   createPrintableMap(printable_map, map_structure, player);
//   int key_status;
//   clearAndPrintMap(printable_map, dialog_box);
//   return 0;
// }