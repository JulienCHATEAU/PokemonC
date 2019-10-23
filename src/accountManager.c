#include "accountManager.h"
#include "bag.h"
#include "fileManager.h"
#include "map.h"
#include "pokemon.h"
#include "print.h"
#include "ttyraw.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACCOUNTS_FILE_PATH "save/accounts.txt"

/* Manages the connexion menu (ask the user his pseudo, his password checking if
 * correct ect...) pseudo : an array to store the pseudo given by the user
 */
void manageConnexionMenu(char *pseudo) {
  char password[PASSWORD_MAX_LENGTH];
  int key_status = 1;
  char key_pressed = -1;
  do {
    clearConsole();
    printf("\n\n");
    askPseudo(pseudo);
    askPassword(password);
    printf("\n\n     --------------       --------------------\n");
    printf("    | 1. Connexion |     | 2. Creer le compte |\n");
    printf("     --------------       --------------------\n");
    setRawMode('1');
    do {
      key_pressed = getchar();
    } while (key_pressed == -1);
    key_status = manageConnexionKeyPressed(pseudo, password, key_pressed);
  } while (key_status != 0);
}

/* Checks if the given pseudo is valid
 * pseudo : the pseudo to check
 * max_length : the maximum length the pseudo can be
 */
int isPseudoValid(char *pseudo, int max_length) {
  int i = 0;
  int valid = 1;
  while (valid && i < max_length) {
    // only a-Z characters and '-' are valid in a pseudo
    if ((pseudo[i] < 48 || pseudo[i] > 57) &&
        (pseudo[i] < 65 || pseudo[i] > 90) &&
        (pseudo[i] < 97 || pseudo[i] > 122) && pseudo[i] != '-' &&
        pseudo[i] != 0) {
      valid = 0;
    }
    i++;
  }
  return valid;
}

/* Checks if the given password is valid
 * pseudo : the password to check
 * max_length : the maximum length the password can be
 */
int isPasswordValid(char *password, int max_length) {
  return isPseudoValid(password, max_length);
}

/* Asks the user to provide a pseudo or a password
 * array : an array to store the pseudo or the password
 * array_max_length : the pseudo or the password is asked depending on this
 * value
 */
void askPseudoOrPw(char *array, int array_max_length) {
  memset(array, 0, array_max_length);
  int first_time = 1;
  int valid_state = 1;
  do {
    if (first_time) {
      first_time = 0;
    } else if (valid_state == 0) {
      printf("  Seulement les caractères minuscules, majuscules et '-' sont "
             "acceptés\n");
    }
    if (array_max_length == PSEUDO_MAX_LENGTH) {
      printf("\n  Pseudo (%d caractères max) : ", array_max_length);
    } else {
      printf("\n  Mot de passe (%d caractères max) : ", array_max_length);
    }
    fgets(array, array_max_length, stdin);
    int array_length = strlen(array);
    array[(array_length - 1)] = '\0';
    valid_state = isPseudoValid(array, array_max_length);
  } while (valid_state == 0);
}

/* Asks the user provide a pseudo
 * array : an array to store the pseudo
 */
void askPseudo(char *array) { askPseudoOrPw(array, PSEUDO_MAX_LENGTH); }

/* Asks the user provide a password
 * array : an array to store the password
 */
void askPassword(char *array) { askPseudoOrPw(array, PASSWORD_MAX_LENGTH); }

/* Manages the key pressed by the user when he has to chose between creating an
 * account or connection to his one pseudo : the user's pseudo password : the
 * user's password key_pressed : the key pressed by the user
 */
int manageConnexionKeyPressed(char *pseudo, char *password, char key_pressed) {
  int key_pressed_status = 1;
  tty_reset();
  if (key_pressed == '1') {
    if (pseudoPasswordExists(pseudo, password, BOTH)) {
      key_pressed_status = 0;
    } else {
      printf("\n  Pseudo ou mot de passe incorrect\n\n\n  ");
      waitForEnter();
    }
  } else if (key_pressed == '2') {
    int exist = addAccount(pseudo, password);
    if (exist == 0) {
      printf("\n  Création du compte réussi !\n");
      char *random_name;
      getRandomPokemonName(&random_name);
      Player player = createPlayer(pseudo, PLAYER_START_POS, '^', random_name);
      savePlayerData(0, 0, &player);
      printf("\n\n\n ");
      key_pressed_status = 0;
      waitForEnter();
    } else if (exist == 1) {
      printf("\n  Ce pseudo existe déjà.\n\n\n  ");
      waitForEnter();
    } else if (exist == 2) {
      printf("\n  Ce mot de passe existe déjà.\n\n\n  ");
      waitForEnter();
    }
  } else if (key_pressed == EXIT) {
    exit(0);
  } else {
    key_pressed_status = 2;
  }
  return key_pressed_status;
}

/* Adds an account in the accounts.txt file -> the pattern is for each account
 * '$pseudo $password' Each account is described on one line in this file pseudo
 * : the pseudo of the account password : the password of the account return 1
 * if pseudo exists, 2 if password exist, 0 otherwise
 */
