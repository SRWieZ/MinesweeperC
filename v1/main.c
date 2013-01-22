#define TRUE 1
#define FALSE 0

#define CONSOLE_W 80
#define CONSOLE_H 25

#define OFFSET_X 2
#define OFFSET_Y 2

#define BORDER 2

// Codes ascii  de l'affichages
#define A_BORDER 219
#define A_BOMB 66
#define A_MINED 250
#define A_MAP 254
#define A_FLAG 33

#define C_BORDER DARKGRAY
#define C_BOMB LIGHTRED
#define C_MINED LIGHTGRAY
#define C_MAP LIGHTGRAY
#define C_FLAG GREEN
#define C_TEXT WHITE
#define C_ERROR WHITE
#define C_WARNING WHITE
#define C_SUCESS WHITE

#define B_BORDER BLACK
#define B_BOMB WHITE
#define B_MINED WHITE
#define B_MAP WHITE
#define B_FLAG WHITE
#define B_TEXT BLACK
#define B_ERROR LIGHTRED
#define B_WARNING MAGENTA
#define B_SUCESS GREEN

#define NB_CASES 1000
#define NB_BOMBS 25

#include <stdio.h>
#include <stdlib.h>
#include "myconio.h"

/*

Démineur par DENIZ Eser

Ce jeu contient une version modifier de myconio pour enlever les x - 1 et y - 1 de goto(x,y)
Ici la case 0,0 est en haut à gauche
*/

void generateBombs();
void generateMap();
void showBorder();
void showMenu();
void mineCase(int x, int y);
int isMined(int x, int y);
int isBomb(int x, int y);
int isFlags(int x, int y);
void showBombs();
void putFlag(int x, int y);
void addFlag(int x, int y);
void removeFlag(int x, int y);
void newGame();
void endGame();
void msgZone(int color, int background);
void clearMsg();

int mapY = 20, mapX = 50;								// Taille de la carte
int minedY[NB_CASES], minedX[NB_CASES], nbMined = 0;	// Endroits miner sur l'axe x et l'axe y
int bombsY[NB_BOMBS], bombsX[NB_BOMBS], nbBombs ;				// Position des bombes sur l'axe x et l'axe y
int flagsY[NB_BOMBS], flagsX[NB_BOMBS], nbFlags = 0;	// Position des drapeaux et décompte
int needToClear = FALSE;									// Permet de demander à effacer la zone de message
int gameOver = FALSE;

main()
{

    char touche;	// Variable où sera stocké la touche
	int y = OFFSET_Y, x = OFFSET_X;		// Variable de la position du pointeur de l'utilisateur
	double bestScore = -1, score = -1, lastScore = -1;

	time_t depart, arrivee; // Variables de temps

	// Activation de rand()
	srand(time(NULL));

	// Génération du menu
	showBorder();

	// Génération du menu
	showMenu();

	// Nouvelle partie
	newGame();

	// On démarre le chrono
    time(&depart);

	// Gestion des touches
	do{
		// Définition de la couleur du texte
		textbackground(B_TEXT);
		textcolor(C_TEXT);

		// On affiche le nombre de bombes restantes
		gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+8);
		printf("Bombs : %d   ", NB_BOMBS-nbFlags);

		if(lastScore != -1)
		{
			// On affiche le meilleur score
			gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+12);
			printf("Last Score : %.0f    ", lastScore);
		}

		if(bestScore != -1)
		{
			// On affiche le meilleur score
			gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+13);
			printf("Best Score : %.0f    ", bestScore);
		}


		if(gameOver == TRUE)
		{
			// Affichage des bombes
			showBombs();

			// On affiche le message
			msgZone(C_ERROR, B_ERROR);
			printf("BOOOOOM !");
			gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+18);
			printf("Retry ? Press \"r\"");

			// Pour l'esthétique, on remet le curseur à sa place
			gotoxy(x, y);

			do{
				touche = getch();
			}while(touche != 114);
		}
		else if(nbMined == NB_CASES-NB_BOMBS)
		{
			// On calcul le temps mis pour gagner
			time(&arrivee);
			score = difftime(arrivee, depart);

			if(score < bestScore || bestScore == -1) bestScore = score;
			lastScore = score;

			// On affiche qu'il a gagner
			msgZone(C_SUCESS, B_SUCESS);
			printf("YOU WIN !");
			gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+18);
			printf("Score : %.0f", score);
			// Pour l'esthétique, on remet le curseur à sa place
			gotoxy(x, y);

			do{
				touche = getch();
			}while(touche != 114);
		}
		else
		{
			// On remet le curseur là où le joueur joue
			gotoxy(x, y);

			//On récupère la touche appuyer
			touche = getch();
		}

		//printf("\n%c %d", touche, touche); // DEBUG : permet de voir le code de la touche

		if(needToClear == TRUE)
		{
			clearMsg(); // On efface le message précédent
			gotoxy(x, y); // On remet le curseur là où le joueur joue
		}

		// Si c'est un mouvement, on se déplace
		if(touche == -32)
		{
			// On récupère le sens
			touche = getch();
			switch(touche)
			{
				case 75:if(x != 0+OFFSET_X) x--; break; // gauche
				case 77:if(x != mapX-1+OFFSET_X) x++;break; // droite
				case 80:if(y != mapY-1+OFFSET_Y) y++;break; // bas
				case 72:if(y != 0+OFFSET_Y) y--; break; // haut
			}

			// On déplace le curseur
			gotoxy(x, y);
		}
		else if(touche == 32) // S'il selectionne la case (espace)
		{
			mineCase(x, y);
		}
		else if(touche == 102) // S'il pose un drapeau (touche "F")
		{
			putFlag(x, y);
		}
		else if(touche == 114) // Touche pour commencer une nouvelle partie
		{
			newGame();
			x = OFFSET_X;
			y = OFFSET_Y;
			gotoxy(OFFSET_X, OFFSET_Y);

			// On remet à zéro le chrono
			time(&depart);
		}
		else if(touche == 113) // Touche pour quitter
		{
			return 1;
		}
		else if(touche == 51) // Touche de debug pour voir les bombes (touche B)
		{
			showBombs();
		}
	}
	while(touche != '0');
}

