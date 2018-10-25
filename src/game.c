#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fileManager.h"
#include "game.h"
#include "startMenu.h"
#include "print.h"
#include "battlePane.h"
#include "ttyraw.h"


void getRandomPokemonName(char **name) {
  srand(time(NULL));
  int random = rand()%POKEDEX_LENGTH;
  FILE *pokemons_file = openFile(POKEMONS_PATH, "r");
  int i = 0;
  while (i < random) {
    fscanf(pokemons_file, "%*[^\n]\n");
    i++;
  }
  int name_length = 0;
  fscanf(pokemons_file, "%d ", &name_length);
  *name = malloc(sizeof(char)*name_length);
  fscanf(pokemons_file, "%[^|]|", *name);
  closeFile(pokemons_file);
}

/* Fills a Skill structure with the data stored in the skills file storage
   'SKILLS_PATH' according to the name given in parameters.
The syntax of a line of the file is :
   'name_length name|power accuracy pp type priority|description_length description'
* name : the name of the skill to store data
* return the Skill structure filled
*/
Skill fillSkill(int random) {
  Skill skill;
  FILE *skills_file = openFile(SKILLS_PATH, "r");
  int type = 0;
  int i = 0;
  while (i < random) {
    fscanf(skills_file, "%*[^\n]\n");
    i++;
  }
  fscanf(skills_file, "%d ", &skill.name_length);
  skill.name = malloc(sizeof(char)*skill.name_length+5);
  fscanf(skills_file, "%[^|]", skill.name);
  fscanf(skills_file, "|%d ", &skill.power);
  fscanf(skills_file, "%d ", &skill.accuracy_max);
  skill.accuracy = skill.accuracy_max;
  fscanf(skills_file, "%d ", &skill.pp_max);
  skill.pp = skill.pp_max;
  fscanf(skills_file, "%d ", &type);
  skill.type = (Type) type;
  fscanf(skills_file, "%d ", &skill.priority);
  Effect effect;
  int stat_enum;
  fscanf(skills_file, "|%d ", &stat_enum);
  effect.stat_to_change = (StatEnum) stat_enum;
  fscanf(skills_file, "%d %d", &effect.change_value, &effect.accuracy);
  skill.effect = effect;
  fscanf(skills_file, "|%d %d", &skill.ailment.ailment_enum, &skill.ailment.accuracy);
  fscanf(skills_file, "|%d", &skill.effect.target);
  fscanf(skills_file, "|%d", &skill.description_length);
  skill.description = malloc(sizeof(char)*skill.description_length+5);
  fscanf(skills_file, " %[^\n]\n", skill.description);
  skill.id = random;
  closeFile(skills_file);
  return skill;
}

/* Fills some data of a pokemon starting from lvl to max stats
* file : the file stream to read the data
* pkmn : the pokemon structure to fill
*/
void fillSomePokemonData(FILE *file, Pokemon *pkmn) {
  int stats[4];
  int type = 0;
  fscanf(file, "%d ", &pkmn->lvl);
  fscanf(file, "%d ", &pkmn->xp);
  fscanf(file, "%d", &type);
  pkmn->type[0] = (Type) type;
  fscanf(file, "%d", &type);
  pkmn->type[1] = (Type) type;
  int done[4];
  int random;
  srand(time(NULL));
  for (int i = 0; i < 4; i++) {
    do {
      random = rand()%SKILLS_COUNT;
    } while (contains(done, i, random));
    done[i] = random;
    pkmn->skills[i] = fillSkill(random);
  }
  pkmn->crt_ailments[0] = NO_AILMENT;
  pkmn->crt_ailments[1] = NO_AILMENT;
  fscanf(file, "|%d", &pkmn->base_xp);
  fscanf(file, "|%d %d %d %d", &stats[0], &stats[1], &stats[2], &stats[3]);
  pkmn->stats = createStats(25+stats[0]/5, 25+stats[1]/5, 25+stats[2]/5, 25+stats[3]/5);
}

