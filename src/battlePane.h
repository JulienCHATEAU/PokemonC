#include <stdbool.h>

#ifndef BATTLE_PANE_H
#define BATTLE_PANE_H

#define BATTLE_PANE_PATH "data/battlePane.txt"
#define TYPE_COUNT 18

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

#define PLAYER_PKMN_NAME_POS 707
#define PLAYER_HLTH_BAR_STRTNG_POS 753
#define PLAYER_HLTH_POINT_POS 843

#define ENEMY_PKMN_NAME_ENDNG_POS 107
#define ENEMY_HLTH_BAR_STRTNG_POS 173
#define ENEMY_HLTH_POINT_ENDING_POS 243

#define FRST_TEXT_LINE_POS 1022
#define SCD_TEXT_LINE_POS 1090
#define TEST_LINE_MAX_LGTH 34
#define FRST_DB_POS 952

#define FRST_PIPE_MENU 990
#define NEXT_LINE 68

#define CURSOR_FRST_SKILL_LINE 1082
#define CURSOR_SCD_SKILL_LINE 1150
#define LEFT_SHIFT 3

#define MENU_ARROW_LGTH 2
#define HLTH_BAR_LGTH 24 //counting '{' and '}'
#define BATTLE_LINE_LGTH 66
#define BATTLE_PANE_LGTH 1223

#define BATTLE_PERCENTAGE 14

typedef enum {
  HIDDEN = 0,
  ATTAQUES = 1061,
  SAC = 1074,
  POKEMON = 1129,
  FUITE = 1142
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
int switchThenManageSkillMenu(Player *player, char *battle_pane, Pokemon *enemy, Skill *new_skill, int state);
void setArrowYesNo(char *battle_pane, int yes);
void showYesNoMenu(char *battle_pane);
int nDigits(unsigned int n);
bool hasAnyAlivePokemon(Player *player);
void clearAndPrintBattlePane(char *battle_pane);
void printBattlePane(char *battle_pane);
void loadBattlePane(char *battle_pane);
void addInfoText(char *top_text, int top_text_length, char *bot_text, int bot_text_length, char *battle_pane);
void createHealthBar(char *health_bar, Pokemon pkmn);
void addPokemonName(Pokemon pkmn, char *battle_pane, int pos);
void addPokemonHealthBar(Pokemon pkmn, char *battle_pane, int pos);
void addPokemonHpPoint(Pokemon pkmn, char *battle_pane, int pos);
void eraseArrayLine(int pos, char *battle_pane, int space_count);
void refreshBattlePane(Pokemon player, Pokemon enemy, char *battle_pane);
void compareAndPlaceSkills(int *cursor_first_skills_line, int *cursor_second_skills_line, int i, Player *player, char *battle_pane);
void placeTwoSkills(Player *player, char *battle_pane, int i, int *cursor_first_skills_line, int *cursor_second_skills_line, int arrows_xys[]);
void changeMenuToSkillMenu(Player *player, char *battle_pane, int arrows_xys[]);
void setMenuBack(char *battle_pane, int *arrows_xys, MenuArrow arrow);
int manageSkillMenuKeyPressed(int *targetted_skill, int key_pressed, int arrows_xys[], char *battle_pane, Player *player, Pokemon *enemy, int state);
int manageMenuChoice(MenuArrow *arrow, char *battle_pane, Player *player, Pokemon *enemy);
int manageBattleMenuKeyPressed(char key_pressed, MenuArrow *arrow, char *battle_pane, Player *player, Pokemon *enemy);
void battle(Player *player);
void printAndManageBattlePane(char *battle_pane, Player *player, Pokemon *enemy);
void isBattle(Player *player);
void addAilments(Pokemon pkmn, char *battle_pane, int pos, bool left_erase);

#endif