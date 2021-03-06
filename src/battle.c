#include "battle.h"
#include "battlePane.h"
#include "fileManager.h"
#include "map.h"
#include "pokemon.h"
#include "print.h"
#include "startMenu.h"
#include "util.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

/* The types table */
static int types_table[TYPES_TABLE_LGTH]; // 1 is x1, 2 is x2, 3 is x0.5
/* A boolean to describe if the type table is filled */
static bool types_table_filled = false;

/* Swaps the content of p1 and p2 as pointers on Pokemon
 * p1 : the first pointer
 * p2 : the second pointer
 */
void swapPokemonPointerContent(Pokemon *p1, Pokemon *p2) {
  Pokemon tmp;
  tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

/* Swaps the content of p1 and p2 as pointer on int
 * p1 : the first pointer
 * p2 : the second pointer
 */
void swapIntPointerContent(int *p1, int *p2) {
  int tmp;
  tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

/* Fills the types table
 */
void fillTypesTable() {
  if (!types_table_filled) {
    FILE *types_table_file = openFile(TYPES_TABLE_PATH, "r");
    int i = 0;
    while (fscanf(types_table_file, "%d|", &types_table[i]) != EOF) {
      i++;
    }
    closeFile(types_table_file);
    types_table_filled = true;
  }
}

/* Gets the weakness coefficient according to a skill and a pokemon
* skill_type : the type of the skill
* target : the pokemon targetted by the skill
* return a double related to the target weaknesses/resistances against the skill
type. values used to be : 0     if the target is insensitive to the skill 0.25
if the target has 2 types resisting to the skill type 0.50  if the target has 1
type resisting to the skill type 1     if the target has no type resisting to
the skill type 2     if the target has 1 type weak against the skill type 4 if
the target has 2 type weak against the skill type
*/
double getWeakness(Type skill_type, Pokemon *target) {
  double weakness = 1.0;
  int weakness_box = 0;
  for (int i = 0; i < 2; i++) {
    if (target->type[i] != NO_TYPE) {
      weakness_box = (int)skill_type * TYPE_COUNT + (int)target->type[i];
      if (types_table[weakness_box] == 3) {
        weakness *= 0.5;
      } else {
        weakness *= types_table[weakness_box];
      }
    }
  }
  return weakness;
}

/* Choses the play order of the pokemons
* frst_to_play : the fastest pkmn in theory
* scd_to_play : the slowest pkmn in theory
There, we are considering that the frst_to_play Pokemon is faster than the
scd_to_play Pokemon so that this function only tests if this is true or not
(swapping them if not)
* frst_player_skill : the skill chosen by the first player
* scd_player_skill : the skill chosen by the second player
* return true if the pkmn was swapped, false otherwise
*/
bool chosePlayOrder(Pokemon *frst_to_play, Pokemon *scd_to_play,
                    int *frst_player_skill, int *scd_player_skill) {
  printw("Frst to play : %s - %d spd\n", frst_to_play->name,
         frst_to_play->stats.spd);
  printw("Scd to play : %s - %d spd\n", scd_to_play->name,
         scd_to_play->stats.spd);
  int swapped = false;
  int priority1 = frst_to_play->skills[*frst_player_skill].priority;
  int priority2 = scd_to_play->skills[*scd_player_skill].priority;
  if (priority1 == priority2) {
    if (scd_to_play->stats.spd > frst_to_play->stats.spd) {
      printw("1");
      swapPokemonPointerContent(frst_to_play, scd_to_play);
      swapped = true;
    } else if (scd_to_play->stats.spd == frst_to_play->stats.spd) {
      if (rand() % 100 < SAME_SPEED_RAND_PERCENTAGE) {
        printw("2");
        swapPokemonPointerContent(frst_to_play, scd_to_play);
        swapped = true;
      }
    }
  } else if (priority1 < priority2) {
    printw("3");
    swapPokemonPointerContent(frst_to_play, scd_to_play);
    swapped = true;
  }
  printw("Frst to play : %s - %d spd\n", frst_to_play->name,
         frst_to_play->stats.spd);
  printw("Scd to play : %s - %d spd\n", scd_to_play->name,
         scd_to_play->stats.spd);
  return swapped;
}

/* Calculates the health points count the target will lose
 * skill_user : the pokemon playing the turn
 * target : the targetted pokemon
 * skill : the chosen skill
 * crit : true if the skill is a critical hit, false otherwise
 * weakness : a double related to the target weaknesses/resistances.
 * return the health points count the target will lose
 */
int hpCountLost(Pokemon *skill_user, Pokemon *target, Skill skill, bool crit,
                double weakness) {
  double hp_count_lost = 0;
  if (skill.power > 0) {
    double stab = 1.0;
    double critical_hit = 1.0;
    if (crit) {
      critical_hit = 1.5;
    }
    if (skill_user->type[0] == skill.type ||
        skill_user->type[1] == skill.type) {
      stab = 1.5;
    }
    double numerator =
        (skill_user->lvl * 0.4 + 2) * skill_user->stats.att * skill.power;
    double denominator = (target->stats.def * 50);
    hp_count_lost =
        (numerator / denominator + 2) * stab * weakness * critical_hit;
  }
  return (int)round(hp_count_lost);
}

/* Shows information in the chat menu about the ailment state of the pokemon
* skill_user : the pokemon curently triggering a skill
* target : the target
* swapped_xor_frst : this boolean describe whether the player's pokemon was
swapped or not and whether it's the first to play or not (see playTurn()) this
is used to know if the player is pointed by the 'skill_user' parameter or the
'target' one
* battle_pane : the battle_pane
* frst_ailment_text : the first text about the ailment state of the pokemon
* frst_ailment_text_length : the length of the first text about the ailment
state of the pokemon
* scd_ailment_text : the second text about the ailment state of the pokemon
* scd_ailment_text_length : the length of the second text about the ailment
state of the pokemon
*/
void ailmentTextAnimation(Pokemon *skill_user, Pokemon *target,
                          bool swapped_xor_frst, char *battle_pane,
                          char *frst_ailment_text, int frst_ailment_text_length,
                          char *scd_ailment_text, int scd_ailment_text_length) {
  int ailment_text_length =
      skill_user->name_length + 1 + frst_ailment_text_length;
  char *ailment_text = malloc(sizeof(char) * ailment_text_length + 1);
  sprintf(ailment_text, "%s %s", skill_user->name, frst_ailment_text);

  addInfoText(ailment_text, ailment_text_length, scd_ailment_text,
              scd_ailment_text_length, battle_pane);
  if (swapped_xor_frst) {
    refreshBattlePane(*skill_user, *target, battle_pane);
  } else {
    refreshBattlePane(*target, *skill_user, battle_pane);
  }
  clearAndPrintBattlePane(battle_pane);
  waitNMs(WAIT_BETWEEN_ANIM);
}

bool manageConfusion(Pokemon *skill_user, Pokemon *target,
                     bool swapped_xor_frst, char *battle_pane, int i) {
  bool play = true;
  if (rand() % 100 < 33) { // not confused yet
    removeAilment(skill_user, i);
    ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                         CONFUSION1_TEXT, CONFUSION1_TEXT_LENGTH, " ", 1);
  } else { // keep confused
    ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                         CONFUSION2_TEXT, CONFUSION2_TEXT_LENGTH, " ", 1);
    srand(time(NULL));
    if (rand() % 100 < 50) { // confusion hurts
      skill_user->stats.hp -= skill_user->stats.hp_max / 8;
      setBackHealthToZero(skill_user);
      play = false;
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                           CONFUSION3_TEXT, CONFUSION3_TEXT_LENGTH,
                           CONFUSION4_TEXT, CONFUSION4_TEXT_LENGTH);
    }
  }
  return play;
}
/* Manages the effect of the pokemon's first ailment
* skill_user : the pokemon which is gonna use a skill
* target : the target
* swapped_xor_frst : this boolean describe whether the player's pokemon was
swapped or not and whether it's the first to play or not (see playTurn())
this is used to know if the player is pointed by the 'skill_user' parameter
or the 'target' one
* battle_pane : the battle pane
*/
bool manageFirstAilment(Pokemon *skill_user, Pokemon *target,
                        bool swapped_xor_frst, char *battle_pane) {
  bool play = true;
  srand(time(NULL));
  switch (skill_user->crt_ailments[0]) {
  case PARALYSIS:
    if (rand() % 100 < 25) {
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                           FRST_PAR_TEXT, FRST_PAR_TEXT_LENGTH, SCD_PAR_TEXT,
                           SCD_PAR_TEXT_LENGTH);
      play = false;
    }
    break;

  case SLEEP:
    if (rand() % 100 < 66) {
      play = false;
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                           SLEEP_TEXT, SLEEP_TEXT_LENGTH, " ", 1);
    } else {
      removeAilment(skill_user, 0);
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                           WAKE_UP_TEXT, WAKE_UP_TEXT_LENGTH, " ", 1);
    }
    break;

  case FREEZE:
    if (rand() % 100 < 80) {
      play = false;
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                           FREEZE_TEXT, FREEZE_TEXT_LENGTH, " ", 1);
    } else {
      removeAilment(skill_user, 0);
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                           UNFREEZE_TEXT, UNFREEZE_TEXT_LENGTH, " ", 1);
    }
    break;

  case BURN:
    skill_user->stats.hp -= skill_user->stats.hp_max / 8;
    setBackHealthToZero(skill_user);
    ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                         BURN_TEXT, BURN_TEXT_LENGTH, " ", 1);
    break;

  case POISONING:
    skill_user->stats.hp -= skill_user->stats.hp_max / (rand() % 3 + 7);
    setBackHealthToZero(skill_user);
    ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane,
                         POISONNING_TEXT, POISONNING_TEXT_LENGTH, " ", 1);
    break;

  case CONFUSION:
    play =
        manageConfusion(skill_user, target, swapped_xor_frst, battle_pane, 0);
    break;
  }
  return play;
}

