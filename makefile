main:
	@ echo "Création du dossier binaire"
	@ mkdir -p bin
	@ echo "Création du dossier de sauvegarde"
	@ mkdir -p save
	@ echo "Création du fichier de sauvegarde des comptes"
	@ touch save/accounts.txt
	@ echo "Compilation des fichiers sources..."
	@ gcc -O0 -g -o bin/main src/*.c -lm -lpthread -lncurses
	@ echo "\n"
	@ echo "C'est prêt !"
	@ echo "\n"
	@ echo "Pour lancer le jeu, tapez la commande : 'make play'"

play:
	@ echo "Lancement du jeu..."
	@ sleep 1
	@ ./bin/main 1

rowMode:
	@ echo "Row mode to 0"
	./bin/main 0

clean:
	@ echo "Suppression du dossier binaire"
	@ rm -rf bin