// Génération des bombes
void generateBombs()
{
	int present = FALSE;	// Varaible de vérification
	int i, j; // Variable de recherche

	for(i = 0; i < NB_BOMBS; i++)
	{
		bombsY[i] = rand()%mapY+OFFSET_Y;
		do{
			bombsX[i] = rand()%mapX+OFFSET_X;
			present = FALSE;
			// Vérification si la valeur n'existe pas
			for(j = 0; j < NB_BOMBS; j++)
			{
				if(bombsY[j] == bombsY[i] && j != i)		// Si dans le même y
				{
					if(bombsX[j] == bombsX[i])	// et dans le même x ya la même valeur que celle définie au dessus
						present = TRUE;
				}
			}
		}
		while(present == TRUE); // tant qu'on retrouve pas le même

		present = FALSE;
	}
}

// Génération de la map
void generateMap()
{
	int i, j; // Variable de recherche

	//On choisit la couleur
	textbackground(B_MAP);
	textcolor(C_MAP);

    for(i = OFFSET_Y; i < mapY+OFFSET_Y; i++)
	{
		gotoxy(OFFSET_X, i);
        for(j = OFFSET_X; j < mapX+OFFSET_X; j++)
		{
            printf("%c", A_MAP);
		}
	}
}

// Génération du menu
void showBorder()
{
	int i, j, h;

	// On choisit nos couleurs
	textbackground(B_BORDER);
	textcolor(C_BORDER);

	for(i = 0; i < CONSOLE_H; i++)
	{
		gotoxy(0, i);

		if(i < OFFSET_Y || i >= mapY+OFFSET_Y)
		{
			for(j = 0; j < CONSOLE_W; j++)
			{
				printf("%c", A_BORDER);
			}
		}
		else
		{
			for(h=0;h<BORDER;h++)
			{
				printf("%c", A_BORDER);
			}

			gotoxy(mapX+OFFSET_X, i);

			for(h=0;h<BORDER;h++)
			{
				printf("%c", A_BORDER);
			}

			gotoxy(CONSOLE_W-BORDER, i);

			for(h=0;h<BORDER;h++)
			{
				printf("%c", A_BORDER);
			}
		}
	}

	gotoxy(0, 0);
}

// Génération du menu
void showMenu()
{
	// Définition de la couleur du texte
	textbackground(B_TEXT);
	textcolor(C_TEXT);

	// On écris les touches
	gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+2);
	printf("Select\t: SPACE");
	gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+3);
	printf("Flag\t: f");
	gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+4);
	printf("Restart\t: r");
	gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+5);
	printf("Quit\t: q");
}

// Fonction executé lorsque l'utilisateur choisit une case
void mineCase(int x, int y)
{
	int countBomb = 0; // permet de compter les bombes aux alentours
	int i, j; // Variable de recherche

	// On vérifie que c'est pas la deuxième fois que l'utilisateur appuie ici
	if(isMined(x, y) != TRUE)
	{
		// On marque cette position comme déjà miner
		minedX[nbMined] = x;
		minedY[nbMined] = y;
		nbMined++;
	}

	// On se positionne sur la case à miner
	gotoxy(x, y);

	if(isBomb(x, y))
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
		if(y != OFFSET_Y) i = y-1;
		else i = y;

		// On parcourt en Y
		for (i; i <= y+1 && i < mapY+OFFSET_Y; i++)
		{
			// On définie la première case de recherche sur l'axe X
			if(x != OFFSET_X) j = x-1;
			else j = x;

			// Puis en X
			for (j; j <= x+1 && j < mapX+OFFSET_X; j++)
			{
				if(i != y || j != x)
				{
					if(isBomb(j, i) == TRUE)
						countBomb++; // S'il y a une bombe, on l'ajoute
				}
			}
		}

		// On vérifie que le joueur n'avait pas posé un drapeau ici
		if(isFlag(x, y))
			removeFlag(x, y); // On l'enlève si c'était le cas

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
			if(y != OFFSET_Y) i = y-1;
			else i = y;

			// On parcourt en Y
			for (i; i <= y+1 && i < mapY+OFFSET_Y; i++)
			{
				// On définie la première case de recherche sur l'axe X
				if(x != OFFSET_X) j = x-1;
				else j = x;

				// Puis en X
				for (j; j <= x+1 && j < mapX+OFFSET_X; j++)
					if((i != y || j != x) && isMined(j, i) != TRUE)
						mineCase(j, i); // On mine à coté
			}
		}
	}
}