/* Manages the effect of all the pokemon's ailments
* skill_user : the pokemon which is gonna use a skill
* target : the target
* swapped_xor_frst : this boolean describe whether the player's pokemon was
swapped or not and whether it's the first to play or not (see playTurn()) this
is used to know if the player is pointed by the 'skill_user' parameter or the
'target' one
* battle_pane : the battle pane
*/
bool manageAllAilments(Pokemon *skill_user, Pokemon *target,
                       bool swapped_xor_frst, char *battle_pane) {
  bool play =
      manageFirstAilment(skill_user, target, swapped_xor_frst, battle_pane);
  if (skill_user->crt_ailments[1] == CONFUSION && play) {
    play =
        manageConfusion(skill_user, target, swapped_xor_frst, battle_pane, 1);
  }
  return play;
}

/* Manages the first information text animation
 * skill_user : the pokemon whose using the skill
 * chosen_skill : an integer representing the chosen skill
 * battle_pane : the battle pane
 */
void manageFirstTextAnimation(Pokemon *skill_user, int chosen_skill,
                              char *battle_pane) {
  Skill chosen = skill_user->skills[chosen_skill];
  int pkmn_use_skill_length = skill_user->name_length + 9 + chosen.name_length;
  char *pkmn_use_skill = malloc(sizeof(char) * pkmn_use_skill_length + 1);
  sprintf(pkmn_use_skill, "%s utilise %s", skill_user->name, chosen.name);
  addInfoText(pkmn_use_skill, pkmn_use_skill_length, " ", 1, battle_pane);
  clearAndPrintBattlePane(battle_pane);
  waitNMs(WAIT_BETWEEN_ANIM);
}

