#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "pokemon.h"
#include "util.h"
#include "map.h"
#include "print.h"
#include "fileManager.h"
#include "battlePane.h"
#include "startMenu.h"
#include "battle.h"

/* The types table */
static int types_table[TYPES_TABLE_LGTH];//1 is x1, 2 is x2, 3 is x0.5
/* A boolean to describe if the type table is filled */
static Boolean types_table_filled = false;

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
* return a double related to the target weaknesses/resistances against the skill type
*/
double getWeakness(Type skill_type, Pokemon *target) {
  double weakness = 1.0;
  int weakness_box = 0;
  for (int i = 0; i < 2; i++) {
    if (target->type[i] != NO_TYPE) {
      weakness_box = (int) skill_type * TYPE_COUNT + (int) target->type[i];
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
There, we are considering that the frst_to_play Pokemon is faster than the scd_to_play Pokemon
so that this function only tests if this is true or not (swapping them if not)
* frst_player_skill : the skill chosen by the first player
* scd_player_skill : the skill chosen by the second player
* return true if the pkmn was swapped, false otherwise
*/
Boolean chosePlayOrder(Pokemon *frst_to_play, Pokemon *scd_to_play, int *frst_player_skill, int *scd_player_skill) {
  int swapped = false;
  int priority1 = frst_to_play->skills[*frst_player_skill].priority;
  int priority2 = scd_to_play->skills[*scd_player_skill].priority;
  if (priority1 = priority2) {
    if (scd_to_play->stats.spd > frst_to_play->stats.spd) {
      swapPokemonPointerContent(frst_to_play, scd_to_play);
      swapped = true;
    } else if (scd_to_play->stats.spd = frst_to_play->stats.spd) {
      if (rand()%100 < SAME_SPEED_RAND_PERCENTAGE) {
        swapPokemonPointerContent(frst_to_play, scd_to_play);
        swapped = true;
      }
    }
  } else if (priority1 < priority2) {
    swapPokemonPointerContent(frst_to_play, scd_to_play);
    swapped = true;
  } else {
  }
  return swapped;
}

/* Calculates the health points count the target will lose
* skill_user : the pokemon playing the turn
* target : the targetted pokemon
* skill : the chosen skill
* crit : true if the skill is a critical hit, false otherwise
* weakness : a double related to the target weaknesses/resistances. values used to be :
  0.25 if the target has 2 types resisting to the skill type
  0.50 if the target has 1 type resisting to the skill type
  1 if the target has no type resisting to the skill type
  2 if the target has 1 type weak against the skill type
  4 if the target has 2 type weak against the skill type
* return the health points count the target will lose
*/
int hpCountLost(Pokemon *skill_user, Pokemon *target, Skill skill, Boolean crit, double weakness) {
  double hp_count_lost = 0;
  if (skill.power > 0) {
    double stab = 1.0;
    double critical_hit = 1.0;
    if (crit) {
      critical_hit = 1.5;
    }
    if (skill_user->type[0] == skill.type || skill_user->type[1] == skill.type) {
      stab = 1.5;
    }
    double numerator = (skill_user->lvl * 0.4 + 2) * skill_user->stats.att * skill.power;
    double denominator = (target->stats.def * 50);
    hp_count_lost = (numerator / denominator + 2) * stab * weakness * critical_hit;
  }
  return (int)round(hp_count_lost);
}

void ailmentTextAnimation(Pokemon *skill_user, Pokemon *target, Boolean swapped_xor_frst, char *battle_pane, char *frst_ailment_text, int frst_ailment_text_length, char *scd_ailment_text, int scd_ailment_text_length) {
  int ailment_text_length = skill_user->name_length+frst_ailment_text_length+1;
  char *ailment_text = malloc(sizeof(char) * ailment_text_length);
  sprintf(ailment_text, "%s %s", skill_user->name, frst_ailment_text);

  int ailment_text_length2 = scd_ailment_text_length;
  char *ailment_text2 = malloc(sizeof(char) * ailment_text_length2);
  sprintf(ailment_text2, "%s", scd_ailment_text);

  addInfoText(ailment_text, ailment_text_length, ailment_text2, ailment_text_length2, battle_pane);
  if (swapped_xor_frst) {
    refreshBattlePane(*skill_user, *target, battle_pane);
  } else {
    refreshBattlePane(*target, *skill_user, battle_pane);
  }
  clearAndPrintBattlePane(battle_pane);
  free(ailment_text);
  free(ailment_text2);
  waitNMs(WAIT_BETWEEN_ANIM);
}

Boolean manageFirstAilment(Pokemon *skill_user, Pokemon *target, Boolean swapped_xor_frst, char *battle_pane) {
  Boolean play = true;
  srand(time(NULL));
  switch (skill_user->crt_ailments[0]) {
    case PARALYSIS:
    if (rand()%100 < 25) {
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, FRST_PAR_TEXT, FRST_PAR_TEXT_LENGTH, SCD_PAR_TEXT, SCD_PAR_TEXT_LENGTH);
      play = false;
    }
    break;

    case SLEEP:
    if (rand()%100 < 66) {
      play = false;
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, SLEEP_TEXT, SLEEP_TEXT_LENGTH, " ", 1);
    } else {
      removeFirstAilment(skill_user);
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, WAKE_UP_TEXT, WAKE_UP_TEXT_LENGTH, " ", 1);
    }
    break;

    case FREEZE:
    if (rand()%100 < 80) {
      play = false;
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, FREEZE_TEXT, FREEZE_TEXT_LENGTH, " ", 1);
    } else {
      removeFirstAilment(skill_user);
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, UNFREEZE_TEXT, UNFREEZE_TEXT_LENGTH, " ", 1);
    }
    break;

    case BURN:
    skill_user->stats.hp -= skill_user->stats.hp_max / 8;
    setBackHealthToZero(skill_user);
    ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, BURN_TEXT, BURN_TEXT_LENGTH, " ", 1);
    break;

    case POISONING:
    skill_user->stats.hp -= skill_user->stats.hp_max / (rand()%3+7);
    setBackHealthToZero(skill_user);
    ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, POISONNING_TEXT, POISONNING_TEXT_LENGTH, " ", 1);
    break;

    case CONFUSION:
    if (rand()%100 < 33) {
      removeFirstAilment(skill_user);
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, CONFUSION1_TEXT, CONFUSION1_TEXT_LENGTH, " ", 1);
    } else {
      ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, CONFUSION2_TEXT, CONFUSION2_TEXT_LENGTH, " ", 1);
      srand(time(NULL));
      if (rand()%100 < 50) {
        skill_user->stats.hp -= skill_user->stats.hp_max / 8;
        setBackHealthToZero(skill_user);
        play = false;
        ailmentTextAnimation(skill_user, target, swapped_xor_frst, battle_pane, CONFUSION3_TEXT, CONFUSION3_TEXT_LENGTH, " ", 1);
      }
    }
    break;
  }
  return play;
}


