#include "typedef.h"
#include <stdbool.h>
#include <stdio.h>

#ifndef BATTLE_PANE_H
#define BATTLE_PANE_H

#define BATTLE_PANE_PATH "data/battle_pane.txt"
#define TYPE_COUNT 18

#define USE_POTION "Vous utilisez une potion"
#define USE_POTION_LENGTH 24
#define USE_TOTAL_SOIN "Vous utilisez un total soin"
#define USE_TOTAL_SOIN_LENGTH 27
#define THROW_POKEBALL "Vous lancez une Pokeball !"
#define THROW_POKEBALL_LENGTH 26
#define GRATZ_CATCH "Bravo ! Vous avez capturez"
#define GRATZ_CATCH_LENGTH 26
#define FAIL_CATCH "de la Pokeball"
#define FAIL_CATCH_LENGTH 14
#define TO_MUCH_POKEMON1 "Vous ne pouvez pas avoir"
#define TO_MUCH_POKEMON1_LENGTH 24
#define TO_MUCH_POKEMON2 "plus de 6 pokemons"
#define TO_MUCH_POKEMON2_LENGTH 18
#define CANT_FLY "Fuite impossible !"
#define CANT_FLY_LENGTH 18

#define PLAYER_PKMN_NAME_POS 716
#define PLAYER_HLTH_BAR_STRTNG_POS 764
#define PLAYER_HLTH_POINT_POS 854

#define ENEMY_PKMN_NAME_ENDNG_POS 108
#define ENEMY_HLTH_BAR_STRTNG_POS 175
#define ENEMY_HLTH_POINT_ENDING_POS 246

#define FRST_TEXT_LINE_POS 1037
#define SCD_TEXT_LINE_POS 1106
#define TEST_LINE_MAX_LGTH 34

#define FRST_PIPE_MENU 1004
#define NEXT_LINE 69

#define CURSOR_FRST_SKILL_LINE 1097
#define CURSOR_SCD_SKILL_LINE 1166
#define LEFT_SHIFT 3

#define MENU_ARROW_LGTH 2
#define HLTH_BAR_LGTH 24 // counting '{' and '}'
#define BATTLE_LINE_LGTH 67
#define BATTLE_PANE_LGTH 1240

#define BATTLE_PERCENTAGE 14

typedef enum {
  HIDDEN = 0,
  ATTAQUES = 1076,
  SAC = 1089,
  POKEMON = 1145,
  FUITE = 1158
} MenuArrow;

typedef struct {
  int *targetted_skill;
  Player *player;
  Pokemon *enemy;
  char *battle_pane;
  int *key_pressed_status;
  bool player_turn;
} PlayTurnParameters;

void createThreadNW8(void *function, void *function_params);
void eraseInfoText();
int switchThenManageSkillMenu(Player *player, char *battle_pane, Pokemon *enemy,
                              Skill *new_skill, int state);
void setArrowYesNo(char *battle_pane, int yes);
void showYesNoMenu(char *battle_pane);
int nDigits(unsigned int n);
bool hasAnyAlivePokemon(Player *player);
void clearAndPrintBattlePane(char *battle_pane);
void addInfoTextClearAndWait(char *top_text, int top_text_length,
                             char *bot_text, int bot_text_length,
                             char *battle_pane, int anim_wait);
void printBattlePane(char *battle_pane);
void loadBattlePane(char *battle_pane);
void addInfoText(char *top_text, int top_text_length, char *bot_text,
                 int bot_text_length, char *battle_pane);
void createHealthBar(char *health_bar, Pokemon pkmn);
void addPokemonName(Pokemon pkmn, char *battle_pane, int pos);
void addPokemonHealthBar(Pokemon pkmn, char *battle_pane, int pos);
void addPokemonHpPoint(Pokemon pkmn, char *battle_pane, int pos);
void eraseArrayLine(int pos, char *battle_pane, int space_count);
void refreshBattlePane(Pokemon player, Pokemon enemy, char *battle_pane);
void compareAndPlaceSkills(int *cursor_first_skills_line,
                           int *cursor_second_skills_line, int i,
                           Player *player, char *battle_pane);
void placeTwoSkills(Player *player, char *battle_pane, int i,
                    int *cursor_first_skills_line,
                    int *cursor_second_skills_line, int arrows_xys[]);
void changeMenuToSkillMenu(Player *player, char *battle_pane, int arrows_xys[]);
void setMenuBack(char *battle_pane, int *arrows_xys, MenuArrow arrow);
int manageSkillMenuKeyPressed(int *targetted_skill, int key_pressed,
                              int arrows_xys[], char *battle_pane,
                              Player *player, Pokemon *enemy, int state);
int manageMenuChoice(MenuArrow *arrow, char *battle_pane, Player *player,
                     Pokemon *enemy, bool flee_possible);
int manageBattleMenuKeyPressed(char key_pressed, MenuArrow *arrow,
                               char *battle_pane, Player *player,
                               Pokemon *enemy, bool flee_possible);
void battle(Player *player, int *x_map, int *y_map);
void printAndManageBattlePane(char *battle_pane, Player *player, Pokemon *enemy,
                              bool flee_possible);
bool goForBattle(Player *player, char *printable_map, int *x_map, int *y_map, char *dialog_box);
void isBattle(Player *player, char *printable_map, int *x_map, int *y_map, char *dialog_box);
void addAilments(Pokemon pkmn, char *battle_pane, int pos, bool left_erase);

#endif