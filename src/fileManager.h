#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#define POKEMONS_PATH "data/pokemons.txt"
#define SKILLS_PATH "data/skills.txt"

#include <stdio.h>

FILE *openFile(char *file_path, char *mode);
int closeFile(FILE *file);
void skipLines(FILE *save_file, int line_count);

#endif