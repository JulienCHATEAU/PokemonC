#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "pokemon.h"
#include "util.h"
#include "map.h"
#include "battlePane.h"
#include "print.h"
#include "fileManager.h"


/* Prints the information of the targetted skill (to replace by the new_skill)
   and then the one of the new skill
* targetted_skill : the targetted skill
* new_skill : the new skill
*/
void printLearningSkillInfo(Skill targetted_skill, Skill new_skill) {
  //tty_reset();
  printw("\n\n\n\n");
  printSkill(targetted_skill);
  printw("\n\n");
  printSkill(new_skill);
  //setRawMode('1');
}

/* Print a skill
* skill : the skill
*/
void printSkill(Skill skill) {
  printw("    %s : \n", skill.name);
  char type_string[25];
  typeEnumToString(skill.type, type_string);
  printw("      Description :   de type %s. %s\n", type_string, skill.description);
  printw("      Puissance :     %d\n", skill.power);
  printw("      Précision :     %d / 100\n", skill.accuracy);
  printw("      PP :            %d / %d\n", skill.pp, skill.pp_max);
}

/* Print stats
* stats : the stats
*/
void printStats(Stats stats) {
  printw("    Pdv :        %d / %d\n", stats.hp, stats.hp_max);
  printw("    Attaque :    %d / %d\n", stats.att, stats.att_max);
  printw("    Défense :    %d / %d\n", stats.def, stats.def_max);
  printw("    Vitesse :    %d / %d\n", stats.spd, stats.spd_max);
}

/* Print a pokemon
* pkmn : the pokemon
*/
void printPokemon(Pokemon pkmn) {
  printw("\n\n    %s : \n\n", pkmn.name);
  printw("    Niveau :                  %d\n", pkmn.lvl);
  printw("    Expérience :              %d / %d\n", pkmn.xp, getCurrentExperienceStage(pkmn));
  char type_string[25];
  getTypesString(pkmn, type_string);
  printw("    Types :                   %s\n", type_string);
  char *ailement;
  getAilmentLabel(pkmn, &ailement);
  if (strcmp(ailement, " ") == 0) {
    ailement = "Aucun";
  }
  printw("    Problème de status :      %s\n\n", ailement);
  printStats(pkmn.stats);
  printw("\n\n");
  for (int i = 0; i < 4; i++) {
    printSkill(pkmn.skills[i]);
    printw("\n\n");
  }
}

/* Stores the dialog box in an array
* dialog_box : the array to store the dialog box
*/
void loadDialogBox(char *dialog_box) {
  FILE *dialog_box_file = openFile(DIALOG_BOX_PATH, "r");
  fread(dialog_box, sizeof(char), DIALOG_BOX_LENGTH, dialog_box_file);
  closeFile(dialog_box_file);
}

/* Prints the dialog box
* dialog_box : the array describing the dialog box
*/
void printDialogBox(char *dialog_box) {
  // dialog_box[DIALOG_BOX_LENGTH] = '\0';
  printw("\n\n");
  for (int j = 0; j < DIALOG_BOX_LENGTH; j++) {
    if (dialog_box[j] != 13) {
      addch(dialog_box[j]);
    }
  }
}

/* Erases a line in the dialog box
* xy_line_start : the xy coordinate where to start the erase
* dialog_box : the dialog box
*/
void eraseDialogBoxLine(int xy_line_start, char *dialog_box) {
  eraseArrayLine(xy_line_start+TEXT_MAX_LENGTH-1, dialog_box, TEXT_MAX_LENGTH);
}

/* Erases all the dialog box lines
* dialog_box : the dialog box
*/
void eraseDialogBoxLines(char *dialog_box) {
  eraseDialogBoxLine(FRST_LINE_START, dialog_box);
  eraseDialogBoxLine(SCD_LINE_START, dialog_box);
  eraseDialogBoxLine(THRD_LINE_START, dialog_box);
}

/* Adds a text in a dialog box
* xy_start : the xy coordinate where to start writting the text
* text : the text
* text_length : the text length
* dialog_box : the dialog box
*/
void addTextInDialogBox(int xy_start, char *text, int text_length, char *dialog_box) {
  if (text_length <= TEXT_MAX_LENGTH) {
    addStringToArray(text, text_length, xy_start, dialog_box);
  }
}