int addAccount(char *pseudo, char *password) {
  int exist = 0;
  if (!pseudoPasswordExists(pseudo, password, PSEUDO)) {
    if (!pseudoPasswordExists(pseudo, password, PASSWORD)) {
      FILE *accounts = openFile(ACCOUNTS_FILE_PATH, "a");
      fprintf(accounts, "%s %s\n", pseudo, password);
      char new_file_path[100];
      sprintf(new_file_path, "save/%s.txt", pseudo);
      openFile(new_file_path, "w+");
      closeFile(accounts);
    } else {
      exist = 2;
    }
  } else {
    exist = 1;
  }
  return exist;
}

/* Checks if the pseudo, the password or both exist dependong on the chosen mode
 * pseudo : the pseudo
 * password : the password
 * mode : the checking mode : PSEUDO/PASSWORD to check if the pseudo/password
 * exists, BOTH to check if a couple (pseudo; password) exists return : 1 if
 * what you wanted to check is valid, 0 otherwise
 */
int pseudoPasswordExists(char *pseudo, char *password, Check mode) {
  int pseudo_exists = 0;
  int password_exists = 0;
  int stop = 0;
  FILE *accounts = openFile(ACCOUNTS_FILE_PATH, "r");
  char crt_pseudo[PSEUDO_MAX_LENGTH];
  char crt_password[PASSWORD_MAX_LENGTH];
  while (fscanf(accounts, "%s %s\n", crt_pseudo, crt_password) != EOF &&
         !stop) {
    if (strcmp(crt_pseudo, pseudo) == 0) {
      pseudo_exists = 1;
    }
    if (strcmp(crt_password, password) == 0) {
      password_exists = 1;
    }
    switch (mode) {
    case PSEUDO:
      if (pseudo_exists) {
        stop = 1;
      }
      break;

    case PASSWORD:
      if (password_exists) {
        stop = 1;
      }
      break;

    case BOTH:
      if (pseudo_exists && password_exists) {
        stop = 1;
      }
      break;
    }
  }
  closeFile(accounts);
  return stop;
}

/* Opens the file containing all the given player data
 * player : the player_pos
 * mode : the oppening mode
 */
FILE *openPlayerSaveFile(Player *player, char *mode) {
  char *save_file_path = malloc(sizeof(char) * (9 + player->pseudo_length +
                                                1)); // 9 -> 'save/.txt' length
  sprintf(save_file_path, "save/%s.txt", player->pseudo);
  FILE *save_file = openFile(save_file_path, mode);
  free(save_file_path);
  return save_file;
}

/* In the file containing the player data, skip the first lines describing the
 * player save_file : the concerned file
 */
void skipPlayerData(FILE *save_file) {
  int pkmn_count = 0;
  int bag_item_count = 0;
  fscanf(save_file, "%*d|%*d|%*d|");
  fscanf(save_file, "%*c|%*c|%d|%d|%*d|%*d\n", &bag_item_count, &pkmn_count);
  skipLines(save_file, pkmn_count + bag_item_count);
}

/* In the file containing the player data, read one pokemon line storing the
 * data in the given Pokemon structure save_file : the concerned file pokemon :
 * the pokemon structure where to store the data
 */
void readOnePokemonLine(FILE *save_file, Pokemon *pkmn) {
  fscanf(save_file, "%d ", &pkmn->name_length);
  pkmn->name = malloc(sizeof(char) * pkmn->name_length + 1);
  fscanf(save_file, "%[^|]|", pkmn->name);
  int stats[4];
  int type = 0;
  fscanf(save_file, "%d ", &pkmn->lvl);
  fscanf(save_file, "%d ", &pkmn->xp);
  fscanf(save_file, "%d", &type);
  pkmn->type[0] = (Type)type;
  fscanf(save_file, "%d", &type);
  pkmn->type[1] = (Type)type;
  int id;
  for (int i = 0; i < 4; i++) {
    fscanf(save_file, "|%d", &id);
    pkmn->skills[i] = fillSkill(id);
  }
  fscanf(save_file, "|%d", &pkmn->base_xp);
  fscanf(save_file, "|%d", &pkmn->crt_ailments[0]);
  pkmn->crt_ailments[1] = NO_AILMENT;
  fscanf(save_file, "|%d %d %d %d", &stats[0], &stats[1], &stats[2], &stats[3]);
  pkmn->stats = createStats(stats[0], stats[1], stats[2], stats[3]);
  fscanf(save_file, "%d %d %d %d", &pkmn->stats.hp, &pkmn->stats.att,
         &pkmn->stats.def, &pkmn->stats.spd);
  fscanf(save_file, "|%d ", &pkmn->evo_lvl);
  if (pkmn->evo_lvl != -1) {
    int fscanf_ret = fscanf(save_file, "%d ", &pkmn->evo_name_length);
    pkmn->evo_name = malloc(sizeof(char) * pkmn->evo_name_length + 1);
    fscanf(save_file, "%[^\n]\n", pkmn->evo_name);
  } else {
    pkmn->evo_name_length = 0;
    pkmn->evo_name = "";
    fscanf(save_file, "%*[^\n]\n");
  }
}