/* Fills a Pokemon structure with the data stored in the pokemons file storage
   'POKEMONS_PATH' according to the name given in parameters
The syntax of a line of the file is :
   'name_length name|lvl xp type[0] type[1]|skill[0]|skill[1]|skill[2]|skill[3]|base_xp|hp att def spd'
* name : the name of the pokemon to store data
* return the Pokemon structure filled
*/
Pokemon fillPokemon(char *name) {
  Pokemon pkmn;
  FILE *pkmns_file = openFile(POKEMONS_PATH, "r");
  Boolean trouve = false;
  int fscanf_ret = 0;
  while (!trouve && fscanf_ret != EOF) {
    fscanf_ret = fscanf(pkmns_file, "%d ", &pkmn.name_length);
    pkmn.name = malloc(sizeof(char)*pkmn.name_length+1);
    fscanf(pkmns_file, "%[^|]|", pkmn.name);
    if (strcmp(pkmn.name, name) == 0) {
      trouve = true;
      fillSomePokemonData(pkmns_file, &pkmn);
    } else {
      free(pkmn.name);
      fscanf(pkmns_file, "%*[^\n]\n");
    }
  }
  closeFile(pkmns_file);
  for (int i = 0; i < 4; i++) {
    pkmn.effect_stage[i] = 0;
  }
  return pkmn;
}


/* Resets some stats of a pokemon (after a battle)
* pkmn : the concerned pokemon
*/
void resetPokemonStatsAfterBattle(Pokemon *pkmn) {
  pkmn->stats.att = pkmn->stats.att_max;
  pkmn->stats.def = pkmn->stats.def_max;
  pkmn->stats.spd = pkmn->stats.spd_max;
  pkmn->crt_ailments[1] = NO_AILMENT;
  if (pkmn->crt_ailments[0] == CONFUSION) {
    pkmn->crt_ailments[0] = NO_AILMENT;
  }
  if (pkmn->crt_ailments[0] == BURN) {
    pkmn->stats.att = pkmn->stats.att_max/2;
  }
  if (pkmn->crt_ailments[0] == PARALYSIS) {
    pkmn->stats.spd = pkmn->stats.spd_max/4;
  }
  for (int i = 0; i < 4; i++) {
    pkmn->skills[i].accuracy = pkmn->skills[i].accuracy_max;
    pkmn->effect_stage[i] = 0;
  }
}

/* Resets some stats of all the pokemons of a player (after a battle)
* player : the player
*/
void resetPlayerPkmnsStatsAfterBattle(Player *player) {
  for (int i = 0; i < player->pkmn_count; i++) {
    resetPokemonStatsAfterBattle(&player->pkmns[i]);
  }
}

/* Ups the level of a pokemon a chosen number of time
* pkmn : the concerned pokemon
* lvl : the new level of the pokemon
*/
void setPokemonLvl(Pokemon *pkmn, int lvl) {
  while (pkmn->lvl < lvl-1) {
    upLevel(pkmn);
  }
  pkmn->xp = getCurrentExperienceStage(*pkmn);
  upLevel(pkmn);
}


/* Converts the given ailment to a string
* ailement : the ailement
* ailment_string : the adress of a char array to store the enum string
*/
void ailmentEnumToString(AilmentEnum ailment, char **ailment_string) {
  *ailment_string = malloc(sizeof(char)*3);
  switch (ailment) {
    case PARALYSIS:*ailment_string = "PAR";break;
    case SLEEP:*ailment_string = "SOM";break;
    case FREEZE:*ailment_string = "GEL";break;
    case BURN:*ailment_string = "BRU";break;
    case POISONING:*ailment_string = "PSN";break;
    case CONFUSION:*ailment_string = "CNF";break;
  }
}

