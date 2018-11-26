#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "pokemon.h"
#include "accountManager.h"
#include "fileManager.h"
#include "util.h"
#include "map.h"
#include "bag.h"

/* Waits during n millisecond
* n : the number of millisecond to wait
*/
void waitNMs(int n) {/*n millisecond*/
  int msec = 0;
  clock_t before = clock();
  do {
    clock_t difference = clock() - before;
    msec = difference * 1000 / CLOCKS_PER_SEC;
  } while ( msec < n );
}

void textColor(int attr, int fg) {
  char command[13];
	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, BLACK + 40);
	printf("%s", command);
}

/* Checks if a character is equal to another
* return : true if the character is a equal to the other one, false otherwise
*/
bool isEqual(char character, char comp) {
  return character == comp;
}

/* Adds the menu arrow
* arrow_xy : the arrow's corrdinates
* battle_pane : the battle pane
*/
void addArrow(int arrow_xy, char *battle_pane) {
  if (arrow_xy != 0) {//if !HIDDEN
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
//these cursors are used to know where to place the different parts of a line one by one
*/
void shiftMenuPipes(char *battle_pane, int top_pipe_xy_to_add, int top_pipe_xy_to_erase, int next_line, int pipe_count) {
  battle_pane[top_pipe_xy_to_erase] = ' ';
  battle_pane[top_pipe_xy_to_erase + (pipe_count-1)*next_line] = '_';

  for (int i = 0; i < pipe_count; i++) {
    battle_pane[top_pipe_xy_to_add + i*next_line] = '|';
  }
}

/* Adds a string in an array starting from a point
* string : the string to add
* string_length : the length of the string
* start : the starting index in the array where to add the string
* array : the array
*/
void addStringToArray(char *string, int string_length, int start, char *array) {
  for (int i = 0; i < string_length; i++) {
    array[start+i] = string[i];
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
  Stats stats = {hp_max, hp_max, att_max, att_max, def_max, def_max, spd_max, spd_max};
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
  setPlayerPseudo(pseudo, &p);
  return p;
}

/* Frees a Skill structure
* skill : the Skill structure to free
*/
void freeSkill(Skill skill) {
  free(skill.name);
  free(skill.description);
}

/* Frees a Player structure
* player : the Player structure to free
*/
void freePlayer(Player player) {
  for (int i = 0; i < player.pkmn_count; i++) {
    freePokemon(player.pkmns[i]);
  }
}

/* Frees a Pokemon structure
* pkmn : the Pokemon structure to free
*/
void freePokemon(Pokemon pkmn) {
  free(pkmn.name);
  for (size_t i = 0; i < 4; i++) {
    freeSkill(pkmn.skills[i]);
  }
}

void copyEffect(Effect effect, Effect *copy) {
 copy->stat_to_change = effect.stat_to_change;
 copy->change_value = effect.change_value;
 copy->accuracy = effect.accuracy;
 copy->target = effect.target;
}

void copySkill(Skill skill, Skill *copy) {
  copy->name_length = skill.name_length;
  copy->name = malloc(sizeof(char)*copy->name_length+1);
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
  copy->description = malloc(sizeof(char)*copy->description_length+1);
  strcpy(copy->description, skill.description);
  copy->id = skill.id;
}
