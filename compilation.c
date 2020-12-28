#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "compilation.h"
#define szASCII 128

/*alocation */

struct automate_fini_non_deterministe alouer_AFN(int  nb_etat, int accept_F){

	struct automate_fini_non_deterministe a;
	
	a.Q = nb_etat;
	a.accept_F = accept_F;
	a.s = 0;
	a.F = malloc (sizeof(int) * a.accept_F);
	a.delta = malloc (sizeof(struct transitions) * a.Q);
	for (int i = 0; i < a.Q; i++)
	{
		a.delta[i] = malloc (sizeof(struct transitions) * szASCII);

	}

	return a;
}


void free_AFN(struct automate_fini_non_deterministe a)
{
	int i, j, k;
	for (i = 0; i < a.Q; i++)
	{
		for (j = 0; j < szASCII; j++)
		{
			for (k = 0; k < a.delta[i][j].taille; k++)
			{
				free (a.delta[i][j].etat);
			}
		}
		free (a.delta[i]);
	}
	free(a.delta);
	free(a.F);
}

/*  fonctions de base */

struct automate_fini_non_deterministe langage_vide(){
	
	struct automate_fini_non_deterministe  a = alouer_AFN(1,0);// langage vide contient un seul etat et pas d'etat accepteur 
	a.s=0;
	return a ;
}


struct automate_fini_non_deterministe un_seul_mot_vide(){

	struct automate_fini_non_deterministe  a = alouer_AFN(1,1);// un seul etat et un seul etat accepteur(initial=accepteur)
	a.s=0;
	a.F[0]=0;//etat initial le seul etat accepteur
	
	return a ;
}

struct automate_fini_non_deterministe mot_seul_caractere(char c){

	struct automate_fini_non_deterministe a = alouer_AFN(2,1);// 2 etat: 1 initial et 1 accepteur
	a.s = 0 ;
	a.F[0] = 1;

	// une transition de 0 a 1 par char c 
	a.delta[0][(int)c].taille = 1;
	a.delta[0][(int)c].etat = malloc (sizeof(int) * a.delta[0][(int)c].taille);
	a.delta[0][(int)c].etat[0] = 1;
	return a;	
	
}

/* AFFICHAGE DES AUTOMATES */

void affiche_automate_fini_non_deterministe(struct automate_fini_non_deterministe a) {

	int i, j, k;
	printf("L'automate non deterministe a %d etat(s)\n",a.Q);
	printf("Son etat initial est %d\n", a.s);
	printf("Son/Ses etat(s) final(s) sont {" );
	for (i = 0; i < a.accept_F; i++) {
		printf(" %d " , a.F[i]);
	}
	printf("}\n");
	printf("Ses transitions sont :\n" );
	for (i = 0; i < a.Q; i++) {
		for (j = 0; j < szASCII; j++) {
			if ( a.delta[i][j].taille>0)
			for (k = 0; k < a.delta[i][j].taille; k++) {
				printf("\t %d '%c' %d\n ", i, j, a.delta[i][j].etat[k]);
			}
		}
	}
	printf("fin  \n" );
}



int main (int argc, char const *argv[])
{

struct automate_fini_non_deterministe  a ;
a = mot_seul_caractere('a');
affiche_automate_fini_non_deterministe(a);

return 0;
}