/* Get the label of the currents aliment of a pokemon
* pkmn : the pokemon
* ailment_label : the adress of a char array to store the label
* return the length of the created label
*/
int getAilmentLabel(Pokemon pkmn, char **ailment_label) {
  int ailment_label_length = 1 + AILMENT_LABEL_LENGTH + 1;//1 + 1 -> [ailment] for the two brackets
  int label_length = 0;
  char *first_ailment;
  ailmentEnumToString(pkmn.crt_ailments[0], &first_ailment);
  if (pkmn.crt_ailments[1] == CONFUSION) {
    label_length = ailment_label_length + 1 + ailment_label_length;//1 for space
    *ailment_label = malloc(sizeof(char)*label_length);
    char *confusion_ailment;
    ailmentEnumToString(CONFUSION, &confusion_ailment);
    sprintf(*ailment_label, "[%s] [%s]", confusion_ailment, first_ailment);
  } else if (pkmn.crt_ailments[0] != NO_AILMENT) {
    label_length = ailment_label_length;
    *ailment_label = malloc(sizeof(char)*label_length);
    sprintf(*ailment_label, "[%s]", first_ailment);
  } else {
    label_length = 1;
    *ailment_label = malloc(sizeof(char)*label_length);
    *ailment_label = " ";
  }
  //TODO : try to free first_ailment confusion_ailment
  return label_length;
}

/* Converts the given type to a string
* type : the type
*/
void typeEnumToString(Type type, char *type_string) {
  switch (type) {
    case NORMAL: setStringToTab(type_string, "NORMAL");break;
    case FIGHTING: setStringToTab(type_string, "COMBAT");break;
    case FLYING: setStringToTab(type_string, "VOL");break;
    case POISON: setStringToTab(type_string, "POISON");break;
    case GROUND: setStringToTab(type_string, "SOL");break;
    case ROCK: setStringToTab(type_string, "ROCHE");break;
    case BUG: setStringToTab(type_string, "INSECTE");break;
    case GHOST: setStringToTab(type_string, "SPECTRE");break;
    case STEEL: setStringToTab(type_string, "ACIER");break;
    case FIRE: setStringToTab(type_string, "FEU");break;
    case WATER: setStringToTab(type_string, "EAU");break;
    case GRASS: setStringToTab(type_string, "PLANTE");break;
    case ELECTRIC: setStringToTab(type_string, "ELECTRIQUE");break;
    case PSYCHIC: setStringToTab(type_string, "PSY");break;
    case ICE: setStringToTab(type_string, "GLACE");break;
    case DRAGON: setStringToTab(type_string, "DRAGON");break;
    case DARK: setStringToTab(type_string, "TENEBRE");break;
    case FAIRY: setStringToTab(type_string, "FEE");break;
  }
}

/* Gets a string describing the types of a pokemon
* pkmn : the pokemon
*/
void getTypesString(Pokemon pkmn, char *type_string) {
  typeEnumToString(pkmn.type[0], type_string);
  if (pkmn.type[1] != NO_TYPE) {
    char scd_type_string[25];
    typeEnumToString(pkmn.type[1], scd_type_string);
    sprintf(type_string, "%s - %s", type_string, scd_type_string);
  }
}

/* Swaps two player's pokemon order
* player : the player
* src : the index of the first pokemon
* dest : the index of the second pokemon
*/
void swapPokemon(Player *player, int src, int dest) {
  Pokemon tmp = player->pkmns[src];
  player->pkmns[src] = player->pkmns[dest];
  player->pkmns[dest] = tmp;
}

/* Gets the experience count needed to make the pokemon level-upping
* pkmn : the concerned pokemon
* return the experience count needed to make the pokemon level-upping
*/
int getCurrentExperienceStage(Pokemon pkmn) {
  int lvl_plus_one = pkmn.lvl+1;
  return lvl_plus_one * lvl_plus_one * lvl_plus_one;
}