Boolean manageAllAilments(Pokemon *skill_user, Pokemon *target, Boolean swapped_xor_frst, char *battle_pane) {
  Boolean play = manageFirstAilment(skill_user, target, swapped_xor_frst, battle_pane);
  if (skill_user->crt_ailments[0] == CONFUSION) {
    play = manageFirstAilment(skill_user, target, swapped_xor_frst, battle_pane);
  }
  return play;
}

/* Manages the first information text animation
* skill_user : the pokemon whose using the skill
* chosen_skill : an integer representing the chosen skill
* battle_pane : the battle pane
*/
void manageFirstTextAnimation(Pokemon *skill_user, int chosen_skill, char *battle_pane) {
  Skill chosen = skill_user->skills[chosen_skill];
  int pkmn_use_skill_length = skill_user->name_length+9+chosen.name_length;
  char *pkmn_use_skill = malloc(sizeof(char) * pkmn_use_skill_length);
  sprintf(pkmn_use_skill, "%s utilise %s", skill_user->name, chosen.name);
  addInfoText(pkmn_use_skill, pkmn_use_skill_length, " ", 1, battle_pane);
  clearAndPrintBattlePane(battle_pane);
  free(pkmn_use_skill);
  waitNMs(WAIT_BETWEEN_ANIM);
}


