#include "util.h"
#include "accountManager.h"
#include "bag.h"
#include "fileManager.h"
#include "map.h"
#include "pnj.h"
#include "pokemon.h"
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>

/* Waits during n millisecond
 * n : the number of millisecond to wait
 */
void waitNMs(int n) { /*n millisecond*/
  refresh();
  usleep(n * 1000);
  // int msec = 0;
  // clock_t before = clock();
  // do {
  //   clock_t difference = clock() - before;
  //   msec = difference * 1000 / CLOCKS_PER_SEC;
  // } while (msec < n);
}

void getUserInput(char *key_pressed) {
  noecho();
  do {
    *key_pressed = getch();
    if (*key_pressed > 0) {
      break;
    }
  } while (1);
}

void textColor(int attr, int fg) {
  char command[13];
  /* Command is the control command to the terminal */
  sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, BLACK + 40);
  printw("%s", command);
}

/* Checks if a character is equal to another
 * return : true if the character is a equal to the other one, false otherwise
 */
bool isEqual(char character, char comp) { return character == comp; }

/* Adds the menu arrow
 * arrow_xy : the arrow's corrdinates
 * battle_pane : the battle pane
 */
void addArrow(int arrow_xy, char *battle_pane) {
  if (arrow_xy != 0) { // if !HIDDEN
    addStringToArray("->", 2, arrow_xy, battle_pane);
  }
}

/* Removes the menu arrow
 * arrow_xy : the arrow's corrdinates
 * battle_pane : the battle pane
 */
void removeArrow(int arrow_xy, char *battle_pane) {
  addStringToArray("  ", 2, arrow_xy, battle_pane);
}

/* Shifts the pipes used on the left of the menu to adapt to the skill menu
* battle_pane : the battle pane
* top_pipe_xy_to_add : the xy coordinates of the pipe at the top to add
* top_pipe_xy_to_erase : the xy coordinates of the pipe at the top to erase
* next_line : the number of character to skip a line
* pipe_count : the pipe count to shift
//these cursors are used to know where to place the different parts of a line
one by one
*/
void shiftMenuPipes(char *battle_pane, int top_pipe_xy_to_add,
                    int top_pipe_xy_to_erase, int next_line, int pipe_count) {
  battle_pane[top_pipe_xy_to_erase] = ' ';
  battle_pane[top_pipe_xy_to_erase + (pipe_count - 1) * next_line] = '_';

  for (int i = 0; i < pipe_count; i++) {
    battle_pane[top_pipe_xy_to_add + i * next_line] = '|';
  }
}

void copyMapSquare(MapSquare tmp, MapSquare *item) {
  item->x_map = tmp.x_map;
  item->y_map = tmp.y_map;
  item->xy = tmp.xy;
}

void addPickedItemPlayer(Player *player, MapSquare square) {
  copyMapSquare(square, &player->item_picked[player->item_picked_count]);
  player->item_picked_count++;
}

bool containsPickedItem(Player *player, MapSquare picked_item,
                        MapSquare *item) {
  MapSquare tmp;
  bool trouve = false;
  int i = 0;
  while (i < player->item_picked_count) {
    tmp = player->item_picked[i];
    if (isMapSquareEqual(picked_item, tmp)) {
      copyMapSquare(tmp, item);
      trouve = true;
      break;
    }
    i++;
  }
  return trouve;
}

/* Adds a string in an array starting from a point
 * string : the string to add
 * string_length : the length of the string
 * start : the starting index in the array where to add the string
 * array : the array
 */
void addStringToArray(char *string, int string_length, int start, char *array) {
  for (int i = 0; i < string_length; i++) {
    array[start + i] = string[i];
  }
}

/* Checks if an array contains an integer
 * tab : the array
 * count : the number of elements in the array
 * elem : the integer to find
 */
bool contains(int *tab, int count, int elem) {
  bool trouve = false;
  for (int i = 0; i < count && !trouve; i++) {
    if (tab[i] == elem) {
      trouve = true;
    }
  }
  return trouve;
}

/* Copy a string into a char array
 * tab : the destination
 * string : the source
 */
void setStringToTab(char *tab, char *string) {
  int i = 0;
  bool stop = false;
  while (!stop) {
    tab[i] = string[i];
    if (string[i] == '\0') {
      stop = true;
    }
    i++;
  }
}

/* Instantiates an Effect structure
 * stat_to_change : an integer representing a StatEnum
 * change_value : the value describing how much the stat is changing
 * accuracy : the accuracy of the effect
 */