/* Manages the second information text animation
 * battle_pane : the battle pane
 * crit : this boolean is true if the skill is a critical hit, false otherwise
 * weakness : the weakness or resistance coefficient
 * return true if something was written, false otherwise
 */
bool manageSecondTextAnimation(char *battle_pane, bool crit, double weakness) {
  bool written_something = true;
  if (!crit && weakness != 1.0) {
    if (weakness > 1.0) {
      addInfoText(SUPER_EFFECTIVE_STRING, SUPER_EFFECTIVE_STRING_LENGTH, " ", 1,
                  battle_pane);
    } else {
      addInfoText(NOT_EFFECTIVE_STRING, NOT_EFFECTIVE_STRING_LENGTH, " ", 1,
                  battle_pane);
    }
  } else if (crit && weakness == 1.0) {
    addInfoText(CRIT_STRING, CRIT_STRING_LENGTH, " ", 1, battle_pane);
  } else if (crit && weakness != 1.0) {
    if (weakness > 1.0) {
      addInfoText(CRIT_STRING, CRIT_STRING_LENGTH, SUPER_EFFECTIVE_STRING,
                  SUPER_EFFECTIVE_STRING_LENGTH, battle_pane);
    } else {
      addInfoText(CRIT_STRING, CRIT_STRING_LENGTH, NOT_EFFECTIVE_STRING,
                  NOT_EFFECTIVE_STRING_LENGTH, battle_pane);
    }
  } else {
    written_something = false;
  }
  return written_something;
}

