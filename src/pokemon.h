#include "typedef.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef GAME_H
#define GAME_H

#define POKEDEX_LENGTH 151
#define PKMN_SKILL_LEARN_FREQU 6
#define SKILLS_COUNT 218
#define AILMENT_LABEL_LENGTH 3

void getRandomPokemonName(char **name);
void setSkill(Pokemon *pkmn, Skill skill, int index);
void fillSomePokemonData(FILE *file, Pokemon *pkmn);
void resetPokemonStatsAfterBattle(Pokemon *pkmn);
void resetPlayerPkmnsStatsAfterBattle(Player *player);
void setPokemonLvl(Pokemon *pkmn, int lvl);
bool containsSkill(Skill *skills, int skill_count, int skill_id);
void ailmentEnumToString(AilmentEnum ailment, char **ailment_string);
int getAilmentLabel(Pokemon pkmn, char **ailment_label);
void typeEnumToString(Type type, char *type_string);
void getTypesString(Pokemon pkmn, char *type_string);
void swapPokemon(Player *player, int src, int dest);
int getCurrentExperienceStage(Pokemon pkmn);
void upLevel(Pokemon *pkmn);
int getAverageLevel(Player *player);
int getKeyCount(Player *player);
int getMaxLevel(Player *player);
int gainExperience(Pokemon *winner, Pokemon *loser);
void addAilment(Pokemon *pkmn, AilmentEnum ailment);
void removeFirstAilment(Pokemon *pkmn);
void removeAllAilments(Pokemon *pkmn);
void resetAilementStatDebuf(Pokemon *pkmn, AilmentEnum ailment);
void setBackHealthToZero(Pokemon *pkmn);
void heal(Pokemon *pkmn, int heal);
void resetPokemonStats(Pokemon *pkmn);
void copyPokemon(Pokemon pkmn, Pokemon *copy);
void resetAllPokemonsStats(Player *player);
Skill fillSkill(int random);
Pokemon fillPokemon(char *name);

#endif