Effect createEffect(int stat_to_change, int change_value, int accuracy) {
  Effect e = {stat_to_change, change_value, accuracy};
  return e;
}

/* Instantiates a Stats structure
 * hp_max : the stats hp_max value
 * att_max : the stats att_max value
 * def_max : the stats def_max value
 * spd_max :the stats spd_max value
 */
Stats createStats(int hp_max, int att_max, int def_max, int spd_max) {
  Stats stats = {hp_max,  hp_max,  att_max, att_max,
                 def_max, def_max, spd_max, spd_max};
  return stats;
}

/* Sets the pseudo of the player
 * pseudo : the pseudo
 * player : the player
 */
void setPlayerPseudo(char *pseudo, Player *player) {
  int i = 0;
  while (pseudo[i] != '\0') {
    player->pseudo[i] = pseudo[i];
    i++;
  }
  player->pseudo[i] = '\0';
  player->pseudo_length = i;
}

/* Instantiates a Player structure
 * pseudo : the player's pseudo
 * xy : the player's position in the map
 * pos : the player's orientation
 * pkmn : the player's pokemon
 * return a Player structure
 */
Player createPlayer(char *pseudo, int xy, char pos, char *pkmn_name) {
  Player p;
  p.xy = xy;
  p.pos = pos;
  p.char_at_pos = ' ';
  p.pkmns[0] = fillPokemon(pkmn_name);
  setPokemonLvl(&p.pkmns[0], 5);
  p.pkmn_count = 1;
  p.money = 0;
  initBag(&p);
  p.bag_item_count = 0;
  p.item_picked_count = 0;
  initPikedItem(&p);
  setPlayerPseudo(pseudo, &p);
  return p;
}

void initColors() {
  init_pair(CLASSIC_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(POKEMON_PANE_COLOR, 226, COLOR_BLACK);
  init_pair(TREE_COLOR, 28, COLOR_BLACK);
  init_pair(GRASS_COLOR, 46, COLOR_BLACK);
  init_pair(WATER_COLOR, 33, COLOR_BLACK);
  init_pair(POKEBALL_COLOR, 9, COLOR_BLACK);
  init_pair(HEAL_COLOR, 200, COLOR_BLACK);
  init_pair(CUTABLE_TREE_COLOR, 70, COLOR_BLACK);
  init_pair(DOOR_COLOR, 101, COLOR_BLACK);
  init_pair(STONE_COLOR, 130, COLOR_BLACK);
  init_pair(PILLAR_COLOR, 215, COLOR_BLACK);
}

void setColor(int colorId) {
  attron(COLOR_PAIR(colorId));
}

void enterKey() {
  char key_pressed;
  noecho();
  do {
      key_pressed = getch();
      if (key_pressed == ENTER) {
        break;
      }
    } while (1);
  echo();
}

void getSpacesString(char *string, int length) {
  int i = 0;
  for (i; i < length; ++i) {
    string[i] = ' ';
  }
  string[i] = '\0';
}

void copyEffect(Effect effect, Effect *copy) {
  copy->stat_to_change = effect.stat_to_change;
  copy->change_value = effect.change_value;
  copy->accuracy = effect.accuracy;
  copy->target = effect.target;
}

void copySkill(Skill skill, Skill *copy) {
  copy->name_length = skill.name_length;
  copy->name = malloc(sizeof(char) * copy->name_length + 1);
  strcpy(copy->name, skill.name);
  copy->power = skill.power;
  copy->accuracy_max = skill.accuracy_max;
  copy->accuracy = skill.accuracy;
  copy->pp_max = skill.pp_max;
  copy->pp = skill.pp;
  copy->type = skill.type;
  copy->priority = skill.priority;
  copyEffect(skill.effect, &copy->effect);
  copy->ailment.ailment_enum = skill.ailment.ailment_enum;
  copy->ailment.accuracy = skill.ailment.accuracy;
  copy->description_length = skill.description_length;
  copy->description = malloc(sizeof(char) * copy->description_length + 1);
  strcpy(copy->description, skill.description);
  copy->id = skill.id;
}

int getColumn(int xy) { return xy % LINE_SEPARATOR; }

int getLine(int xy) { return xy / LINE_SEPARATOR; }

bool isVertical(int xy, int xy2) { return getColumn(xy) == getColumn(xy2); }

bool isHorizontal(int xy, int xy2) { return getLine(xy) == getLine(xy2); }
