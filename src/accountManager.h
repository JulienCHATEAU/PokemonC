#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#define PSEUDO_MAX_LENGTH 50
#define PASSWORD_MAX_LENGTH 40

typedef enum {
  PSEUDO,
  PASSWORD,
  BOTH
} Check;

void manageConnexionMenu(char *pseudo);
void skipLines(FILE *save_file, int line_count);
void skipLines(FILE *save_file, int line_count);
int isPseudoValid(char *pseudo, int max_length);
int isPasswordValid(char *password, int max_length);
void askPseudoOrPw(char *array, int array_max_length);
void askPseudo(char *array);
void askPassword(char *array);
int addAccount(char *pseudo, char *password);
int manageConnexionKeyPressed(char *pseudo, char *password, char key_pressed);
int pseudoPasswordExists(char *pseudo, char *password, Check mode);
FILE *openPlayerSaveFile(Player *player, char *mode);
FILE *openPlayerNewSaveFile(Player *player, char *mode);
void skipPlayerData(FILE *save_file, Player *player);
void readOnePokemonLine(FILE *save_file, Pokemon *pkmn);
void loadPlayerData(int *x_map, int *y_map, Player *player);
void saveMapDataFromFile(Player *player);
void writeOnePokemonLine(FILE *save_file, Pokemon pkmn);
void savePlayerData(int x_map, int y_map, Player *player);

#endif