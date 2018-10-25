#include <stdio.h>
#include "pokemonPane.h"
#include "fileManager.h"
#include "game.h"

/* Stores the pokemon pane in an array
* pokemon_pane : the pokemon panel
*/
void loadPokemonPane(char *pokemon_pane) {
  FILE *pokemon_pane_file = openFile(POKEMON_PANE_PATH, "r");
  fread(pokemon_pane, sizeof(char), POKEMON_PANE_LENGTH, pokemon_pane_file);
  closeFile(pokemon_pane_file);
}

/* Prints the pokemon panel
* pokemon_pane : the array describing the pokemon panel
*/
void printPokemonPane(char *pokemon_pane) {
  clearConsole();
  printf("\n\n");
  printf("%s\n", pokemon_pane);
}

/* Manages the pokemon pane menu
*/
void managePokemonPaneMenu() {
  char pokemon_pane[POKEMON_PANE_LENGTH];
  loadPokemonPane(pokemon_pane);
  printPokemonPane(pokemon_pane);
  printf("\n\n\n\n                            ");
  waitForEnter();
}