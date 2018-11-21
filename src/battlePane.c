#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "pokemon.h"
#include "util.h"
#include "map.h"
#include "fileManager.h"
#include "ttyraw.h"
#include "battlePane.h"
#include "startMenu.h"
#include "print.h"
#include "battle.h"


/* Creates a thread and wait for its ending
* function : the function that the thread will execute
* function_params : the parameters of the function
*/
void createThreadNW8(void *function, void *function_params) {
  pthread_t thread;
  if (pthread_create(&thread, NULL, function, function_params)) {
    perror("pthread_create");
    exit(1);
  }
  if (pthread_join(thread, NULL)) {
    perror("pthread_join");
    exit(2);
  }
}

/* Counts the number of digits in an integer
* n : the integer
* return the number of digits in this integer
*/
int nDigits(unsigned int n) {
  int ret = 0;
  do {
    n /= 10;
    ret++;
  } while (n > 0);
  return ret;
}

/* Checks if the player still has any alive pokemon
* player : the player
* return true if the player still has any alive pokemon, false otherwise
*/
bool hasAnyAlivePokemon(Player *player) {
  bool any_alive = false;
  int i = 0;
  while (i < player->pkmn_count && !any_alive) {
    if (player->pkmns[i].stats.hp > 0) {
      any_alive = true;
    }
    i++;
  }
  return any_alive;
}

/* Clears and prints the battle pane
* battle_pane : the battle pane
*/
void clearAndPrintBattlePane(char *battle_pane) {
  clearConsole();
  printBattlePane(battle_pane);
}

/* Erases a line of the battle pane
* pos : a coordinate on the right from the beggining of the line
* battle_pane : the battle pane
* space_count : the size of the erase
*/
void eraseArrayLine(int pos, char *battle_pane, int space_count) {
  if (space_count < 0) {
    for (int i = 0; i < -space_count; i++) {
      battle_pane[pos+i] = ' ';
    }
  } else {
    for (int i = 0; i < space_count; i++) {
      battle_pane[pos-i] = ' ';
    }
  }
}

/* Prints the battle pane
* battle_pane : the battle pane
*/
void printBattlePane(char *battle_pane) {
  printf("%s\n", battle_pane);
}

/* Fills an array with all the character of the battle pane file
* battle_pane : the array to fill representing the battle pane
*/
void loadBattlePane(char *battle_pane) {
  FILE *battle_pane_file = openFile(BATTLE_PANE_PATH, "r");
  fread(battle_pane, sizeof(char), BATTLE_PANE_LGTH, battle_pane_file);
  closeFile(battle_pane_file);
}

/* Fills a char array describing the health bar of a pokemon depending on the the ratio between his
*  health points and his maximum health points
* health_bar : the char array to fill
* pkmn : the pokemon
*/
void createHealthBar(char *health_bar, Pokemon pkmn) {
  health_bar[0] = '{';
  health_bar[HLTH_BAR_LGTH-1] = '}';
  double hp_percentage = ((double)pkmn.stats.hp / (double)pkmn.stats.hp_max);
  int pipe_number = (int) (hp_percentage * (HLTH_BAR_LGTH - 2));
  for (int i = 1; i < pipe_number+1; i++) {
    health_bar[i] = '|';
  }
  for (int j = pipe_number+1; j < HLTH_BAR_LGTH-1; j++) {
    health_bar[j] = ' ';
  }
}

/* Find and return the maximum length of the text in the current information menu
* battle_pane : the battle pane
*/
int getTextLineMaxLength(char *battle_pane) {
  int i = FRST_TEXT_LINE_POS+2;
  int text_line_max_lgth = 0;
  while (battle_pane[i] != '|') {
    text_line_max_lgth++;
    i++;
  }
  return text_line_max_lgth;
}

/* Erases the two lines of information
* battle_pane : the battle pane
*/
void eraseInfoText(char *battle_pane) {
  int text_line_max_lgth = getTextLineMaxLength(battle_pane);
  eraseArrayLine(FRST_TEXT_LINE_POS+text_line_max_lgth, battle_pane, text_line_max_lgth+1);
  eraseArrayLine(SCD_TEXT_LINE_POS+text_line_max_lgth, battle_pane, text_line_max_lgth+1);
}