/* Manages the effects done by a skill
 * chosen_skill : an integer describing the chosen skill
 * skill_user : the pokemon playing the current turn
 * target : the targetted pokemon
 * battle_pane : the battle pane
 */
bool manageEffect(int chosen_skill, Pokemon *skill_user, Pokemon *target,
                  char *battle_pane) {
  Skill chosen = skill_user->skills[chosen_skill];
  Effect effect = chosen.effect;
  Pokemon *effect_target = skill_user;
  int message_length = 6; //"  de l" length
  char *message;
  int written_something = false;
  printw("%d\n", effect.stat_to_change);
  if (effect.stat_to_change != NO_STATENUM && rand() % 100 < effect.accuracy) {
    written_something = true;
    double base = 1;
    if (effect.target == 0) {
      effect_target = target;
    }
    effect_target->effect_stage[(int)effect.stat_to_change] +=
        effect.change_value;
    for (int i = 0;
         i < abs(effect_target->effect_stage[(int)effect.stat_to_change]);
         i++) {
      base += 0.5;
    }
    if (effect.change_value < 0) {
      base = 1 / base;
    }
    switch (effect.stat_to_change) {
    case ATT:
      effect_target->stats.att = effect_target->stats.att_max * base;
      if (effect.change_value < 0) {
        message_length +=
            effect_target->name_length + 8 + 4; // 8 -> "'attaque" | 4 -> "perd"
        message = malloc(sizeof(char) * message_length);
        sprintf(message, "%s perd de l'attaque", effect_target->name);
      } else {
        message_length += effect_target->name_length + 8 +
                          5; // 8 -> "'attaque" | 5 -> "gagne"
        message = malloc(sizeof(char) * message_length);
        sprintf(message, "%s gagne de l'attaque", effect_target->name);
      }
      break;

    case DEF:
      effect_target->stats.def = effect_target->stats.def_max * base;
      if (effect.change_value < 0) {
        message_length += effect_target->name_length + 9 +
                          4; // 9 -> "a défense" | 4 -> "perd"
        message = malloc(sizeof(char) * message_length);
        sprintf(message, "%s perd de la defense", effect_target->name);
      } else {
        message_length += effect_target->name_length + 9 +
                          5; // 9 -> "a défense" | 5 -> "gagne"
        message = malloc(sizeof(char) * message_length);
        sprintf(message, "%s gagne de la defense", effect_target->name);
      }
      break;

    case SPD:
      effect_target->stats.spd = effect_target->stats.spd_max * base;
      if (effect.change_value < 0) {
        message_length += effect_target->name_length + 9 +
                          4; // 9 -> "a vitesse" | 4 -> "perd"
        message = malloc(sizeof(char) * message_length);
        sprintf(message, "%s perd de la vitesse", effect_target->name);
      } else {
        message_length += effect_target->name_length + 9 +
                          5; // 9 -> "a vitesse" | 5 -> "gagne"
        message = malloc(sizeof(char) * message_length);
        sprintf(message, "%s gagne de la vitesse", effect_target->name);
      }
      break;

    case ACC:
      for (int i = 0; i < 4; i++) {
        effect_target->skills[i].accuracy =
            effect_target->skills[i].accuracy_max * base;
      }
      if (effect.change_value < 0) {
        message_length += effect_target->name_length + 11 +
                          4; // 11 -> "a precision" | 4 -> "perd"
        message = malloc(sizeof(char) * message_length);
        sprintf(message, "%s perd de la precision", effect_target->name);
      } else {
        message_length += effect_target->name_length + 11 +
                          5; // 11 -> "a precision" | 5 -> "gagne"
        message = malloc(sizeof(char) * message_length);
        sprintf(message, "%s gagne de la precision", effect_target->name);
      }
      break;

    default:
      printw("ERROR MANAGING EFFECT\n");
      exit(4);
      break;
    }
    addInfoText(message, message_length, " ", 1, battle_pane);
    clearAndPrintBattlePane(battle_pane);
    waitNMs(WAIT_BETWEEN_ANIM);
  }
  return written_something;
}

