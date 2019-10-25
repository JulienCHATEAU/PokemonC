#include "pnj.h"
#include "battlePane.h"
#include "fileManager.h"
#include "map.h"
#include "print.h"
#include "util.h"

void managePnjOrientation(Map *map, char player_orientation,
                          int xy_ifo_player) {
  switch (player_orientation) {
  case PLAYER_N:
    map->pnj.orientation = PLAYER_S;
    break;

  case PLAYER_S:
    map->pnj.orientation = PLAYER_N;
    break;

  case PLAYER_E:
    map->pnj.orientation = PLAYER_W;
    break;

  case PLAYER_W:
    map->pnj.orientation = PLAYER_E;
    break;

  default:
    break;
  }
  map->printable_map[xy_ifo_player] = map->pnj.orientation;
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
    enterKey();
    eraseDialogBoxLines(dialog_box);
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
      fscanf(pnjs_file, "%c|", &pnj->orientation);
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
  closeFile(pnjs_file);
  return trouve;
}

bool pnjBattle(Map *map, Player *player) {
  addTextInDialogBox(FRST_LINE_START, "!!!", 3, map->dialog_box);
  clearW8NErase(map->printable_map, map->dialog_box);
  int want_battle_length =
      17 + map->pnj.name_length; // 17 = ' veux se battre !' length
  char *want_battle = malloc(sizeof(char) * want_battle_length + 1);
  sprintf(want_battle, "%s veux se battre !", map->pnj.name);
  addTextInDialogBox(FRST_LINE_START, want_battle, want_battle_length,
                     map->dialog_box);
  clearW8NErase(map->printable_map, map->dialog_box);
  bool lost = goForBattle(player, map, TRAINER_BATTLE);
  if (!lost) {
    addPickedItemPlayer(player, map->pnj.square);
    clearAndPrintMap(map->printable_map, map->dialog_box);
    pnjDialog(map->printable_map, map->dialog_box, &map->pnj);
  }
  return lost;
}

bool pnjWantsBattle(Map *map, Player *player) {
  MapSquare item;
  bool trouve = containsPickedItem(player, map->pnj.square, &item);
  return map->x == map->pnj.square.x_map && map->y == map->pnj.square.y_map &&
         map->pnj.is_trainer && !trouve;
}

void handlePnjBattle(Map *map, Player *player) {
  if (pnjWantsBattle(map, player)) {
    bool seen = false;
    if (isHorizontal(map->pnj.square.xy, player->xy)) {
      if (map->pnj.orientation == PLAYER_E) {
        int player_column = getColumn(player->xy);
        if (player_column <=
                getColumn(map->pnj.square.xy) + map->pnj.look_range &&
            player_column > getColumn(map->pnj.square.xy)) {
          seen = true;
          player->pos = '<';
        }
      } else if (map->pnj.orientation == PLAYER_W) {
        int player_column = getColumn(player->xy);
        if (player_column >=
                getColumn(map->pnj.square.xy) - map->pnj.look_range &&
            player_column < getColumn(map->pnj.square.xy)) {
          seen = true;
          player->pos = '>';
        }
      }
    } else if (isVertical(map->pnj.square.xy, player->xy)) {
      if (map->pnj.orientation == PLAYER_N) {
        int player_line = getLine(player->xy);
        if (player_line >= getLine(map->pnj.square.xy) - map->pnj.look_range &&
            player_line < getLine(map->pnj.square.xy)) {
          player->pos = 'v';
          seen = true;
        }
      } else if (map->pnj.orientation == PLAYER_S) {
        int player_line = getLine(player->xy);
        if (player_line <= getLine(map->pnj.square.xy) + map->pnj.look_range &&
            player_line > getLine(map->pnj.square.xy)) {
          seen = true;
          player->pos = '^';
        }
      }
    }
    if (seen) {
      map->printable_map[TO_PRINTABLE_MAP1 * player->xy + TO_PRINTABLE_MAP2] =
          player->pos;
      bool lost = pnjBattle(map, player);
    }
  }
}

void printPnj(Pnj *pnj) {
  printf("%c\n", pnj->orientation);
  printf("%d;%d %d\n", pnj->square.x_map, pnj->square.y_map, pnj->square.xy);
  printf("%d %d\n", pnj->is_trainer, pnj->look_range);
  printf("%d %s\n", pnj->name_length, pnj->name);
  printf("%d\n", pnj->texts_count);
  for (int i = 0; i < pnj->texts_count; ++i) {
    printf("%d %s", pnj->texts_length[i], pnj->dialog_texts[i]);
    printf("\n");
  }
}