/* Adds a information text to a line
* text : the text to add
* text_length : the length of text
* pos : the xy coordinates where to start adding the text
* battle_pane : the battle pane
*/
void addLineText(char *text, int text_length, int pos, char *battle_pane) {
  int text_line_max_lgth = getTextLineMaxLength(battle_pane);
  eraseArrayLine(pos+text_line_max_lgth, battle_pane, text_line_max_lgth);
  if (text_length <= text_line_max_lgth) {
    addStringToArray(text, text_length, pos, battle_pane);
  } else {
    addStringToArray(" ", 1, pos, battle_pane);
  }
}

/* Adds information texts to bothe top and bottom line
* top_text : the top text to add
* top_text_length : the length of top text
* bot_text : the bottom text to add
* bot_text_length : the length of bottom text
* battle_pane : the battle pane
*/
void addInfoText(char *top_text, int top_text_length, char *bot_text, int bot_text_length, char *battle_pane) {
  addLineText(top_text, top_text_length, FRST_TEXT_LINE_POS, battle_pane);
  addLineText(bot_text, bot_text_length, SCD_TEXT_LINE_POS, battle_pane);
}

/* Adds a pokemon name to battle pane
* pkmn : the pokemon
* battle_pane : the battle pane
* pos : the xy coordinate where to start writting the pokemon name
*/
void addPokemonName(Pokemon pkmn, char *battle_pane, int pos) {
  eraseArrayLine(pos+15, battle_pane, 20);
  int lvl_string_length = 6+nDigits(pkmn.lvl);
  int name_n_lvl_length = lvl_string_length+pkmn.name_length;
  char *name_n_lvl = malloc(sizeof(char)*name_n_lvl_length);
  sprintf(name_n_lvl, "%s  lvl %d", pkmn.name, pkmn.lvl);
  addStringToArray(name_n_lvl, name_n_lvl_length, pos, battle_pane);
  free(name_n_lvl);
}

/* Adds the player's pokemon health bar to battle pane
* pkmn : the pokemon
* battle_pane : the battle pane
* pos : the xy coordinate where to start writting the pokemon health bar
*/
void addPokemonHealthBar(Pokemon pkmn, char *battle_pane, int pos) {
  char health_bar[HLTH_BAR_LGTH];
  createHealthBar(health_bar, pkmn);
  addStringToArray(health_bar, HLTH_BAR_LGTH, pos, battle_pane);
}

/* Adds the player pokemon hp/hp_max point to battle pane
* pkmn : the pokemon
* battle_pane : the battle pane
* pos : the xy coordinate where to start writting the pokemon hp count
*/
void addPokemonHpPoint(Pokemon pkmn, char *battle_pane, int pos) {
  eraseArrayLine(pos+15, battle_pane, 20);
  int hp = pkmn.stats.hp;
  int hp_max = pkmn.stats.hp_max;
  int hp_length = nDigits(hp)+nDigits(hp_max)+7;
  char pdv_string[hp_length];
  sprintf(pdv_string, "%d / %d PDV", hp, hp_max);
  addStringToArray(pdv_string, hp_length, pos, battle_pane);
}

/* Adds the ailments to the battle pane
* pkmn : the pokemon
* battle_pane : the battle pane
* pos : the xy coordinate where to start writting the pokemon hp count
*/
void addAilments(Pokemon pkmn, char *battle_pane, int pos, bool left_erase) {
  char *ailment_string;
  int label_length = getAilmentLabel(pkmn, &ailment_string);
  if (left_erase) {
    eraseArrayLine(pos, battle_pane, 11);
    addStringToArray(ailment_string, label_length, pos-label_length, battle_pane);
  } else {
    eraseArrayLine(pos, battle_pane, -11);
    addStringToArray(ailment_string, label_length, pos, battle_pane);
  }
}

