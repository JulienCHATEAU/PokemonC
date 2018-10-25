#ifndef BATTLE_PANE_H
#define BATTLE_PANE_H

#define BATTLE_PANE_PATH "data/battlePane.txt"
#define TYPE_COUNT 18

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

#define MENU_ARROW_LGTH 2
#define HLTH_BAR_LGTH 24 //counting '{' and '}'
#define TEXT_LINE_MAX_LGTH 34
#define BATTLE_LINE_LGTH 67
#define BATTLE_PANE_LGTH 1240

#define BATTLE_PERCENTAGE 15

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
  Boolean player_turn;
} PlayTurnParameters;

void createThreadNW8(void *function, void *function_params);
void eraseInfoText();
int nDigits(unsigned int n);
Boolean hasAnyAlivePokemon(Player *player);
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
int manageSkillMenuKeyPressed(int *targetted_skill, int key_pressed, int arrows_xys[], char *battle_pane, Player *player, Pokemon *enemy);
int manageMenuChoice(MenuArrow *arrow, char *battle_pane, Player *player, Pokemon *enemy);
int manageBattleMenuKeyPressed(char key_pressed, MenuArrow *arrow, char *battle_pane, Player *player, Pokemon *enemy);
void battle(Player *player);
void printAndManageBattlePane(char *battle_pane, Player *player, Pokemon *enemy);
void isBattle(Player *player);
void addAilments(Pokemon pkmn, char *battle_pane, int pos, Boolean left_erase);

#endif