/* Manages the second information text animation
* battle_pane : the battle pane
* crit : this boolean is true if the skill is a critical hit, false otherwise
* weakness : the weakness or resistance coefficient
* return true if something was written, false otherwise
*/
Boolean manageSecondTextAnimation(char *battle_pane, Boolean crit, double weakness) {
  Boolean written_something = true;
  if (!crit && weakness != 1.0) {
    if (weakness > 1.0) {
      addInfoText(SUPER_EFFECTIVE_STRING, SUPER_EFFECTIVE_STRING_LENGTH, " ", 1, battle_pane);
    } else {
      addInfoText(NOT_EFFECTIVE_STRING, NOT_EFFECTIVE_STRING_LENGTH, " ", 1, battle_pane);
    }
  } else if (crit && weakness == 1.0) {
    addInfoText(CRIT_STRING, CRIT_STRING_LENGTH, " ", 1, battle_pane);
  } else if (crit && weakness != 1.0) {
    if (weakness > 1.0) {
      addInfoText(CRIT_STRING, CRIT_STRING_LENGTH, SUPER_EFFECTIVE_STRING, SUPER_EFFECTIVE_STRING_LENGTH, battle_pane);
    } else {
      addInfoText(CRIT_STRING, CRIT_STRING_LENGTH, NOT_EFFECTIVE_STRING, NOT_EFFECTIVE_STRING_LENGTH, battle_pane);
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
Boolean manageEffect(int chosen_skill, Pokemon *skill_user, Pokemon *target, char *battle_pane) {
  Skill chosen = skill_user->skills[chosen_skill];
  Effect effect = chosen.effect;
  Pokemon *effect_target = skill_user;
  int message_length = 6; //"  de l" length
  char *message;
  int written_something = false;
  printf("%d\n", effect.stat_to_change);
  if (effect.stat_to_change != NO_STATENUM && rand()%100 < effect.accuracy) {
    written_something = true;
    double base = 1;
    if (effect.target == 0) {
      effect_target = target;
    }
   effect_target->effect_stage[(int)effect.stat_to_change] += effect.change_value;
   for (int i = 0; i < abs(effect_target->effect_stage[(int)effect.stat_to_change]); i++) {
     base += 0.5;
   }
   if (effect.change_value < 0) {
     base = 1 / base;
   }
   switch (effect.stat_to_change) {
     case ATT:
     effect_target->stats.att = effect_target->stats.att_max * base;
     if (effect.change_value < 0) {
       message_length += effect_target->name_length + 8 + 4;//8 -> "'attaque" | 4 -> "perd"
       message = malloc(sizeof(char) * message_length);
       sprintf(message, "%s perd de l'attaque", effect_target->name);
     } else {
       message_length += effect_target->name_length + 8 + 5;//8 -> "'attaque" | 5 -> "gagne"
       message = malloc(sizeof(char) * message_length);
       sprintf(message, "%s gagne de l'attaque", effect_target->name);
     }
     break;

     case DEF:
     effect_target->stats.def = effect_target->stats.def_max * base;
     if (effect.change_value < 0) {
       message_length += effect_target->name_length + 9 + 4;//9 -> "a défense" | 4 -> "perd"
       message = malloc(sizeof(char) * message_length);
       sprintf(message, "%s perd de la defense", effect_target->name);
     } else {
       message_length += effect_target->name_length + 9 + 5;//9 -> "a défense" | 5 -> "gagne"
       message = malloc(sizeof(char) * message_length);
       sprintf(message, "%s gagne de la defense", effect_target->name);
     }
     break;

     case SPD:
     effect_target->stats.spd = effect_target->stats.spd_max * base;
     if (effect.change_value < 0) {
       message_length += effect_target->name_length + 9 + 4;//9 -> "a vitesse" | 4 -> "perd"
       message = malloc(sizeof(char) * message_length);
       sprintf(message, "%s perd de la vitesse", effect_target->name);
     } else {
       message_length += effect_target->name_length + 9 + 5;//9 -> "a vitesse" | 5 -> "gagne"
       message = malloc(sizeof(char) * message_length);
       sprintf(message, "%s gagne de la vitesse", effect_target->name);
     }
     break;

     case ACC:
     for (int i = 0; i < 4; i++) {
       effect_target->skills[i].accuracy = effect_target->skills[i].accuracy_max * base;
     }
     if (effect.change_value < 0) {
       message_length += effect_target->name_length + 11 + 4;//11 -> "a precision" | 4 -> "perd"
       message = malloc(sizeof(char) * message_length);
       sprintf(message, "%s perd de la precision", effect_target->name);
     } else {
       message_length += effect_target->name_length + 11 + 5;//11 -> "a precision" | 5 -> "gagne"
       message = malloc(sizeof(char) * message_length);
       sprintf(message, "%s gagne de la precision", effect_target->name);
     }
     break;

     default:
     printf("ERROR MANAGING EFFECT\n");
     exit(4);
     break;
   }
   addInfoText(message, message_length, " ", 1, battle_pane);
   free(message);
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
* swapped_xor_frst : this boolean describe whether the player's pokemon was swapped or not and whether it's the first to play or not (see playTurn())
  this is used to know if the player is pointed by the 'skill_user' parameter or the 'target' one
* return true if the skill has failed, false otherwise
*/
Boolean useSkill(Pokemon *skill_user, Pokemon *target, int chosen_skill, char *battle_pane, Boolean swapped_xor_frst) {
  Skill chosen = skill_user->skills[chosen_skill];
  fillTypesTable();
  Boolean failed = false;
  if (rand()%100 < chosen.accuracy) {
    Boolean crit = false;
    double weakness = getWeakness(chosen.type, target);
    if (rand()%100 < CRIT_PERCENTAGE) {
      crit = true;
    }
    target->stats.hp -= hpCountLost(skill_user, target, chosen, crit, weakness);
    if (target->stats.hp < 0) {
      target->stats.hp = 0;
    }
    if (chosen.power == 0) {
      crit = false;
      weakness = 1.0;
    }
    Boolean written_something = manageSecondTextAnimation(battle_pane, crit, weakness);
    if (rand()%100 < chosen.ailment.accuracy) {
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
      written_something = manageEffect(chosen_skill, skill_user, target, battle_pane);
      eraseInfoText(battle_pane);
      clearAndPrintBattlePane(battle_pane);
    }
    if (!written_something && !isAnyKoPokemon(skill_user, target)) {
      waitNMs(WAIT_BETWEEN_ANIM);
    }
  } else {
    failed = true;
    int name_miss_string_length = skill_user->name_length+MISS_STRING_LENGTH;
    char *name_miss_string = malloc(sizeof(char)*name_miss_string_length);
    sprintf(name_miss_string, "%s%s", skill_user->name, MISS_STRING);
    addInfoText(name_miss_string, name_miss_string_length, " ", 1, battle_pane);
    clearAndPrintBattlePane(battle_pane);
    waitNMs(WAIT_BETWEEN_ANIM);
    free(name_miss_string);
  }
  if (chosen.pp > 0) {
    chosen.pp--;
  }
  return failed;
}

/* Checks if any of the two pokemon is KO
* p1 : the first pokemon
* p2 : the second pokemon
* return true if on of the two pokemons is KO, false otherwise
*/
Boolean isAnyKoPokemon(Pokemon *p1, Pokemon *p2) {
  return p1->stats.hp == 0 || p2->stats.hp == 0;
}

/* Plays the turn of one pokemon
* skill_user : the pokemon playing the turn
* target : the targetted pokemon
* chosen_skill : an integer representing the chosen skill
* swapped_xor_frst : this boolean describe whether the player's pokemon was swapped or not and whether it's the first to play or not (see playTurn())
  this is used to know if the player is pointed by the 'skill_user' parameter or the 'target' one
* battle_pane : the battle pane
* return true if the skill failed, false otherwise
*/
Boolean playOnePokemonTurn(Pokemon *skill_user, Pokemon *target, int chosen_skill, Boolean swapped_xor_frst, char *battle_pane) {
  manageFirstTextAnimation(skill_user, chosen_skill, battle_pane);
  Boolean failed = useSkill(skill_user, target, chosen_skill, battle_pane, swapped_xor_frst);
  return failed;
}

/* Plays a battle turn (this function is used in a thread)
* p : p is a PlayTurnParameters structure
*/
void *playTurn(void *p) {
  PlayTurnParameters *params = p;
  int *targetted_skill = params->targetted_skill;
  Player *player = params->player;
  Pokemon *enemy = params->enemy;
  char *battle_pane = params->battle_pane;
  int *key_pressed_status = params->key_pressed_status;
  Boolean player_turn = params->player_turn;

  int res = 0;
  //considering firstly that the player is faster than the enemy
  Pokemon *frst_to_play = &player->pkmns[0];
  Pokemon *scd_to_play = enemy;
  int *frst_player_skill = malloc(sizeof(int));
  int *scd_player_skill = malloc(sizeof(int));
  *frst_player_skill = *targetted_skill;
  *scd_player_skill = rand()%4;
  while (scd_to_play->skills[*scd_player_skill].pp == 0) {
    *scd_player_skill = rand()%4;
  }

  Boolean swapped = false;
  Boolean failed = false;
  Boolean is_ko = false;
  if (player_turn) {//if the turn of the player is skipped
    swapped = chosePlayOrder(frst_to_play, scd_to_play, frst_player_skill, scd_player_skill);
    if (swapped) {
      swapIntPointerContent(frst_player_skill, scd_player_skill);
    }
    if (manageAllAilments(frst_to_play, scd_to_play, swapped^true, battle_pane)) {
      is_ko = isAnyKoPokemon(frst_to_play, scd_to_play);
      if (!is_ko) {
        failed = playOnePokemonTurn(frst_to_play, scd_to_play, *frst_player_skill, swapped^true, battle_pane);
        is_ko = isAnyKoPokemon(frst_to_play, scd_to_play);
      }
    }
  }
  if (!is_ko) {
    if (manageAllAilments(scd_to_play, frst_to_play, swapped^false, battle_pane)) {
      is_ko = isAnyKoPokemon(frst_to_play, scd_to_play);
      if (!is_ko) {
        failed = playOnePokemonTurn(scd_to_play, frst_to_play, *scd_player_skill, swapped^false, battle_pane);
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
      if (gainExperience(&player->pkmns[0], enemy) == 1) {
        int level_up_length = player->pkmns[0].name_length+17+nDigits(player->pkmns[0].lvl);
        char *lvl_up = malloc(sizeof(char)*level_up_length);
        sprintf(lvl_up, "%s passe au niveau %d", player->pkmns[0].name, player->pkmns[0].lvl);
        addInfoText(lvl_up, level_up_length, " ", 1, battle_pane);
        free(lvl_up);
        refreshBattlePane(*frst_to_play, *scd_to_play, battle_pane);
        clearAndPrintBattlePane(battle_pane);
        waitNMs(WAIT_BETWEEN_ANIM);
      }
      addInfoText(WIN_STRING, WIN_STRING_LENGTH, " ", 1, battle_pane);
      clearAndPrintBattlePane(battle_pane);
      waitNMs(WAIT_BETWEEN_ANIM);
      resetPlayerPkmnsStatsAfterBattle(player);
      player->money += 200 + enemy->lvl + rand()%21;
    } else {//test if the player has lost or has still some alive pokemon
      int is_ko_length = player->pkmns[0].name_length+9;
      char *is_ko = malloc(sizeof(char)*is_ko_length);
      sprintf(is_ko, "%s est K.O.", player->pkmns[0].name);
      addInfoText(is_ko, is_ko_length, " ", 1, battle_pane);
      free(is_ko);
      clearAndPrintBattlePane(battle_pane);
      waitNMs(WAIT_BETWEEN_ANIM+200);
      if (hasAnyAlivePokemon(player)) {
        managePokemonsMenu(player, 2);
        refreshBattlePane(player->pkmns[0], *enemy, battle_pane);
        clearAndPrintBattlePane(battle_pane);
        res = 0;//battle not finished there
      } else {
        addInfoText(LOST_STRING, LOST_STRING_LENGTH, " ", 1, battle_pane);
        clearAndPrintBattlePane(battle_pane);
        waitNMs(WAIT_BETWEEN_ANIM);
        resetPlayerPkmnsStatsAfterBattle(player);
        for (int i = 0; i < player->pkmn_count; i++) {
          player->pkmns[i].stats.hp = 1;
        }
        player->money += 300 + enemy->lvl + rand()%21;
        if (player->money < 0) {
          player->money = 0;
        }
      }
    }
  }
  eraseInfoText(battle_pane);
  addArrow(ATTAQUES, battle_pane);
  free(frst_player_skill);
  free(scd_player_skill);
  *key_pressed_status = res;
}