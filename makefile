main:
	@ echo "Création du dossier binaire"
	@ mkdir bin
	@ echo "Compilation des fichiers sources..."
	@ gcc -O0 -g -o bin/main src/*.c -lm -lpthread
	@ echo "\n"
	@ echo "C'est prêt !"

clean:
	@ echo "Suppression du dossier binaire"
	@ rm -rf bin