/* Triggers the skill
* skill_user : the pokemon playing the turn
* target : the targetted pokemon
* chosen_skill : an integer representing the chosen skill
* battle_pane : the battle pane
* swapped_xor_frst : this boolean describe whether the player's pokemon was
swapped or not and whether it's the first to play or not (see playTurn()) this
is used to know if the player is pointed by the 'skill_user' parameter or the
'target' one
* return true if the skill has failed, false otherwise
*/
bool useSkill(Pokemon *skill_user, Pokemon *target, int chosen_skill,
              char *battle_pane, bool swapped_xor_frst) {
  Skill chosen = skill_user->skills[chosen_skill];
  fillTypesTable();
  bool failed = false;
  if (rand() % 100 < chosen.accuracy) {
    bool crit = false;
    double weakness = getWeakness(chosen.type, target);
    if (weakness != 0.0) {
      if (rand() % 100 < CRIT_PERCENTAGE) {
        crit = true;
      }
      target->stats.hp -=
          hpCountLost(skill_user, target, chosen, crit, weakness);
      if (target->stats.hp < 0) {
        target->stats.hp = 0;
      }
      if (chosen.power == 0) {
        crit = false;
        weakness = 1.0;
      }
      bool written_something =
          manageSecondTextAnimation(battle_pane, crit, weakness);
      if (rand() % 100 < chosen.ailment.accuracy) {
        addAilment(target, chosen.ailment.ailment_enum);
      }
      if (swapped_xor_frst) {
        refreshBattlePane(*skill_user, *target, battle_pane);
      } else {
        refreshBattlePane(*target, *skill_user, battle_pane);
      }
      if (written_something) {
        clearAndPrintBattlePane(battle_pane);
        waitNMs(WAIT_BETWEEN_ANIM);
      }
      if (!isAnyKoPokemon(skill_user, target)) {
        written_something =
            manageEffect(chosen_skill, skill_user, target, battle_pane);
        eraseInfoText(battle_pane);
        clearAndPrintBattlePane(battle_pane);
      }
      if (!written_something && !isAnyKoPokemon(skill_user, target)) {
        waitNMs(WAIT_BETWEEN_ANIM);
      }
    } else {
      addInfoText(NO_EFFECT_STRING, NO_EFFECT_STRING_LENGTH, " ", 1,
                  battle_pane);
      clearAndPrintBattlePane(battle_pane);
      waitNMs(WAIT_BETWEEN_ANIM);
    }
  } else {
    failed = true;
    int name_miss_string_length = skill_user->name_length + MISS_STRING_LENGTH;
    char *name_miss_string = malloc(sizeof(char) * name_miss_string_length);
    sprintf(name_miss_string, "%s%s", skill_user->name, MISS_STRING);
    addInfoText(name_miss_string, name_miss_string_length, " ", 1, battle_pane);
    clearAndPrintBattlePane(battle_pane);
    waitNMs(WAIT_BETWEEN_ANIM);
  }
  if (skill_user->skills[chosen_skill].pp > 0) {
    skill_user->skills[chosen_skill].pp--;
  }
  return failed;
}