/* Refreshes the battle pane
* player : the players pokemon
* enemy : the enemy pokemon
* battle_pane : the battle pane
*/
void refreshBattlePane(Pokemon player, Pokemon enemy, char *battle_pane) {
  addPokemonName(enemy, battle_pane, ENEMY_PKMN_NAME_ENDNG_POS);
  addPokemonHealthBar(enemy, battle_pane, ENEMY_HLTH_BAR_STRTNG_POS);
  addPokemonHpPoint(enemy, battle_pane, ENEMY_HLTH_POINT_ENDING_POS);
  addAilments(enemy, battle_pane, ENEMY_HLTH_BAR_STRTNG_POS-2, true);

  // addPokemonName(player, battle_pane, PLAYER_PKMN_NAME_POS-(player.name_length+6+nDigits(player.lvl)));
  addPokemonName(player, battle_pane, PLAYER_PKMN_NAME_POS-player.name_length-7);
  addPokemonHealthBar(player, battle_pane, PLAYER_HLTH_BAR_STRTNG_POS);
  int hp_length = nDigits(player.stats.hp)+nDigits(player.stats.hp_max)+7;
  // int hp_length = nDigits(player.stats.hp)+nDigits(player.stats.hp_max)+7;
  addPokemonHpPoint(player, battle_pane, PLAYER_HLTH_POINT_POS-hp_length);
  addAilments(player, battle_pane, PLAYER_HLTH_BAR_STRTNG_POS+HLTH_BAR_LGTH+1, false);
}

/* Replace the main menu by a yes/no question menu
* battle_pane : the battle pane
*/
void showYesNoMenu(char *battle_pane) {
  eraseArrayLine(CURSOR_FRST_SKILL_LINE, battle_pane, 50);
  eraseArrayLine(CURSOR_SCD_SKILL_LINE, battle_pane, 50);

  int yes_length = 3+MENU_ARROW_LGTH+1;
  int no_length = 3;
  int yes_coord = CURSOR_FRST_SKILL_LINE-LEFT_SHIFT-MENU_ARROW_LGTH-1;
  int no_coord = CURSOR_SCD_SKILL_LINE-LEFT_SHIFT;
  addStringToArray("-> Oui", yes_length, yes_coord, battle_pane);
  addStringToArray("Non", no_length, no_coord, battle_pane);
  shiftMenuPipes(battle_pane, yes_coord-NEXT_LINE-LEFT_SHIFT+1-MENU_ARROW_LGTH, FRST_PIPE_MENU, NEXT_LINE, 4);
}

/* Compare the length of the skills and place them respecting alignment
* cursor_first_skills_line : the cursor of the first skill line
* cursor_second_skills_line : the cursor of the second skill line
//these cursors are used to know where to place the different parts of a line one by one
* i : the index of the skill on the top
* player : the player
* battle_pane : the battle pane
*/
void compareAndPlaceSkills(int *cursor_first_skills_line, int *cursor_second_skills_line, int i, Player *player, char *battle_pane) {
  int higher_length = 0;
  Pokemon pkmn = player->pkmns[0];
  if (pkmn.skills[i].name_length > pkmn.skills[i+2].name_length) {
    higher_length = pkmn.skills[i].name_length;
  } else {
    higher_length = pkmn.skills[i+2].name_length;
  }
  *cursor_first_skills_line -= higher_length;
  *cursor_second_skills_line -= higher_length;
  addStringToArray(pkmn.skills[i].name, pkmn.skills[i].name_length, *cursor_first_skills_line, battle_pane);
  addStringToArray(pkmn.skills[i+2].name, pkmn.skills[i+2].name_length, *cursor_second_skills_line, battle_pane);
}

/* Places two skills one above the other in the skill menu respecting alignment and
   Creates the space to place that will be used by the arrows
For exemple : Vive-Attaque
              Eclair
* player : the player
* battle_pane : the battle pane
* i : the index of the skill on the top
* cursor_first_skills_line : the cursor of the first skill line
* cursor_second_skills_line : the cursor of the second skill line
//these cursors are used to know where to place the different parts of a line one by one
* arrows_xys : an array to save the coordinates of the 4 arrows pointing the skills
*/
void placeTwoSkills(Player *player, char *battle_pane, int i, int *cursor_first_skills_line, int *cursor_second_skills_line, int arrows_xys[]) {
  compareAndPlaceSkills(cursor_first_skills_line, cursor_second_skills_line, i, player, battle_pane);
  arrows_xys[i] = *cursor_first_skills_line - 3;
  arrows_xys[i+2] = *cursor_second_skills_line - 3;
  *cursor_first_skills_line -= 5;
  *cursor_second_skills_line -= 5;
  addStringToArray("     ", 5, *cursor_first_skills_line, battle_pane);
  addStringToArray("     ", 5, *cursor_second_skills_line, battle_pane);
}

