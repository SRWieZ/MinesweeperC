#include <stdio.h>
#include <stdlib.h>
#include "myconio.h"

// permet de simuler un booelen
#define TRUE 1
#define FALSE 0

// Variables de tailles de la fenetre et des elements d'affichages
#define BORDER 2
#define CONSOLE_W 80
#define CONSOLE_H 25
#define MENU_W 24

// Codes ascii des caractères d'affichage
#define A_BORDER 219
#define A_BOMB 66
#define A_MINED 250
#define A_MAP 254
#define A_FLAG 33

// Couleurs d'affichage
#define C_BORDER DARKGRAY
#define C_BOMB LIGHTRED
#define C_MINED LIGHTGRAY
#define C_MAP LIGHTGRAY
#define C_FLAG GREEN
#define C_TEXT WHITE
#define C_ERROR WHITE
#define C_WARNING WHITE
#define C_SUCESS WHITE

// Couleur de fond des caractères
#define B_BORDER BLACK
#define B_BOMB WHITE
#define B_MINED WHITE
#define B_MAP WHITE
#define B_FLAG WHITE
#define B_TEXT BLACK
#define B_ERROR LIGHTRED
#define B_WARNING MAGENTA
#define B_SUCESS GREEN

// Valeurs du tableau
#define GAME_NULL -1
#define GAME_BOMB 9
#define GAME_FLAG 10

// Valeurs maximales
#define MAX_X 50
#define MAX_Y 20

/*

Démineur par DENIZ Eser

Ce jeu contient une version modifier de myconio pour enlever les x - 1 et y - 1 de goto(x,y)
Ici la case 0,0 est en haut à gauche

Les valeurs maximales ne sont là que pour un prochain menu permettant de decider de la taille du jeu
en changeant les valeurs de gameX, gameY, nbBombs et nbCases = gameX*gameY.
Il suffit plus qu'à apeller la fonction showBorders() puis newGame()
Essayez avec les touches 1, 2 et 3
*/

// Variables globales
int game[MAX_X][MAX_Y], nbCases = 100, gameX = 10, gameY = 10; // Carte où seront stocké nos informations
int flags[MAX_X][MAX_Y]; // Carte où seront stocké les drapeaux
int offsetX = BORDER, offsetY = BORDER; // Ecart en x et y du jeu par rapport au bord, calculé ensuite dans showBorder();
int nbBombs = 5; // Nombre de bombes
int nbMined = 0; // Nombre de cases mines
int nbFlags = 0; // Nombre de drapeaux posés
int gameOver = FALSE; // Permet de notifer à la boucle principale la fin du jeu
int needToClear = FALSE;	// Permet de demander à effacer la zone de message

// Headers des fonctions
void generateGame();
void generateBombs();
void showBorders();
void showMenu();
void showGame();
void gotoCase();
void gotoMenu();
void mineCase(int x, int y);
void putFlag(int x, int y);
void addFlag(int x, int y);
void removeFlag(int x, int y);
void newGame();
void endGame();
void msgZone(int color, int background);
void clearMsg();