/* Checks if any of the two pokemon is KO
 * p1 : the first pokemon
 * p2 : the second pokemon
 * return true if on of the two pokemons is KO, false otherwise
 */
bool isAnyKoPokemon(Pokemon *p1, Pokemon *p2) {
  return p1->stats.hp == 0 || p2->stats.hp == 0;
}

/* Plays the turn of one pokemon
* skill_user : the pokemon playing the turn
* target : the targetted pokemon
* chosen_skill : an integer representing the chosen skill
* swapped_xor_frst : this boolean describe whether the player's pokemon was
swapped or not and whether it's the first to play or not (see playTurn()) this
is used to know if the player is pointed by the 'skill_user' parameter or the
'target' one
* battle_pane : the battle pane
* return true if the skill failed, false otherwise
*/
bool playOnePokemonTurn(Pokemon *skill_user, Pokemon *target, int chosen_skill,
                        bool swapped_xor_frst, char *battle_pane) {
  manageFirstTextAnimation(skill_user, chosen_skill, battle_pane);
  bool failed =
      useSkill(skill_user, target, chosen_skill, battle_pane, swapped_xor_frst);
  return failed;
}

/* Manages the keys pressed in a yes/no menu
 * key_pressed : the key pressed
 * yes : an integer describing which one of the yes or no field is targetted (1
 * for yes, 0 for no)
 */
int manageYesNoKeyPressed(char key_pressed, int *yes) {
  int key_pressed_status = 1;
  if (key_pressed == MOVE_Z) {
    *yes = 1;
  } else if (key_pressed == MOVE_S) {
    *yes = 0;
  } else if (key_pressed == ENTER) { // enter
    key_pressed_status = 0;
  } else { // wrong key
    key_pressed_status = 2;
  }
  return key_pressed_status;
}

/* Manages the yes/no menu
 * yes : an integer describing which one of the yes or no field is targetted (1
 * for yes, 0 for no) battle_pane : the battle pane
 */
void manageYesNoMenu(int *yes, char *battle_pane) {
  char key_pressed = -1;
  int key_pressed_status;
  do {
    do {
      getUserInput(&key_pressed);
    } while (key_pressed == -1);
    key_pressed_status = manageYesNoKeyPressed(key_pressed, yes);
    if (key_pressed_status == 1) {
      setArrowYesNo(battle_pane, *yes);
      clearAndPrintBattlePane(battle_pane);
    }
  } while (key_pressed_status != 0); //!= enter
}

/* Manages the time when a pokemon can learn a new skill
 * player : the player
 * battle_pane : the battle pane
 * frst_to_play : the first player to play the turn
 * scd_to_play : the second player to play the turn
 */