/* Changes the menu to the skill menu
* player : the player
* battle_pane : the battle pane
* arrows_xys : the xy coordinates of the arrows in the skill menu
*/
void changeMenuToSkillMenu(Player *player, char *battle_pane, int arrows_xys[]) {
  int cursor_first_skills_line = CURSOR_FRST_SKILL_LINE;
  int cursor_second_skills_line = CURSOR_SCD_SKILL_LINE;

  eraseArrayLine(cursor_first_skills_line, battle_pane, 35);
  eraseArrayLine(cursor_second_skills_line, battle_pane, 35);

  placeTwoSkills(player, battle_pane, 1, &cursor_first_skills_line, &cursor_second_skills_line, arrows_xys);
  placeTwoSkills(player, battle_pane, 0, &cursor_first_skills_line, &cursor_second_skills_line, arrows_xys);

  shiftMenuPipes(battle_pane, cursor_first_skills_line-1-NEXT_LINE, FRST_PIPE_MENU, NEXT_LINE, 4);//-1 because there cursor_first_skills_line is only 1 space left to the '-' of the arrow
  addStringToArray("->", 2, arrows_xys[0], battle_pane);
}

/* Sets the main menu back
* battle_pane : the battle pane
* arrows_xys : the xy coordinates of the arrows in the skill menu
* arrow : the arrow shown when the main menu is set back
*/
void setMenuBack(char *battle_pane, int *arrows_xys, MenuArrow arrow) {
  eraseArrayLine(CURSOR_FRST_SKILL_LINE, battle_pane, 50);
  eraseArrayLine(CURSOR_SCD_SKILL_LINE, battle_pane, 50);
  shiftMenuPipes(battle_pane, FRST_PIPE_MENU, arrows_xys[0]-3-NEXT_LINE, NEXT_LINE, 4);//-3 because 2 pipes are between '-' of the arrow and left |
  MenuArrow arrows[4] = {ATTAQUES, SAC, POKEMON, FUITE};
  addArrow(arrow, battle_pane);
  addStringToArray("Attaques", 8, arrows[0]+3, battle_pane);
  addStringToArray("Sac", 3, arrows[1]+3, battle_pane);
  addStringToArray("Pokemon", 7, arrows[2]+3, battle_pane);
  addStringToArray("Fuite", 5, arrows[3]+3, battle_pane);
}



