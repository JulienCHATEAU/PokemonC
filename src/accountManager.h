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
int isPseudoValid(char *pseudo, int max_length);
int isPasswordValid(char *password, int max_length);
void askPseudoOrPw(char *array, int array_max_length);
void askPseudo(char *array);
void askPassword(char *array);
int addAccount(char *pseudo, char *password);
int manageConnexionKeyPressed(char *pseudo, char *password, char key_pressed);
int pseudoPasswordExists(char *pseudo, char *password, Check mode);

#endif