void manageLearningSkill(Player *player, char *battle_pane,
                         Pokemon *frst_to_play, Pokemon *scd_to_play) {
  Pokemon *learner = &player->pkmns[0];
  srand(time(NULL));
  int random;
  do {
    random = rand() % SKILLS_COUNT;
  } while (containsSkill(learner->skills, 4, random));
  Skill new_skill = fillSkill(random);
  bool leave_learning = false;

  int wants2learn_length = learner->name_length + 16 + new_skill.name_length;
  char *wants2learn = malloc(sizeof(char) * (wants2learn_length + 1));
  sprintf(wants2learn, "%s veut apprendre %s", learner->name, new_skill.name);

  int wants2giveup_length = 23 + new_skill.name_length + 2;
  char *wants2giveup = malloc(sizeof(char) * (wants2giveup_length + 1));
  sprintf(wants2giveup, "Voulez-vous abandonner %s ?", new_skill.name);

  int replace_length = 4 + new_skill.name_length + 2;
  char *replace = malloc(sizeof(char) * (replace_length + 1));
  sprintf(replace, "par %s ?", new_skill.name);

  while (!leave_learning) { // while new skill not learned or given up
    showYesNoMenu(battle_pane);
    addInfoText(wants2learn, wants2learn_length, "Voulez-vous lui enseigner ?",
                27, battle_pane);
    clearAndPrintBattlePane(battle_pane);
    int yes_learn = 1;
    manageYesNoMenu(&yes_learn, battle_pane);
    if (yes_learn == 1) { // if user chosed to learn the new skill
      int yes_coord = CURSOR_FRST_SKILL_LINE - LEFT_SHIFT - MENU_ARROW_LGTH - 1;
      battle_pane[yes_coord - NEXT_LINE - LEFT_SHIFT + 1 - MENU_ARROW_LGTH] =
          ' ';
      battle_pane[yes_coord - NEXT_LINE - LEFT_SHIFT + 1 - MENU_ARROW_LGTH +
                  3 * NEXT_LINE] = '_';
      addInfoText("Remplacer quel attaque", 22, replace, replace_length,
                  battle_pane);
      int targetted_skill =
          switchThenManageSkillMenu(player, battle_pane, NULL, &new_skill,
                                    1); // can be null because state 1
      if (targetted_skill == 8) {
        showYesNoMenu(battle_pane);
      } else {
        setSkill(learner, new_skill, targetted_skill);
        leave_learning = true;
      }
    } else if (yes_learn == 0) { // if user chosed to give up the new skill
      int yes_give_up = 1;
      setArrowYesNo(battle_pane, yes_give_up);
      addInfoText(wants2giveup, wants2giveup_length, " ", 1, battle_pane);
      clearAndPrintBattlePane(battle_pane);
      manageYesNoMenu(&yes_give_up, battle_pane);
      if (yes_give_up == 1) { // if user confirmed the give up
        leave_learning = true;
      } else { // if user refuted the give up
        yes_learn = 1;
      }
    }
  }
}

/* Manages the time when a pokemon is level-upping
 * player : the player
 * battle_pane : the battle pane
 * frst_to_play : the first player to play the turn
 * scd_to_play : the second player to play the turn
 */
void manageLevelUp(Player *player, char *battle_pane, Pokemon *frst_to_play,
                   Pokemon *scd_to_play) {
  int level_up_length =
      player->pkmns[0].name_length + 17 + nDigits(player->pkmns[0].lvl);
  char *lvl_up = malloc(sizeof(char) * (level_up_length + 1));
  sprintf(lvl_up, "%s passe au niveau %d", player->pkmns[0].name,
          player->pkmns[0].lvl);
  addInfoText(lvl_up, level_up_length, " ", 1, battle_pane);
  refreshBattlePane(*frst_to_play, *scd_to_play, battle_pane);
  clearAndPrintBattlePane(battle_pane);
  waitNMs(WAIT_BETWEEN_ANIM);
  if (player->pkmns[0].lvl % PKMN_SKILL_LEARN_FREQU == 0) {
    manageLearningSkill(player, battle_pane, frst_to_play, scd_to_play);
  }
}

/* Manages the time when the user's pokemon
 * player : the player
 * battle_pane : the battle pane
 */
void manageKoAnimation(Player *player, char *battle_pane) {
  int is_ko_length = player->pkmns[0].name_length + 9;
  char *is_ko = malloc(sizeof(char) * (is_ko_length + 1));
  sprintf(is_ko, "%s est K.O.", player->pkmns[0].name);
  addInfoText(is_ko, is_ko_length, " ", 1, battle_pane);
  clearAndPrintBattlePane(battle_pane);
  waitNMs(WAIT_BETWEEN_ANIM + 200);
}

/* Plays a battle turn (this function is used in a thread)
 * p : p is a PlayTurnParameters structure
 */