/* Manages the key pressed by the player to chose a skill
* targetted_skill : the current targetted skill
* key_pressed : the key pressed by the user
* arrows_xys : the xy coordinates of the skill menu arrows
* battle_pane : the battle pane
* player : the player
* enemy : the enemy pokemon
* state : 0 to trigger a skill, 1 to replace a skill (skill learning)
* return 3 to stay in the skills menu, 0 to come back to the main menu,
*        2 to do nothing, 1 to come back to map, 7 to replace the targetted skill (skill learning)
*        8 to come back to yes/no menu (learning skill)
*/
int manageSkillMenuKeyPressed(int *targetted_skill, int key_pressed, int arrows_xys[], char *battle_pane, Player *player, Pokemon *enemy, int state) {
  int key_pressed_status = 3;//stay in skills menu
  if (key_pressed == MOVE_Z) {
    removeArrow(arrows_xys[*targetted_skill], battle_pane);
    if (*targetted_skill == 2) {
      *targetted_skill = 0;
    } else if (*targetted_skill == 3) {
      *targetted_skill = 1;
    }
    addArrow(arrows_xys[*targetted_skill], battle_pane);
  } else if (key_pressed == MOVE_Q) {
    removeArrow(arrows_xys[*targetted_skill], battle_pane);
    if (*targetted_skill == 1) {
      *targetted_skill = 0;
    } else if (*targetted_skill == 3) {
      *targetted_skill = 2;
    }
    addArrow(arrows_xys[*targetted_skill], battle_pane);
  } else if (key_pressed == MOVE_S) {
    removeArrow(arrows_xys[*targetted_skill], battle_pane);
    if (*targetted_skill == 0) {
      *targetted_skill = 2;
    } else if (*targetted_skill == 1) {
      *targetted_skill = 3;
    }
    addArrow(arrows_xys[*targetted_skill], battle_pane);
  } else if (key_pressed == MOVE_D) {
    removeArrow(arrows_xys[*targetted_skill], battle_pane);
    if (*targetted_skill == 0) {
      *targetted_skill = 1;
    } else if (*targetted_skill == 2) {
      *targetted_skill = 3;
    }
    addArrow(arrows_xys[*targetted_skill], battle_pane);
  } else if (key_pressed == 13) {
    if (state == 0) {// triggers a skill
      setMenuBack(battle_pane, arrows_xys, HIDDEN);
      clearAndPrintBattlePane(battle_pane);
      if (player->pkmns[0].skills[*targetted_skill].pp > 0) {
        PlayTurnParameters play_turn_params = {targetted_skill, player, enemy, battle_pane, &key_pressed_status, true};
        createThreadNW8(&playTurn, &play_turn_params);
        addArrow(ATTAQUES, battle_pane);
        clearAndPrintBattlePane(battle_pane);
      } else {
        key_pressed_status = 3;
        //TODO print pp = 0 warning
      }
    } else if (state == 1) {//replace a skill
      key_pressed_status = 7;//replace a skill
    }
  } else if (key_pressed == 'e') {
    key_pressed_status = 2;//does nothing
    // exit(0);
  } else if (key_pressed == 127) {
    key_pressed_status = 0;//come back to main menu
    if (state == 0) {
      setMenuBack(battle_pane, arrows_xys, ATTAQUES);
      clearAndPrintBattlePane(battle_pane);
    } else if (state == 1) {
      battle_pane[arrows_xys[0]-3-NEXT_LINE] = ' ';
      battle_pane[arrows_xys[0]-3-NEXT_LINE+3*NEXT_LINE] = '_';
      key_pressed_status = 8;//come back to yes/no menu (skill learning)
    }
  } else {
    key_pressed_status = 2;//does nothing
  }
  return key_pressed_status;
}

/* Changes the position of the arrow of the yes/no menu
* battle_pane : the battle pane
* yes : 1 if yes is selected, 0 if no is selected
*/
void setArrowYesNo(char *battle_pane, int yes) {
  int yes_pos = CURSOR_FRST_SKILL_LINE-3-3;//-shift-space-yes_length
  int no_pos = CURSOR_SCD_SKILL_LINE-3-3;//-shift-space-yes_length
  if (yes == 1) {
    int tmp = yes_pos;
    yes_pos = no_pos;
    no_pos = tmp;
  }
  addStringToArray("  ", 2, yes_pos, battle_pane);
  addStringToArray("->", 2, no_pos, battle_pane);
}

/* Switch from a menu to the skill menu and then manage the key pressed in this menu
* player : the player
* battle_pane : the battle pane
* enemy : the enemy pokemon
* new_skill : the new skill to teach to the first player's pokemon (only used if state = 1)
* state : the state of the skill menu (0 to trigger a skill, 1 to replace a skill by the new one)
* return 1 to end the battle, 0 to come back to main menu
*/
int switchThenManageSkillMenu(Player *player, char *battle_pane, Pokemon *enemy, Skill *new_skill, int state) {
  int arrows_xys[4];
  changeMenuToSkillMenu(player, battle_pane, arrows_xys);
  clearAndPrintBattlePane(battle_pane);
  int targetted_skill = 0;
  int targetted_skill_temp = targetted_skill;
  char key_pressed;
  int key_pressed_status = 0;
  do {
    if (state == 1) {
      printLearningSkillInfo(player->pkmns[0].skills[targetted_skill], *new_skill);
    }
    key_pressed = getchar();
    targetted_skill_temp = targetted_skill;
    key_pressed_status = manageSkillMenuKeyPressed(&targetted_skill, key_pressed, arrows_xys, battle_pane, player, enemy, state);
    if (key_pressed_status == 3 && targetted_skill_temp != targetted_skill) {//if arrows was moved
      clearAndPrintBattlePane(battle_pane);
    }
  } while (key_pressed_status != 1 && key_pressed_status != 0 && key_pressed_status != 7 && key_pressed_status != 8);//1 if battle finished, 0 if turn finished, 7 if skill to replace chosen (skill learning)
  if (key_pressed_status == 7) {
    key_pressed_status = targetted_skill;
  }
  return key_pressed_status;
}