// Fonction main
main()
{
    char touche;	// Variable où sera stocké la touche
    int cursorX = 0, cursorY = 0; // Position du curseur du joueur
	time_t depart, arrivee; // Variables de temps
	double bestScore = -1, score = -1, lastScore = -1; // scores

	// On choisit le titre
	system("TITLE Minesweeper");

	// On force la taille de la fenetre (on ajoute 1 à CONSOLE_W et CONSOLE_H)
	//system("mode con LINES=26 COLS=81");

	// Activation de rand()
	srand(time(NULL));

	// Génération de la bordure
	showBorders();

	// Affichage du menu
	showMenu();

	// Nouvelle partie
	newGame();

	// On démarre le chrono
    time(&depart);

	// Gestion des touches (boucle principale du programme)
	do{
		// Définition de la couleur du texte
		textbackground(B_TEXT);
		textcolor(C_TEXT);

		// On affiche le nombre de bombes restantes
		gotoMenu(6);
		printf("Bombs : %d   ", nbBombs-nbFlags);

		// On affiche le meilleur score
		if(lastScore != -1)
		{
			gotoMenu(8);
			printf("Last Score : %.0f    ", lastScore);
		}

		// On affiche le meilleur score
		if(bestScore != -1)
		{
			gotoMenu(9);
			printf("Best Score : %.0f    ", bestScore);
		}

		// Si le joueur perd
		if(gameOver == TRUE)
		{
			// Affichage des bombes
			showBombs();

			// On affiche le message
			msgZone(C_ERROR, B_ERROR);
			printf("BOOOOOM !");
			gotoxy(CONSOLE_W-BORDER-MENU_W+1, CONSOLE_H-BORDER-2);
			printf("Retry ? Press \"r\"");

			// Pour l'esthétique, on remet le curseur à sa place
			gotoCase(cursorX, cursorY);

			// On attend que la touche "r" ou "q" soit appuyer
			do{
				touche = getch();
			}while(touche != 114 && touche != 113);
		}
		else if(nbMined == nbCases-nbBombs) // Si le joueur gagne (le nombre de cases minées est égal au nombre total de cases moins les bombes)
		{
			// On calcul le temps mis pour gagner
			time(&arrivee);
			score = difftime(arrivee, depart);

			// On met à jour le meilleur score
			if(score < bestScore || bestScore == -1) bestScore = score;
			lastScore = score;

			// On affiche qu'il a gagner
			msgZone(C_SUCESS, B_SUCESS);
			printf("YOU WIN !");
			gotoxy(CONSOLE_W-BORDER-MENU_W+1, CONSOLE_H-BORDER-2);
			printf("Score : %.0f", score);

			// Pour l'esthétique, on remet le curseur à sa place
			gotoCase(cursorX, cursorY);

			// On attend que la touche "r" ou "q" soit appuyer
			do{
				touche = getch();
			}while(touche != 114 && touche != 113);
		}
		else
		{
			// On remet le curseur là où le joueur était
			gotoCase(cursorX, cursorY);

			//On récupère la touche appuyer
			touche = getch();
		}

		// Lorsque quelqu'une fonction demande à ce que le message soit effacer
		if(needToClear == TRUE)
		{
			// On efface le message précédent
			clearMsg();
		}

		// Si c'est un mouvement
		if(touche == -32)
		{
			// On récupère le sens
			touche = getch();
			switch(touche)
			{
				case 75:if(cursorX != 0) cursorX--; break; // gauche
				case 77:if(cursorX != gameX-1) cursorX++;break; // droite
				case 80:if(cursorY != gameY-1) cursorY++;break; // bas
				case 72:if(cursorY != 0) cursorY--; break; // haut
			}

			// On déplace le curseur
			gotoCase(cursorX, cursorY);
		}
		else if(touche == 32) // S'il selectionne la case (espace)
		{
			mineCase(cursorX, cursorY); // On execute la fonction de minage
		}
		else if(touche == 102) // S'il pose un drapeau (touche "f")
		{
			putFlag(cursorX, cursorY); // On execute la fonction pour poser/enlever un drapeau
		}
		else if(touche == 114) // Touche pour commencer une nouvelle partie (touche "r")
		{
			// Nouvelle partie
			newGame();

			// On remet le curseur en haut à gauche
			cursorX = 0;
			cursorY = 0;
			gotoCase(cursorX, cursorY);

			// On remet à zéro le chrono
			time(&depart);
		}
		else if(touche == 98) // Touche de debug pour voir les bombes (touche B)
		{
			// Touche de debug/triche pour afficher les bombes
			showBombs();
		}
		else if(touche == '1') // Touche pour aller au niveau 1 (oui c'est une fonction caché à l'utilisateur mais c'est pas terminé même si ça fonctionne)
		{
			gameX=10;
			gameY=10;
			nbBombs=5;
			nbCases=gameX*gameY;
			showBorders();
			newGame();
			cursorX = 0;
			cursorY = 0;
			gotoCase(cursorX, cursorY);
			time(&depart);
		}
		else if(touche == '2') // Touche pour aller au niveau 2
		{
			gameX=30;
			gameY=15;
			nbBombs=20;
			nbCases=gameX*gameY;
			showBorders();
			newGame();
			cursorX = 0;
			cursorY = 0;
			gotoCase(cursorX, cursorY);
			time(&depart);
		}
		else if(touche == '3') // Touche pour aller au niveau 2
		{
			gameX=50;
			gameY=20;
			nbBombs=55;
			nbCases=gameX*gameY;
			showBorders();
			newGame();
			cursorX = 0;
			cursorY = 0;
			gotoCase(cursorX, cursorY);
			time(&depart);
		}
	}
	while(touche != 113); // Touche pour quitter le jeu

	// On termine la fonction main donc le programme
	return TRUE;
}

