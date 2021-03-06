Auteur : Julien CHATEAU

_Ce document est à lire attentivement et vous permettra de comprendre le fonctionnement de ce jeu._

# __Pokemon C__

## Prérequis

* Ce jeu est uniquement jouable sur Linux (ou avec le [Windows Subsystem for Linux](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux) de Windows mais pas garanti)
* Vous devez être en possession :
  * du compilateur C : gcc (`$ sudo apt-get install gcc`)
  * du logiciel make (`$ sudo apt-get install make`)
  * du logiciel git (`$ sudo apt-get install git`)
  * de la librairie ncurses (`$ sudo apt-get install libncurses5-dev libncursesw5-dev`)

## L'initialisation et le lancement du jeu

Pour installer la version du jeu de votre choix, placez vous dans un répertoire et effectuez la suite de commande suivante :

1. `$ git clone https://github.com/JulienCHATEAU/PokemonC.git`
2. `$ cd PokemonC`
3. `$ make`

Pour lancer le jeu effectuez la commande :

- `$ make play`

## Contrôles

- Z : Mouvement vers le haut
- Q : Mouvement vers la gauche
- S : Mouvement vers la droite
- D : Mouvement vers le bas
- Espace : Menu start
- Enter : Valider / Intéragir
- Retour : Annuler un action

*PS : Si vous remarquez quelque chose semblant être erroné n'hésitez pas à me contacter via steamygamecontact@gmail.com ou à ouvrir une issue, je me ferai un plaisir de vous répondre*