bool isCatchSuccessful(Player *player, Pokemon *enemy) {
  int catch_percentage = 70;
  catch_percentage += player->pkmns[0].stats.hp/20+10;
  if (enemy->crt_ailments[0] == POISONING || enemy->crt_ailments[0] == BURN || enemy->crt_ailments[0] == PARALYSIS) {
    catch_percentage += 6;
  } else if (enemy->crt_ailments[0] == SLEEP || enemy->crt_ailments[0] == FREEZE) {
    catch_percentage += 12;
  }
  bool catched = false;
  srand(time(NULL));
  if (rand()%100 < catch_percentage) {
    catched = true;
  }
  return catched;
}

int catchPokemon(char *battle_pane, Player *player, Pokemon *enemy) {
  int catched = 0;
  addInfoText(THROW_POKEBALL, THROW_POKEBALL_LENGTH, " ", 1, battle_pane);
  clearAndPrintBattlePane(battle_pane);
  waitNMs(WAIT_BETWEEN_ANIM*2);

  if (isCatchSuccessful(player, enemy)) {
    int gratz_catch_length = enemy->name_length+3;
    char *gratz_catch_string = malloc(sizeof(char)*gratz_catch_length+1);
    sprintf(gratz_catch_string, "un %s", enemy->name);
    addInfoText(GRATZ_CATCH, GRATZ_CATCH_LENGTH, gratz_catch_string, gratz_catch_length, battle_pane);
    clearAndPrintBattlePane(battle_pane);
    waitNMs(WAIT_BETWEEN_ANIM*2);
    free(gratz_catch_string);
    copyPokemon(*enemy, &player->pkmns[player->pkmn_count]);
    player->pkmn_count++;
    catched = 1;
  } else {
    int fail_catch_length = enemy->name_length+10;
    char *fail_catch_string = malloc(sizeof(char)*fail_catch_length+1);
    sprintf(fail_catch_string, "%s est sorti", enemy->name);
    addInfoText(fail_catch_string, fail_catch_length, FAIL_CATCH, FAIL_CATCH_LENGTH, battle_pane);
    clearAndPrintBattlePane(battle_pane);
    waitNMs(WAIT_BETWEEN_ANIM);
    free(fail_catch_string);
  }
  eraseInfoText(battle_pane);
  clearAndPrintBattlePane(battle_pane);
  return catched;
}

void playOnlyEnemyTurn(MenuArrow *arrow, char *battle_pane, Player *player, Pokemon *enemy, int *stop) {
  int tmp = 0;
  removeArrow(POKEMON, battle_pane);
  clearAndPrintBattlePane(battle_pane);
  PlayTurnParameters play_turn_params = {&tmp, player, enemy, battle_pane, stop, false};
  createThreadNW8(&playTurn, &play_turn_params);
  addArrow(ATTAQUES, battle_pane);
  *arrow = ATTAQUES;
  clearAndPrintBattlePane(battle_pane);
}

/* Manages the Enter key pressed to go into a menu
* arrow : the current position of the arrow in the menu
* battle_pane : the battle pane
* player : the player
* enemy : the enemy pokemon
* return 1 if the battle is finished, 0 to come back to main menu
*/
int manageMenuChoice(MenuArrow *arrow, char *battle_pane, Player *player, Pokemon *enemy) {
  int stop = 0;
  if (*arrow == FUITE) {
    stop = 1;
  } else if (*arrow == ATTAQUES) {
    stop = switchThenManageSkillMenu(player, battle_pane, enemy, NULL, 0);
  } else if (*arrow == POKEMON) {
    int action = managePokemonsMenu(player, 1);
    refreshBattlePane(player->pkmns[0], *enemy, battle_pane);
    clearAndPrintBattlePane(battle_pane);
    if (action == 3) {//if the pokemon in the battle was swapped
      playOnlyEnemyTurn(arrow, battle_pane, player, enemy, &stop);
    }
  } else if (*arrow == SAC) {
    int used_item_id = manageBagMenu(player, 0);
    if (used_item_id == 0) {//if pokeball used
      removeArrow((int)*arrow, battle_pane);
    }
    clearAndPrintBattlePane(battle_pane);
    if (used_item_id == 0 && player->pkmn_count < 6) {//if pokeball used
      stop = catchPokemon(battle_pane, player, enemy);
      if (stop == 0) {//if catch failed
        playOnlyEnemyTurn(arrow, battle_pane, player, enemy, &stop);
      }
    }
  }
  return stop;
}