// Génère un tableau entièrement de 0
void generateGame()
{
	int x, y; // Variables utiles

	for(x = 0; x < gameX; x++)
	{
		for(y = 0; y < gameY; y++)
		{
			game[x][y] = GAME_NULL; // On met toutes les cases à l'état d'innocence
		}
	}
}

// Génération des bombes
void generateBombs()
{
	int placed = 0, x, y; // Variable de recherche

	// On tourne jusqu'à ce que l'on est placé toutes les bombes
	while(placed != nbBombs)
	{
		x = rand()%gameX; // Valeur aléatoire pour x
		y = rand()%gameY; // Valeur aléatoire pour y

		// On vérifie que la bombe n'existe pas déjà
		if(game[x][y] != GAME_BOMB)
		{
			game[x][y] = GAME_BOMB;
			placed++;
		}
	}
}

// Affiche le jeu
void showBorders()
{
	int x, y; // Variable utiles

	offsetX = ((CONSOLE_W-(BORDER*3)-MENU_W-gameX)/2)+BORDER;
	offsetY = ((CONSOLE_H-(BORDER*2)-gameY)/2)+BORDER;

	// On choisit nos couleurs
	textbackground(B_BORDER);
	textcolor(C_BORDER);

	// On parcourt la fenetre du haut vers le bas
	for(y = 0; y < CONSOLE_H; y++)
	{
		// On se positionne à la première case de la ligne
		gotoxy(0, y);

		// On rempli d'une bordure entière les premières et dernières lignes
		if(y < BORDER || y >= CONSOLE_H-BORDER)
		{
			for(x = 0; x <= CONSOLE_W; x++)
			{
				printf("%c", A_BORDER);
			}
		}
		else
		{
			if(y < offsetY || y >= offsetY+gameY)
			{
				for(x = 0; x < CONSOLE_W-BORDER-MENU_W; x++)
				{
					printf("%c", A_BORDER);
				}
			}
			else //Espace où se trouve le jeu
			{
				// Bordure de gauche
				for(x = 0; x < offsetX; x++)
				{
					printf("%c", A_BORDER);
				}

				// Bordure du milieu
				gotoxy(gameX+offsetX, y);
				for(x = 0; x < CONSOLE_W-BORDER-MENU_W-gameX-offsetX; x++)
				{
					printf("%c", A_BORDER);
				}
			}

			// Bordure de droite
			gotoxy(CONSOLE_W-BORDER, y);
			for(x = 0; x < BORDER; x++)
			{
				printf("%c", A_BORDER);
			}
		}
	}

	// Pour corriger un problème d'esthétique (essayer sans pour comprendre)
	gotoxy(0, 0);
}

