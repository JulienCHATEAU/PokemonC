#include "pokemon.h"
#include "util.h"
#include "map.h"

#ifndef PRINT_H
#define PRINT_H

#define DIALOG_BOX_PATH "data/dialogBox.txt"
#define DIALOG_BOX_LENGTH 376
#define DIALOG_BOX_LINE_COUNT 6
#define DIALOG_BOX_LINE 63

#define HEAL_TEXT "Vos Pokemons sont en pleine forme"
#define HEAL_TEXT_LGTH 33

#define TEXT_MAX_LENGTH 55
#define FRST_LINE_START 129
#define SCD_LINE_START 192
#define THRD_LINE_START 255

void printLearningSkillInfo(Skill targetted_skill, Skill new_skill);
void printSkill(Skill skill);
void printStats(Stats stats);
void printPokemon(Pokemon pkmn);
void loadDialogBox(char *dialog_box);
void printDialogBox(char *dialog_box);
void eraseDialogBoxLine(int xy_line_start, char *dialog_box);
void eraseDialogBoxLines(char *dialog_box);
void addTextInDialogBox(int xy_start, char *text, int text_length, char *dialogBox);

#endif