/* Manages the key pressed by the player during the battle
* key_pressed : the key pressed by the player
* arrow : the current position of the arrow in the menu
* battle_pane : the battle pane
* player : the player
* enemy : the enemy pokemon
* return 1 if the battle is finished, 0 to come back to main menu, 2 to do nothing
*/
int manageBattleMenuKeyPressed(char key_pressed, MenuArrow *arrow, char *battle_pane, Player *player, Pokemon *enemy) {
  int stop = 0;
  if (key_pressed == MOVE_Z) {
    removeArrow((int)*arrow, battle_pane);
    if (*arrow == POKEMON) {
      *arrow = ATTAQUES;
    } else if (*arrow == FUITE) {
      *arrow = SAC;
    }
    addArrow((int)*arrow, battle_pane);
  } else if (key_pressed == MOVE_Q) {
    removeArrow((int)*arrow, battle_pane);
    if (*arrow == SAC) {
      *arrow = ATTAQUES;
    } else if (*arrow == FUITE) {
      *arrow = POKEMON;
    }
    addArrow((int)*arrow, battle_pane);
  } else if (key_pressed == MOVE_S) {
    removeArrow((int)*arrow, battle_pane);
    if (*arrow == ATTAQUES) {
      *arrow = POKEMON;
    } else if (*arrow == SAC) {
      *arrow = FUITE;
    }
    addArrow((int)*arrow, battle_pane);
  } else if (key_pressed == MOVE_D) {
    removeArrow((int)*arrow, battle_pane);
    if (*arrow == POKEMON) {
      *arrow = FUITE;
    } else if (*arrow == ATTAQUES) {
      *arrow = SAC;
    }
    addArrow((int)*arrow, battle_pane);
  } else if (key_pressed == ENTER) {
    stop = manageMenuChoice(arrow, battle_pane, player, enemy);
  } else if (key_pressed == 'e') {
    stop = 2;//does nothing
    // exit(0);
  } else {
    stop = 2;//does nothing
  }
  return stop;
}

/* Manages the battle
* player : the player
*/
void battle(Player *player) {
  char battle_pane[BATTLE_PANE_LGTH];
  loadBattlePane(battle_pane);
  char *random_name;
  getRandomPokemonName(&random_name);
  Pokemon enemy = fillPokemon(random_name);
  free(random_name);
  srand(time(NULL));
  int random_sub = (rand()%4)+1;
  setPokemonLvl(&enemy, player->pkmns[0].lvl-random_sub);
  refreshBattlePane(player->pkmns[0], enemy, battle_pane);
  printAndManageBattlePane(battle_pane, player, &enemy);
  freePokemon(enemy);
}


/* Prints and manages the battle pane
* battle_pane : the array representing the battle pane
* player : the player
* enemy : the enemy pokemon
*/
void printAndManageBattlePane(char *battle_pane, Player *player, Pokemon *enemy) {
  clearAndPrintBattlePane(battle_pane);
  MenuArrow arrow = ATTAQUES;
  MenuArrow arrow_temp = arrow;
  char key_pressed = ' ';
  int stop = 0;
  while (stop != 1) {
    key_pressed = getchar();
    arrow_temp = arrow;
    stop = manageBattleMenuKeyPressed(key_pressed, &arrow, battle_pane, player, enemy);
    if (stop != 2 && arrow_temp != arrow) {
      clearAndPrintBattlePane(battle_pane);
    }
  }
}

/* Checks if the player triggered a battle
* player : the player
*/
void isBattle(Player *player) {
  if (isEqual(player->char_at_pos, LONG_GRASS)) {
    if (rand()%100 < BATTLE_PERCENTAGE) {
      battle(player);
      resetPlayerPkmnsStatsAfterBattle(player);
    }
  }
}