/* Makes the pokemon level-upping
* pkmn : the concernded pokemon
*/
void upLevel(Pokemon *pkmn) {
  pkmn->lvl++;
  int rand_hp = rand()%3+1;
  int rand_att = rand()%3+1;
  int rand_def = rand()%3+1;
  int rand_spd = rand()%3+1;
  pkmn->stats.hp_max += rand_hp;
  pkmn->stats.hp += rand_hp;
  pkmn->stats.att_max += rand_att;
  pkmn->stats.att += rand_att;
  pkmn->stats.def_max += rand_def;
  pkmn->stats.def += rand_def;
  pkmn->stats.spd_max += rand_spd;
  pkmn->stats.spd += rand_spd;
}

/* Gives experience to a pokemon depending on loser's base experience
* winner : the pokemon who won the battle
* loser : the pokemon who lost the battle
* return 1 if the pokemon level upped, 0 otherwise
*/
int gainExperience(Pokemon *winner, Pokemon *loser) {
  int level_up = 0;
  int crt_xp_stage = getCurrentExperienceStage(*winner);
  int xp = loser->base_xp * loser->lvl / 7;
  if (winner->xp+xp >= crt_xp_stage) {
    upLevel(winner);
    level_up = 1;
  }
  winner->xp += xp;
  return level_up;
}


void setPlayerPseudo(char *pseudo, Player *player) {
  int i = 0;
  while (pseudo[i] != '\0') {
    player->pseudo[i] = pseudo[i];
    i++;
  }
  player->pseudo_length = i;
}


/* Adds an ailment to the pokemon
* pkmn : the pokemon
* ailment : the ailment
*/
void addAilment(Pokemon *pkmn, AilmentEnum ailment) {
  if (pkmn->crt_ailments[0] == NO_AILMENT) {
    pkmn->crt_ailments[0] = ailment;
  } else if (pkmn->crt_ailments[0] == CONFUSION) {
    pkmn->crt_ailments[1] = CONFUSION;
    pkmn->crt_ailments[0] = ailment;
  }
  if (pkmn->crt_ailments[0] == BURN) {
    pkmn->stats.att /= 2;
  }
  if (pkmn->crt_ailments[0] == PARALYSIS) {
    pkmn->stats.spd /= 4;
  }
}

void removeFirstAilment(Pokemon *pkmn) {
  if (pkmn->crt_ailments[0] == BURN) {
    pkmn->stats.att *= 2;
  }
  if (pkmn->crt_ailments[0] == PARALYSIS) {
    pkmn->stats.spd *= 4;
  }
  pkmn->crt_ailments[0] = pkmn->crt_ailments[1];
  pkmn->crt_ailments[1] = NO_AILMENT;
}

void setBackHealthToZero(Pokemon *pkmn) {
  if (pkmn->stats.hp < 0) {
    pkmn->stats.hp = 0;
  }
}

/* Heals and resets the stats of a pokemon
* pkmn : the concerned pokemon
*/
void resetPokemonStats(Pokemon *pkmn) {
  pkmn->stats.hp = pkmn->stats.hp_max;
  pkmn->stats.att = pkmn->stats.att_max;
  pkmn->stats.def = pkmn->stats.def_max;
  pkmn->stats.spd = pkmn->stats.spd_max;
  pkmn->crt_ailments[0] = NO_AILMENT;
  pkmn->crt_ailments[1] = NO_AILMENT;
  for (int i = 0; i < 4; i++) {
    pkmn->skills[i].pp = pkmn->skills[i].pp_max;
    pkmn->skills[i].accuracy = pkmn->skills[i].accuracy_max;
  }
}

/* Heals and resets the stats of all the pokemons of the player
* player : the player
*/
void resetAllPokemonsStats(Player *player) {
  for (int i = 0; i < player->pkmn_count; i++) {
    resetPokemonStats(&player->pkmns[i]);
  }
}