// Affiche toute les bombes
int isMined(int x, int y)
{
	int present = FALSE; // Variable de retour
	int i; // Variable de recherche

	for(i = 0; i < nbMined && present == FALSE; i++)
		if(minedY[i] == y)
			if(minedX[i] == x)
				present = TRUE;

	return present;
}

// Affiche toute les bombes
int isBomb(int x, int y)
{
	int present = FALSE; // Variable de retour
	int i; // Variable de recherche

	for(i = 0; i < NB_BOMBS && present == FALSE; i++)
		if(bombsY[i] == y)
			if(bombsX[i] == x)
				present = TRUE;

	return present;
}

// Affiche toute les bombes
int isFlag(int x, int y)
{
	int present = FALSE; // Variable de retour
	int i; // Variable de recherche

	for(i = 0; i < nbFlags && present == FALSE; i++)
		if(flagsY[i] == y)
			if(flagsX[i] == x)
				present = TRUE;

	return present;
}

// Affiche toute les bombes
void showBombs()
{
	int i; // Variable de recherche

	// On choisit nos couleurs
	textbackground(B_BOMB);
	textcolor(C_BOMB);

	for(i = 0; i < NB_BOMBS; i++)
	{
		//printf("(%d, %d)\n", bombsY[i], bombsX[i]);
		gotoxy(bombsX[i], bombsY[i]);
		printf("%c", A_BOMB);
	}
}

// Permet de poser un drapeau
void putFlag(int x, int y)
{
	// On execute le contenue seulement si la case n'a pas été minée
	if(isMined(x, y) == FALSE)
	{
		// On vérifie qu'il n'y a pas déjà un drapeau
		if(isFlag(x, y) == FALSE)
		{
			// On vérifie que l'on a pas mis plus de drapeau que de bombes
			if(nbFlags < NB_BOMBS)
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
				gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+18);
				printf("than %d flags", NB_BOMBS);
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
	flagsX[nbFlags] = x;
	flagsY[nbFlags] = y;
	nbFlags++;
}

// Enlève un drapeau du tableau et le réorganise
void removeFlag(int x, int y)
{
	//Variable utile
	int i, j , present = FALSE;

	// On met à -1 les entrés que l'on veut supprimer
	for(i = 0; i < nbFlags && present == FALSE; i++)
	{
		if(flagsY[i] == y)
		{
			if(flagsX[i] == x)
			{
				flagsY[i] = -1;
				flagsX[i] = -1;
				present = TRUE;
			}
		}
	}

	// On supprime les entrés noté -1 En décalant tout d'un cran
	for(i=0, j = 0; i<10; i++)
	{
		if(flagsY[i] != -1)
		{
			flagsY[j] = flagsY[i];
			flagsX[j] = flagsX[i];
			j++;
		}
	}

	// On réduit le nombre de flags
	nbFlags--;
}

// Nouveau Jeu
void newGame()
{
	int i;

	// Remise à zéro des variables
	for(i = 0; i < NB_CASES; i++)
	{
		minedY[i] = -1;
		minedX[i] = -1;

		if(i <= NB_BOMBS)
		{
			bombsY[i] = -1;
			bombsX[i] = -1;
			flagsY[i] = -1;
			flagsX[i] = -1;
		}
	}

	nbFlags = 0;
	nbMined = 0;

	// Génération des bombes
	generateBombs();

	// Génération de la map
	generateMap();

	gameOver = FALSE;
}

// Fin du jeu
void endGame()
{
	gameOver = TRUE;
}

void msgZone(int color, int background)
{
	// Variables utiles
	int i, j;

	// Les couleurs
	textbackground(background);
	textcolor(color);

	for(i = OFFSET_Y+16; i < mapY+OFFSET_Y; i++)
	{
		for(j = mapX+OFFSET_X+BORDER; j < CONSOLE_W-BORDER; j++)
		{
			// On se positionne
			gotoxy(j, i);
			printf("%c", 32);
		}
	}

	//On position le curseur pour le message
	gotoxy(mapX+OFFSET_X+(BORDER*2), OFFSET_Y+17);

	needToClear = TRUE;
}

void clearMsg()
{
	// On efface
	msgZone(B_TEXT, B_TEXT);
	// On indique que s'est effacer
	needToClear = FALSE;
}
