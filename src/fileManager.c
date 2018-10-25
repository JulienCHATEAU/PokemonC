#include <stdio.h>
#include <stdlib.h>

/* Open a file managing errors
* file_path : the path of the file to openFile
* mode : the way to open the file ('r' to read, 'w' to write)
* return a file stream or exit the program with the error code 1 if an error occured
*/
FILE *openFile(char *file_path, char *mode) {
  FILE *file = fopen(file_path, mode);
  if (file == NULL) {
    fprintf(stderr, "Erreur lors de l'ouverture du fichier : %s\n", file_path);
    exit(1);
  }
  return file;
}


/* Close a file managing errorq
* file : the file stream
* return 0 if everything went right, exit the program with the error code 2 if ann error occured
*/
int closeFile(FILE *file) {
  if (fclose(file) == EOF) {
    fprintf(stderr, "Erreur de fermeture d'un fichier\n");
    exit(2);
  }
  return 0;
}