// void *playTurn(void *params) {
void *playTurn(int *targetted_skill, Player *player, Pokemon *enemy, char *battle_pane, int *key_pressed_status, bool player_turn) {
  // PlayTurnParameters *params = p;
  // int *targetted_skill = params->targetted_skill;
  // Player *player = params->player;
  // Pokemon *enemy = params->enemy;
  // char *battle_pane = params->battle_pane;
  // int *key_pressed_status = params->key_pressed_status;
  // bool player_turn = params->player_turn;
  int res = 0;
  // considering firstly that the player is faster than the enemy
  Pokemon *frst_to_play = &player->pkmns[0];
  Pokemon *scd_to_play = enemy;
  int *frst_player_skill = malloc(sizeof(int));
  int *scd_player_skill = malloc(sizeof(int));
  *frst_player_skill = *targetted_skill;
  *scd_player_skill = rand() % 4;
  while (scd_to_play->skills[*scd_player_skill].pp == 0) {
    *scd_player_skill = rand() % 4;
  }

  bool swapped = false;
  bool failed = false;
  bool is_ko = false;
  if (player_turn) { // if the turn of the player is not skipped
    swapped = chosePlayOrder(frst_to_play, scd_to_play, frst_player_skill,
                             scd_player_skill);
    if (swapped) {
      swapIntPointerContent(frst_player_skill, scd_player_skill);
    }
    if (manageAllAilments(frst_to_play, scd_to_play, swapped ^ true,
                          battle_pane)) {
      is_ko = isAnyKoPokemon(frst_to_play, scd_to_play);
      if (!is_ko) {
        failed =
            playOnePokemonTurn(frst_to_play, scd_to_play, *frst_player_skill,
                               swapped ^ true, battle_pane);
        is_ko = isAnyKoPokemon(frst_to_play, scd_to_play);
      }
    }
  }
  if (!is_ko) {
    if (manageAllAilments(scd_to_play, frst_to_play, swapped ^ false,
                          battle_pane)) {
      is_ko = isAnyKoPokemon(frst_to_play, scd_to_play);
      if (!is_ko) {
        failed =
            playOnePokemonTurn(scd_to_play, frst_to_play, *scd_player_skill,
                               swapped ^ false, battle_pane);
        is_ko = isAnyKoPokemon(frst_to_play, scd_to_play);
      }
    }
  }
  if (swapped) {
    swapPokemonPointerContent(frst_to_play, scd_to_play);
  }
  if (is_ko) {
    res = 1;
    if (enemy->stats.hp == 0) {
      addInfoText(WIN_STRING, WIN_STRING_LENGTH, " ", 1, battle_pane);
      clearAndPrintBattlePane(battle_pane);
      waitNMs(WAIT_BETWEEN_ANIM);
      if (gainExperience(&player->pkmns[0], enemy) == 1) {
        manageLevelUp(player, battle_pane, frst_to_play, scd_to_play);
      }
      player->money += 300 + enemy->evo_stage * (100 + enemy->lvl) + rand() % 21;
    } else { // test if the player has lost or has still some alive pokemon
      manageKoAnimation(player, battle_pane);
      if (hasAnyAlivePokemon(player)) {
        int targetted_pkmn = 0;
        managePokemonsMenu(player, 2, &targetted_pkmn);
        refreshBattlePane(player->pkmns[0], *enemy, battle_pane);
        clearAndPrintBattlePane(battle_pane);
        res = 0; // battle not finished there
      } else {
        addInfoText(LOST_STRING, LOST_STRING_LENGTH, " ", 1, battle_pane);
        clearAndPrintBattlePane(battle_pane);
        waitNMs(WAIT_BETWEEN_ANIM);
        for (int i = 0; i < player->pkmn_count; i++) {
          player->pkmns[i].stats.hp = 1;
        }
        player->money -= 400 + enemy->evo_stage * 100 + enemy->lvl + rand() % 21;
      }
    }
  }
  eraseInfoText(battle_pane);
  addArrow(ATTAQUES, battle_pane);
  *key_pressed_status = res;
}