/* In the file containing the player data, read one bag item line and add it to
 * the player's bag save_file : the concerned file player : the player
 */
void readOneBagItemLine(FILE *save_file, Player *player) {
  int item_id = 0;
  int item_count = 0;
  fscanf(save_file, "%d|%d\n", &item_id, &item_count);
  addBagItemPlayer(player, item_id, item_count);
}

/* In the file containing the player data, read one bag item line and add it
 * to the player's bag save_file : the concerned file player : the player
 */
void readOnePickedItemLine(FILE *save_file, Player *player) {
  MapSquare square;
  fscanf(save_file, "%d;%d %d\n", &square.x_map, &square.y_map, &square.xy);
  addPickedItemPlayer(player, square);
}

/* Loads the player data from his save file
 * x_map : the x coordinate of the map where the user exited the game
 * y_map : the y coordinate of the map where the user exited the game
 * player : the player structure to fill
 */
void loadPlayerData(int *x_map, int *y_map, Player *player) {
  initBag(player);
  int bag_item_count = 0;
  int item_picked_count = 0;
  player->pseudo[player->pseudo_length] = '\0';
  player->bag_item_count = 0;
  player->item_picked_count = 0;
  FILE *save_file = openPlayerSaveFile(player, "r");
  fscanf(save_file, "%d|%d|%d|", x_map, y_map, &player->xy);
  fscanf(save_file, "%c|%c|%d|%d|%d|%d\n", &player->pos, &player->char_at_pos,
         &bag_item_count, &player->pkmn_count, &player->money,
         &item_picked_count);
  for (int i = 0; i < player->pkmn_count; i++) {
    readOnePokemonLine(save_file, &player->pkmns[i]);
  }
  for (int i = 0; i < bag_item_count; i++) {
    readOneBagItemLine(save_file, player);
  }
  for (int i = 0; i < item_picked_count; i++) {
    readOnePickedItemLine(save_file, player);
  }
  closeFile(save_file);
}

/* In the file containing the player data, write one pokemon line with the data
 * stored the given Pokemon structure save_file : the concerned file pokemon :
 * the pokemon structure where to store the data
 */
void writeOnePokemonLine(FILE *save_file, Pokemon pkmn) {
  fprintf(save_file, "%d %s|", pkmn.name_length, pkmn.name);
  fprintf(save_file, "%d %d %d %d|", pkmn.lvl, pkmn.xp, (int)pkmn.type[0],
          (int)pkmn.type[1]);
  fprintf(save_file, "%d|%d|%d|%d|", pkmn.skills[0].id, pkmn.skills[1].id,
          pkmn.skills[2].id, pkmn.skills[3].id);
  fprintf(save_file, "%d|", pkmn.base_xp);
  fprintf(save_file, "%d|", (int)pkmn.crt_ailments[0]);
  fprintf(save_file, "%d %d %d %d ", pkmn.stats.hp_max, pkmn.stats.att_max,
          pkmn.stats.def_max, pkmn.stats.spd_max);
  fprintf(save_file, "%d %d %d %d|", pkmn.stats.hp, pkmn.stats.att,
          pkmn.stats.def, pkmn.stats.spd);
  fprintf(save_file, "%d %d %s\n", pkmn.evo_lvl, pkmn.evo_name_length,
          pkmn.evo_name);
}

/* In the file containing the player data, write one bag item line
 * save_file : the concerned file
 * bag_item : the bag item
 */
void writeOneBagItemLine(FILE *save_file, BagItem bag_item) {
  fprintf(save_file, "%d|%d\n", bag_item.id, bag_item.count);
}

/* In the file containing the player data, write one picked item line
 * save_file : the concerned file
 * picked_item : the picked item
 */
void writeOnePickedItemLine(FILE *save_file, MapSquare picked_item) {
  fprintf(save_file, "%d;%d %d\n", picked_item.x_map, picked_item.y_map,
          picked_item.xy);
}

/* save the player data in his save file
 * x_map : the x coordinate of the map where the user currently is
 * y_map : the y coordinate of the map where the user currently is
 * player : the player
 */
void savePlayerData(int x_map, int y_map, Player *player) {
  FILE *save_file = openPlayerSaveFile(player, "w+");
  fprintf(save_file, "%d|%d|%d|", x_map, y_map, player->xy);
  fprintf(save_file, "%c|%c|%d|%d|%d|%d\n", player->pos, player->char_at_pos,
          player->bag_item_count, player->pkmn_count, player->money,
          player->item_picked_count);
  for (int i = 0; i < player->pkmn_count; i++) {
    writeOnePokemonLine(save_file, player->pkmns[i]);
  }
  for (int i = 0; i < player->bag_item_count; i++) {
    writeOneBagItemLine(save_file, player->bag[i]);
  }
  for (int i = 0; i < player->item_picked_count; i++) {
    writeOnePickedItemLine(save_file, player->item_picked[i]);
  }
  closeFile(save_file);
}
