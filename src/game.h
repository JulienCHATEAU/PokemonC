#ifndef GAME_H
#define GAME_H



void getRandomPokemonName(char **name);
void fillSomePokemonData(FILE *file, Pokemon *pkmn);
void resetPokemonStatsAfterBattle(Pokemon *pkmn);
void resetPlayerPkmnsStatsAfterBattle(Player *player);
void setPokemonLvl(Pokemon *pkmn, int lvl);
void ailmentEnumToString(AilmentEnum ailment, char **ailment_string);
int getAilmentLabel(Pokemon pkmn, char **ailment_label);
void typeEnumToString(Type type, char *type_string);
void getTypesString(Pokemon pkmn, char *type_string);
void swapPokemon(Player *player, int src, int dest);
int getCurrentExperienceStage(Pokemon pkmn);
void upLevel(Pokemon *pkmn);
int gainExperience(Pokemon *winner, Pokemon *loser);
void setPlayerPseudo(char *pseudo, Player *player);
void addAilment(Pokemon *pkmn, AilmentEnum ailment);
void removeFirstAilment(Pokemon *pkmn);
void setBackHealthToZero(Pokemon *pkmn);
void resetPokemonStats(Pokemon *pkmn);
void resetAllPokemonsStats(Player *player);

#endif