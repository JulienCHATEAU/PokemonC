main:
	@ echo "Création du dossier binaire"
	@ mkdir -p bin
	@ echo "Création du dossier de sauvegarde"
	@ mkdir -p save
	@ echo "Création du fichier de sauvegarde des comptes"
	@ touch save/account.txt
	@ echo "Compilation des fichiers sources..."
	@ gcc -O0 -g -o bin/main src/*.c -lm -lpthread
	@ echo "\n"
	@ echo "C'est prêt !"

play:
	@ echo "Lancement du jeu..."
	@ sleep 1
	@ ./bin/main 1

clean:
	@ echo "Suppression du dossier binaire"
	@ rm -rf bin