// Génération du menu
void showMenu()
{
	// Définition de la couleur du texte
	textbackground(B_TEXT);
	textcolor(C_TEXT);

	// On écris les touches
	gotoMenu(1);
	printf("Select\t: SPACE");
	gotoMenu(2);
	printf("Flag\t: f");
	gotoMenu(3);
	printf("Restart\t: r");
	gotoMenu(4);
	printf("Quit\t: q");
}

// Affiche le jeu
void showGame()
{
	int x, y; // Variable de recherche

	// On choisit les couleurs
	textbackground(B_MAP);
	textcolor(C_MAP);
	/*
	for(x = 0; x < gameX; x++)
	{
		for(y = 0; y < gameY; y++)
		{
			// On se place sur la case
			gotoCase(x, y);

			// On affiche
			printf("%c", A_MAP);
		}
	}
	*/
	for(y = 0; y < gameY; y++)
	{
		// On se place sur la case
		gotoCase(0, y);
		for(x = 0; x < gameX; x++)
		{
			// On affiche
			printf("%c", A_MAP);
		}
	}
}

void showBombs()
{
	int placed = 0, x, y; // Variable de recherche

	for(x = 0; x < gameX; x++)
	{
		for(y = 0; y < gameY; y++)
		{
			if(game[x][y] == GAME_BOMB)
			{
				// On se place sur la case
				gotoCase(x, y);

				//On choisit la couleur et on affiche
				textbackground(B_BOMB);
				textcolor(C_BOMB);
				printf("%c", A_BOMB);
			}
		}
	}
}

// Permet de se déplacer sur une case du jeu
// Prend en paramètre les coordonnées de "game" et les convertis en coordonnées réels sur la console
void gotoCase(int x, int y)
{
	gotoxy(x+offsetX, y+offsetY);
}

// Permet de se déplacer sur une ligne du menu pour écrire
void gotoMenu(int ligne)
{
	gotoxy(CONSOLE_W-BORDER-MENU_W+2, BORDER+ligne);
}

// Fonction executé lorsque l'utilisateur choisit une case
void mineCase(int x, int y)
{
	int countBomb = 0; // permet de compter les bombes aux alentours
	int i, j; // Variable de recherche

	// On vérifie que c'est pas la deuxième fois que l'utilisateur appuie ici
	if(game[x][y] == GAME_NULL)
	{
		nbMined++;
	}

	// On se positionne sur la case à miner
	gotoCase(x, y);

	if(game[x][y] == GAME_BOMB)
	{
		// S'il mine une bombe, la partie est teminé
		endGame();
	}
	else
	{
		//
		//	On compte les bombes aux alentours
		//

		// On définie la première case de recherche sur l'axe Y
		if(x != 0) i = x-1;
		else i = x;

		// On parcourt en Y
		for (i; i <= x+1 && i < gameX; i++)
		{
			// On définie la première case de recherche sur l'axe X
			if(y != 0) j = y-1;
			else j = y;

			// Puis en X
			for (j; j <= y+1 && j < gameY; j++)
			{
				if(i != x || j != y)
				{
					if(game[i][j] == GAME_BOMB)
						countBomb++; // S'il y a une bombe, on l'ajoute
				}
			}
		}

		// On vérifie que le joueur n'avait pas posé un drapeau ici
		if(flags[x][y] == GAME_FLAG)
			removeFlag(x, y); // On l'enlève si c'était le cas

		// On indique que l'on a miné ici
		game[x][y] = countBomb;

		// On choisit nos couleurs
		textbackground(B_MINED);
		textcolor(C_MINED);

		if(countBomb > 0) // S'il y a des bombes aux alentours on affiche le nombre
		{
			// On change la couleur
			switch(countBomb)
			{
				case 1:textcolor(LIGHTBLUE);break;
				case 2:textcolor(LIGHTGREEN);break;
				case 3:textcolor(LIGHTRED);break;
				case 4:textcolor(BLUE);break;
				case 5:
				case 6:
				case 7:
				case 8:textcolor(RED);break;
			}

			// On affiche le nombre
			printf("%d", countBomb);
		}
		else // Sinon on affiche un vide et on va voir plus loin
		{
			printf("%c", A_MINED);

			//
			// On mine aussi aux alentours
			//

			// On définie la première case de recherche sur l'axe Y
			if(x != 0) i = x-1;
			else i = x;

			// On parcourt en Y
			for (i; i <= x+1 && i < gameX; i++)
			{
				// On définie la première case de recherche sur l'axe X
				if(y != 0) j = y-1;
				else j = y;

				// Puis en X
				for (j; j <= y+1 && j < gameY; j++)
					if((i != x || j != y) && game[i][j] == GAME_NULL)
						mineCase(i, j); // On mine à coté
			}
		}
	}
}

