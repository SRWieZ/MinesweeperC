#include <stdio.h>
#include <stdlib.h>
#include "myconio.h"

#define TRUE 1
#define FALSE 0

// Variables de tailles de la fenetre et des elements d'affichages
#define BORDER 2
#define CONSOLE_W 80
#define CONSOLE_H 25

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
#define MAX_X 20
#define MAX_Y 50
#define NB_BOMBS 25

/*

Démineur par DENIZ Eser

Ce jeu contient une version modifier de myconio pour enlever les x - 1 et y - 1 de goto(x,y)
Ici la case 0,0 est en haut à gauche
*/

// Variables globales
int game[MAX_Y][MAX_X], nbCases = 1000, gameY = 10, gameX = 10; // Carte où seront stocké nos informations
int nbBombs = 5; // Nombre de bombes
int nbMined = 0; // Nombre de cases mines
int gameOver = FALSE;

// Headers des fonctions
void generateGame();
void generateBombs();
void showGame();
void gotoCase();
void mineCase();
void newGame();
void endGame();

// Fonction main
main()
{
    char touche;	// Variable où sera stocké la touche
    int cursorX = 0, cursorY = 0; // Position du curseur
	time_t depart, arrivee; // Variables de temps

	// On choisit le titre
	system("TITLE Minesweeper");

	// On force la taille de la fenetre
	system("mode con LINES=25 COLS=80");

	// Activation de rand()
	srand(time(NULL));

	// Nouvelle partie
	newGame();

	// Gestion des touches
	do{

		// On remet le curseur là où le joueur joue
		gotoCase(cursorX, cursorY);

		//On récupère la touche appuyer
		touche = getch();

		// Si c'est un mouvement, on se déplace
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
			mineCase(cursorX, cursorY);
		}
		else if(touche == 102) // S'il pose un drapeau (touche "f")
		{
			//putFlag(x, y);
		}
		else if(touche == 114) // Touche pour commencer une nouvelle partie (touche "r")
		{
			newGame();
			gotoCase(0, 0);

			// On remet à zéro le chrono
			time(&depart);
		}
		else if(touche == 98) // Touche de debug pour voir les bombes (touche B)
		{
			showBombs();
		}
		else if(touche == 113) // Touche de debug pour voir les bombes (touche q)
		{
			return TRUE;
		}
	}
	while(touche != '0');

}

// Génère un tableau entièrement à 0
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
void showGame()
{
	int x, y; // Variable de recherche

	for(x = 0; x < gameX; x++)
	{
		for(y = 0; y < gameY; y++)
		{
			// On se place sur la case
			gotoCase(x, y);

			// On affiche
			textbackground(B_MAP);
			textcolor(C_MAP);
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
	gotoxy(x, y);
}

// Fonction executé lorsque l'utilisateur choisit une case
void mineCase(int x, int y)
{
	int countBomb = 0; // permet de compter les bombes aux alentours
	int i, j; // Variable de recherche

	// On vérifie que c'est pas la deuxième fois que l'utilisateur appuie ici
	if(game[x][y] != GAME_NULL)
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
		//if(game[x][y] == GAME_FLAG)
		//	removeFlag(x, y); // On l'enlève si c'était le cas

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
				case 5:textcolor(RED);break;
				case 6:textcolor(RED);break;
				case 7:textcolor(RED);break;
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
}

// Fin du jeu
void endGame()
{
	// On affiche le jeu
	showBombs();

	// On indique que le jeu est fini
	gameOver = TRUE;
}
