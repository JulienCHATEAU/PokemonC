#include "battlePane.h"
#include <stdbool.h>

#ifndef BATTLE_H
#define BATTLE_H

#define SAME_SPEED_RAND_PERCENTAGE 50
#define CRIT_PERCENTAGE 7
#define WAIT_BETWEEN_ANIM 1300

#define TYPES_TABLE_PATH "data/types_table.txt"
#define TYPES_TABLE_LGTH TYPE_COUNT *TYPE_COUNT

#define MISS_STRING " rate son attaque !"
#define MISS_STRING_LENGTH 19
#define CRIT_STRING "Coup Critique !"
#define CRIT_STRING_LENGTH 15
#define NO_EFFECT_STRING "Cela n'a aucun effet"
#define NO_EFFECT_STRING_LENGTH 20
#define SUPER_EFFECTIVE_STRING "C'est super efficace !"
#define SUPER_EFFECTIVE_STRING_LENGTH 22
#define NOT_EFFECTIVE_STRING "Ce n'est pas tres efficace..."
#define NOT_EFFECTIVE_STRING_LENGTH 29
#define WIN_STRING "Vous avez gagne !"
#define WIN_STRING_LENGTH 17
#define LOST_STRING "Vous avez perdu ..."
#define LOST_STRING_LENGTH 19

#define FRST_PAR_TEXT "est paralyse,"
#define FRST_PAR_TEXT_LENGTH 12
#define SCD_PAR_TEXT "il ne peut pas attaquer !"
#define SCD_PAR_TEXT_LENGTH 25

#define SLEEP_TEXT "est endormi"
#define SLEEP_TEXT_LENGTH 11
#define WAKE_UP_TEXT "se reveille !"
#define WAKE_UP_TEXT_LENGTH 13

#define FREEZE_TEXT "est gele"
#define FREEZE_TEXT_LENGTH 8
#define UNFREEZE_TEXT "n'est plus gele !"
#define UNFREEZE_TEXT_LENGTH 17

#define BURN_TEXT "souffre de sa brulure"
#define BURN_TEXT_LENGTH 21

#define POISONNING_TEXT "souffre du poison"
#define POISONNING_TEXT_LENGTH 17

#define CONFUSION1_TEXT "n'est plus confus"
#define CONFUSION1_TEXT_LENGTH 17
#define CONFUSION2_TEXT "est confus"
#define CONFUSION2_TEXT_LENGTH 10
#define CONFUSION3_TEXT "se blesse dans"
#define CONFUSION3_TEXT_LENGTH 14
#define CONFUSION4_TEXT "sa confusion"
#define CONFUSION4_TEXT_LENGTH 12

void swapPokemonPointerContent(Pokemon *p1, Pokemon *p2);
void swapIntPointerContent(int *p1, int *p2);
bool chosePlayOrder(Pokemon *frst_to_play, Pokemon *scd_to_play,
                    int *frst_player_skill, int *scd_player_skill);
int hpCountLost(Pokemon *skill_user, Pokemon *target, Skill skill, bool crit,
                double weakness);
void manageFirstTextAnimation(Pokemon *skill_user, int chosen_skill,
                              char *battle_pane);
bool manageSecondTextAnimation(char *battle_pane, bool crit, double weakness);
bool useSkill(Pokemon *skill_user, Pokemon *target, int chosen_skill,
              char *battle_pane, bool swapped_xor_frst);
bool isAnyKoPokemon(Pokemon *p1, Pokemon *p2);
bool playOnePokemonTurn(Pokemon *skill_user, Pokemon *target, int chosen_skill,
                        bool swapped, char *battle_pane);
// void *playTurn(void *p);
void *playTurn(int *targetted_skill, Player *player, Pokemon *enemy, char *battle_pane, int *key_pressed_status, bool player_turn);
void ailmentTextAnimation(Pokemon *skill_user, Pokemon *target,
                          bool swapped_xor_frst, char *battle_pane,
                          char *frst_ailment_text, int frst_ailment_text_length,
                          char *scd_ailment_text, int scd_ailment_text_length);
bool manageConfusion(Pokemon *skill_user, Pokemon *target,
                     bool swapped_xor_frst, char *battle_pane, int i);
bool manageFirstAilment(Pokemon *skill_user, Pokemon *target,
                        bool swapped_xor_frst, char *battle_pane);
bool manageAllAilments(Pokemon *skill_user, Pokemon *target,
                       bool swapped_xor_frst, char *battle_pane);
int manageYesNoKeyPressed(char key_pressed, int *yes);

#endif