// Permet de poser un drapeau
void putFlag(int x, int y)
{
	// On execute le contenu seulement si la case n'a pas été minée
	if(game[x][y] == GAME_NULL || game[x][y] == GAME_BOMB)
	{
		// On vérifie qu'il n'y a pas déjà un drapeau
		if(flags[x][y] != GAME_FLAG)
		{
			// On vérifie que l'on a pas mis plus de drapeau que de bombes
			if(nbFlags < nbBombs)
			{
				// On choisit nos couleurs
				textbackground(B_FLAG);
				textcolor(C_FLAG);

				// On affiche le drapeau
				printf("%c", A_FLAG);

				// On ajoute le drapeau à la liste
				addFlag(x, y);
			}
			else
			{
				// On affiche une erreur
				msgZone(C_WARNING, B_WARNING);
				printf("You can't use more");
				gotoxy(CONSOLE_W-BORDER-MENU_W+1, CONSOLE_H-BORDER-2);
				printf("than %d flags", nbBombs);
			}
		}
		else // Sinon on enlève le drapeau existant
		{
			// On choisit nos couleurs
			textbackground(B_MAP);
			textcolor(C_MAP);

			// On remet un case non miné
			printf("%c", A_MAP);

			// On enleve le drapeau du tableau
			removeFlag(x, y);
		}
	}
}


// Ajoute un drapeau dans le tableau
void addFlag(int x, int y)
{
	flags[x][y] = GAME_FLAG;
	nbFlags++;
}

// Enlève un drapeau du tableau
void removeFlag(int x, int y)
{
	// Enlève le drapeau
	flags[x][y] = GAME_NULL;

	// On réduit le nombre de flags
	nbFlags--;
}

// Nouveau Jeu
void newGame()
{
	// On génère un tableau vide
	generateGame();

	// On génère des bombes aléatoires
	generateBombs();

	// On affiche le jeu
	showGame();

	// On réinitialise les variables
	gameOver = FALSE;
	nbMined = 0;
	nbFlags = 0;
}

// Fin du jeu
void endGame()
{
	// On affiche le jeu
	showBombs();

	// On indique que le jeu est fini
	gameOver = TRUE;
}

// Permet de créer une zone coloré pour écrire un message
void msgZone(int color, int background)
{
	// Variables utiles
	int x, y;

	// Les couleurs
	textbackground(background);
	textcolor(color);

	for(y = CONSOLE_H-BORDER-4; y < CONSOLE_H-BORDER; y++)
	{
		// On se positionne
		gotoxy(CONSOLE_W-BORDER-MENU_W, y);

		for(x = 0; x < MENU_W; x++)
		{
			printf("%c", 32); // Ajoute des espaces
		}
	}

	//On position le curseur pour le message
	gotoxy(CONSOLE_W-BORDER-MENU_W+1, CONSOLE_H-BORDER-3);

	needToClear = TRUE;
}

// Effacer la zone de message
void clearMsg()
{
	// On efface
	msgZone(B_TEXT, B_TEXT);
	// On indique que s'est effacer
	needToClear = FALSE;
}
