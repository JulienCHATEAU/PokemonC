#include "pnj.h"
#include "fileManager.h"
#include "map.h"
#include "print.h"
#include "util.h"

void managePnjOrientation(char *printable_map, char player_orientation,
                          int xy_ifo_player) {
  switch (player_orientation) {
  case PLAYER_N:
    printable_map[xy_ifo_player] = PLAYER_S;
    break;

  case PLAYER_S:
    printable_map[xy_ifo_player] = PLAYER_N;
    break;

  case PLAYER_E:
    printable_map[xy_ifo_player] = PLAYER_W;
    break;

  case PLAYER_W:
    printable_map[xy_ifo_player] = PLAYER_E;
    break;

  default:
    break;
  }
}

void pnjDialog(char *printable_map, char *dialog_box, Pnj *pnj) {
  int txt_length = 2;
  for (int i = 0; i < pnj->texts_count; i += 2) {
    txt_length = pnj->name_length + 3 + pnj->texts_length[i];
    char *first_txt = malloc(sizeof(char) * txt_length + 1);
    sprintf(first_txt, "%s : %s", pnj->name, pnj->dialog_texts[i]);
    addTextInDialogBox(FRST_LINE_START, first_txt, txt_length, dialog_box);
    txt_length = pnj->name_length + 3 + pnj->texts_length[i + 1];
    char *scd_txt = malloc(sizeof(char) * txt_length + 1);
    char *spaces = malloc(sizeof(char) * pnj->name_length + 1);
    getSpacesString(spaces, pnj->name_length);
    sprintf(scd_txt, "%s   %s", spaces, pnj->dialog_texts[i + 1]);
    addTextInDialogBox(SCD_LINE_START, scd_txt, txt_length, dialog_box);

    clearAndPrintMap(printable_map, dialog_box);
    if (i + 2 < pnj->texts_count) {
      enterKey();
      eraseDialogBoxLines(dialog_box);
    }
  }
  clearAndPrintMap(printable_map, dialog_box);
}

bool findPnj(int x_map, int y_map, Pnj *pnj) {
  bool trouve = false;
  FILE *pnjs_file = openFile(PNJS_PATH, "r");
  int fscanf_ret = 0;
  while (!trouve && fscanf_ret != EOF) {
    fscanf_ret = fscanf(pnjs_file, "%d;%d %d|", &pnj->square.x_map,
                        &pnj->square.y_map, &pnj->square.xy);
    if (pnj->square.x_map == x_map && pnj->square.y_map == y_map) {
      trouve = true;
      fscanf(pnjs_file, "%d %d|", &pnj->is_trainer, &pnj->look_range);
      fscanf(pnjs_file, "%d ", &pnj->name_length);
      pnj->name = malloc(sizeof(char) * pnj->name_length + 1);
      fscanf(pnjs_file, "%[^|]|", pnj->name);
      fscanf(pnjs_file, "%d|", &pnj->texts_count);
      pnj->dialog_texts = malloc(sizeof(char *) * pnj->texts_count);
      pnj->texts_length = malloc(sizeof(int) * pnj->texts_count);
      for (int i = 0; i < pnj->texts_count; ++i) {
        fscanf(pnjs_file, "%d ", &pnj->texts_length[i]);
        pnj->dialog_texts[i] = malloc(sizeof(char) * pnj->texts_length[i] + 1);
        fscanf(pnjs_file, "%[^|]|", pnj->dialog_texts[i]);
      }
    }
    fscanf(pnjs_file, "%*[^\n]\n");
  }
  // printPnj(pnj);
  closeFile(pnjs_file);
  printf("trouve = %d", trouve);
  printPnj(pnj);
  return trouve;
}

void printPnj(Pnj *pnj) {
  printf("%d;%d %d\n", pnj->square.x_map, pnj->square.y_map, pnj->square.xy);
  printf("%d %d\n", pnj->is_trainer, pnj->look_range);
  printf("%d %s\n", pnj->name_length, pnj->name);
  printf("%d\n", pnj->texts_count);
  for (int i = 0; i < pnj->texts_count; ++i) {
    printf("%d %s", pnj->texts_length[i], pnj->dialog_texts[i]);
    printf("\n");
  }
}