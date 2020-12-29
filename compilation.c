#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "compilation.h"
#define szASCII 128

/* ALOCATION */

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

struct automate_fini_deterministe alouer_AFD (int  nb_etat, int accept_F)
{
	int i, j ;
	struct automate_fini_deterministe a;
	a.Q = nb_etat;
	a.accept_F = accept_F;
	a.s = 0;
	a.F = malloc (sizeof(int) * a.accept_F);
	
	a.delta = malloc (sizeof( int*)*a.Q);
	for (i = 0; i < a.Q; i++)
	{
		a.delta[i] = malloc (sizeof( int) * szASCII);
	}
	for ( i = 0; i < a.Q; i++)
		for (j = 0; j < szASCII; j++)
		{
			a.delta[i][j] = -1;
		}
	
	return a;
}
//free

/*  FONCTIONS DE BASE */

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

/*FONCTIONS POUR RECONNAITRE DES LANGAGES PLUS EVOLUES*/

unsigned verif_etat_tab(int etat, int *tab, int n) { 
// verifie si l'etat est dans tableau
	int i;
	for (i = 0; i < n; i++) {
		if (tab[i] == etat) {
			return 1;
		}
	}
	return 0;
}


struct automate_fini_non_deterministe reunion_deux_langages(struct automate_fini_non_deterministe a1, struct automate_fini_non_deterministe a2) { 
        // la reunion va se faire comme suit :
	// 1 : on alloue un nouveau automate avec |a1|+|a2|-1 etats
	// 2 : les etats finals du a1 vont etre copiés dans le nouveau  alors que ceux du a2 vont s'incrementer de a1.Q - 1
	// 3 : l'etat initial du nouveau automate va etre un etat qui contient les transitions de a1 identiques avec ceux de a2 incrementés
	// 4 : on copie les etats de a1 dans le nouveau automate a partir de l'etat 1 da a1 dans l'etat 1 du nouveau langage (non 0)
	// 5 : on copie les etats de a2 dans le nouveau automate a partir de l'etat 1 de a2 en les incrementants.
	
	int i,k,j,x  = 0;	
	int T = a1.accept_F + a2.accept_F;// nb d'etat accepteur des 2 automates
	
	if ( verif_etat_tab(0, a1.F, a1.accept_F) && verif_etat_tab(0, a2.F, a2.accept_F) ){
		T--;
	}// si les 2 etats initaux sont accepteurs on decremente par 1 vu qu'on enlevra un etat au total
	
	struct automate_fini_non_deterministe R = alouer_AFN(a1.Q + a2.Q - 1 , T);//taille nouveau automate cest la taille des 2 automtes -1 		vu qu'on enleve 2 etats initals et on ajoute un autre

	R.s = 0; //etat inital de l'automate resultant de la reunion

	//etats finals: on copie les etats finals de a1 dans R
	for (i = 0; i < a1.accept_F; i++) {
		R.F[j++] = a1.F[i];
	}
	
	//etats finals: on copie les etats finals de a2 dans R en incrementant, on verifie si etat inital accepteur dans a2 et pas dans a1 on 		rend l'etat inital de R accepteur sinon op deja faite avec a1
	for (i= 0; i<a2.accept_F; i++){
	    if (a2.F[i] != 0) 
		R.F[j++] = (a2.F[i] + a1.Q) - 1;
	   
	    else if ( !(verif_etat_tab(0, a1.F, a1.accept_F)))		
		R.F[j++] = a2.F[i];	
	}
	
	// etat initial :
	for (i = 0; i < szASCII; i++) {
		if (a1.delta[0][i].etat != NULL && a2.delta[0][i].etat != NULL) {
			R.delta[0][i].taille = a1.delta[0][i].taille + a2.delta[0][i].taille;
			R.delta[0][i].etat = malloc(sizeof(int) * R.delta[0][i].taille);
		}
		else if (a1.delta[0][i].etat != NULL && a2.delta[0][i].etat == NULL) {
			R.delta[0][i].taille = a1.delta[0][i].taille;
			R.delta[0][i].etat = malloc(sizeof(int) * R.delta[0][i].taille);
		}
		else if (a2.delta[0][i].etat != NULL && a1.delta[0][i].etat == NULL) {
			R.delta[0][i].taille = a2.delta[0][i].taille;
			R.delta[0][i].etat = malloc(sizeof(int) * R.delta[0][i].taille);
		}
		
		j = 0;

		for (k = 0; k < a1.delta[0][i].taille; k++) { // pour a1 on recopie les etats 
			R.delta[0][i].etat[j++] = a1.delta[0][i].etat[k];
		}
		
		for (k = 0; k < a2.delta[0][i].taille; k++) { // pour a2 on recopie les etats avec les numeros + taille de a1 -1 
			R.delta[0][i].etat[j++] = a2.delta[0][i].etat[k] + a1.Q - 1;
		}
	}
	

	k = 1;
	//copie etats de a1
	for (i = 1; i < a1.Q; i++) {
		for (j = 0; j < szASCII; j++) {
			R.delta[k][j].taille = a1.delta[i][j].taille;
			R.delta[k][j].etat = malloc(sizeof(int) * R.delta[k][j].taille);
			for (x = 0; x < R.delta[k][j].taille; x++) {
				R.delta[k][j].etat[x] = a1.delta[k][j].etat[x];
			}	
		}
		k++;
	}

	//copie etats de a2 + incrementation
	for (i = 1; i < a2.Q; i++) {
		for (j = 0; j < szASCII; j++) {
			if (a2.delta[i][j].taille> 0){
				R.delta[k][j].taille = a2.delta[i][j].taille;
				R.delta[k][j].etat = malloc(sizeof(int) * R.delta[k][j].taille);
				for (x = 0; x < R.delta[k][j].taille; x++) {
					R.delta[k][j].etat[x] = a2.delta[i][j].etat[x] + a1.Q - 1;
				}
			}
		}
		k++;
	}

	return R;
}



struct automate_fini_non_deterministe concatenation_deux_langages (struct automate_fini_non_deterministe a1,
        struct automate_fini_non_deterministe a2){
	// la concatenantion va se faire comme suit :
	// 1 : la taille du nouvel automate est |a1|+|a2| -1 
	// 2: le nombre des etats accepteurs est le nombre des etats accepteurs de a2 si a2.s n'est pas accepteur sinon la somme
	// des etats accepteurs de (a1 et a2) - 1
	// 3: on alloue le nouvel automate
	// 4:Les etas accepteurs : copier les etats accepteurs de a1 si s.a2 est accepteur et copier ceux de a2 en les incrementant
	// 5: tarnsitions:TODO
	int i, j, k;
	j = 0;
	i = 0;
	int test = 0;	

 
	/*VERIF  if ( (y.Q == 1 && y.taille_accepteur == 0 )  ) // si l'un des automate est le langage vide 
		return y;						// renvoyer le langage vide
	if ( (z.Q == 1 && z.taille_accepteur == 0 )  )
		return z;*/

	int nb_etat =  (a1.Q + a2.Q) - 1; // Enlever l'etat inital de a2
	int accept_F = a2.accept_F;

	if (verif_etat_tab(0, a2.F, a2.accept_F)) {// si l'etat inital de a2 est accepteur
	// on le supprime et on garde les etats accepteurs de a1 
		accept_F += a1.accept_F -1;
		test = 1;
	}

	struct automate_fini_non_deterministe R = alouer_AFN (nb_etat,accept_F);

	if (test == 1){
		for (i = 0; i < a1.accept_F; i++){ // copie des etats accepteurs de a1 si a2.s est accepteur
			R.F[i] = a1.F[i];
		}
	}
	
	j = i;
	for (; i < accept_F + 1; i++){ // copie des etats accepteur de a2 en ignorant letat initial + leur incrementation
		if (a2.s != a2.F[i - (a1.accept_F * test)] ){
			R.F[j] = a2.F[i - (a1.accept_F * test)] + a1.Q - 1;
			j++;
		}	
	}
	
	//copie de a1
	for (i = 0; i < a1.Q ; i++){
		for (j = 0; j < szASCII; j++){
			if (a1.delta[i][j].taille > 0  && verif_etat_tab(i, a1.F, a1.accept_F) == 0){ // etat non accepteur, on copie
				R.delta[i][j].etat = malloc (sizeof(int) * a1.delta[i][j].taille);
				R.delta[i][j].taille = a1.delta[i][j].taille;
				for (k = 0; k < a1.delta[i][j].taille ; k++){
					R.delta[i][j].etat[k]  =  a1.delta[i][j].etat[k];
				}
			}
			else  if ((a2.delta[0][j].taille > 0 || a1.delta[i][j].taille > 0) && verif_etat_tab(i, a1.F, a1.accept_F) == 1) // si etat accepteur,copier et renvoyer vers a2
			{
				R.delta[i][j].taille = a1.delta[i][j].taille + a2.delta[0][j].taille;
				R.delta[i][j].etat = malloc (sizeof(int) * R.delta[i][j].taille);
				for (k = 0; k < a1.delta[i][j].taille; k++)
				{
					R.delta[i][j].etat[k] = a1.delta[i][j].etat[k];
				}
				for (; k < R.delta[i][j].taille; k++)
				{
					R.delta[i][j].etat[k] = a2.delta[0][j].etat[k] + a1.Q - 1;
				}
			}
		}
	}
	// copie de a2
	for (i = 1; i < a2.Q; i++){
		for (j = 0; j < szASCII; j++){
			if (a2.delta[i][j].taille > 0){
				R.delta[i + a1.Q - 1][j].etat = malloc (sizeof(int) * a2.delta[i][j].taille);
				R.delta[i + a1.Q - 1][j].taille = a2.delta[i][j].taille;

				for (k = 0; k < a2.delta[i][j].taille ; k++)
				{
					R.delta[i + a1.Q - 1][j].etat[k]  =  a2.delta[i][j].etat[k] + a1.Q - 1;
				}
			}
		}

	}
	return R;
}


int taille_transitions_kleene(int *etat_dispo, int n, int *nouveau_etats, int m) {
	int resultat = 0;
	int i;
	for (i = 0; i < m; i++) {
		if (!verif_etat_tab(nouveau_etats[i], etat_dispo, n)) {
			resultat++;
		}
	}
	return resultat;
}

struct automate_fini_non_deterministe fermeture_iterative_de_kleene(struct automate_fini_non_deterministe a) {
	// la fermeture_iterative_de_kleene renvoie le meme automate avec un etat final en plus ( le 0 s'il ne l'est pas deja ) et des transitions en plus
	// les etapes pour l"etoile sont :
	// 1 : la taille des etats accepteurs est incrementés de 1 si letat initial n'est pas accepteur dans l'automate de depart
	// 2 : on alloue un nouveau automate
	// 3 : on ajoute l'etat initial en tant qu'etat accepteur s'il ne l'est pas
	// 4 : on fait une boucle sur les etats et pour chaque transition
	// si l'etat n'est pas accepteur on le recopie tel qu'il est
	// sinon on le recopie et on lui ajoute les transitions de l'etat initial
	int i, j, k, w;
	int t = a.accept_F;
	int verif = verif_etat_tab(0, a.F, a.accept_F);
	if (!verif) {//si l'etat initial n'est pas accepteur on incremente le nb d'etats accepteurs
		t++;
	}
	struct automate_fini_non_deterministe R = alouer_AFN(a.Q, t);
	R.s = 0; 
	R.Q = a.Q;
	for (i = 0; i < a.accept_F; i++) {
		R.F[i] = a.F[i];
	}

	if (!verif) {
		R.F[t - 1] = 0;
	}
	
	for (i = 0; i < R.Q; i++) {
		if (!verif_etat_tab(i, a.F, a.accept_F)) { // si l'etat n'est pas accepteur on le recopie
			for (j = 0; j < szASCII; j++) {
				R.delta[i][j].taille = a.delta[i][j].taille;
				R.delta[i][j].etat = malloc(sizeof(int) * R.delta[i][j].taille);
				for (k = 0; k < a.delta[i][j].taille; k++) {
					R.delta[i][j].etat[k] = a.delta[i][j].etat[k];
				}
			}
		}
		else { // si l'etat est accepteur on le recopie ensuite on ajoute les transitions de l'etat initial 
			for (j = 0; j < szASCII; j++) {
				R.delta[i][j].taille = taille_transitions_kleene(R.delta[i][j].etat, R.delta[i][j].taille, a.delta[0][j].etat, a.delta[0][j].taille);
				R.delta[i][j].etat = malloc(sizeof(int) * R.delta[i][j].taille);
				for (k =  0; k < a.delta[i][j].taille; k++) {
					R.delta[i][j].etat[k] = a.delta[i][j].etat[k];
				}
				for (w = 0; w < a.delta[0][j].taille; w++) {
					if (!verif_etat_tab(a.delta[0][j].etat[w], R.delta[i][j].etat, R.delta[i][j].taille)) {
						R.delta[i][j].etat[k++] = a.delta[0][j].etat[w];
					}
				}
			}
		}

	}
	return R;
}
/* TRAITEMENT AUTOMATE DETERMINISTE */
//TODO VERIF!
int execution_mot (struct automate_fini_deterministe a , char* mot )
{
	int i = 0;
	int etat_courant = a.s; // commencer par l'etat initial
	int etat_suivant = -1;
	int test = 0;
	int taille_mot = strlen(mot);

	while  (i < taille_mot && test == 0){
		etat_suivant =  a.delta [etat_courant][(int)mot[i]]; // avancer vers l'etat suivant si le mot peut etre lu sinon ca sera -1
		if (i != taille_mot - 1 && etat_courant == -1 ){ //i!=  vu que le dernier etat contient -1
			test = 1;
		}
		etat_courant = etat_suivant; // mise a jour de l'etat courant
		i++;
	}
	if (i == taille_mot){
		if (verif_etat_tab(etat_courant,a.F,a.accept_F) == 1){
			//mot accepte
			printf("mot accepté\n");
			return 1;
		}
		else{
			printf("mot rejetté\n");
			return 0;
		}
	}
	else{
		printf("mot rejetté\n");
		return 0;
	}
}


unsigned meme_etats(struct transitions etat1, struct transitions etat2) {

	int i;
	if (etat1.taille != etat2.taille) {
		return 0;
	}
	else {
		for (i = 0; i < etat1.taille; i++) {
			if (!verif_etat_tab(etat1.etat[i], etat2.etat, etat1.taille)) {
				return 0;
			}
		}
	}

	return 1;
}

int trouver_numero_etat(struct transitions *tableau_index, int n, struct transitions etat, int i) {

	for (int i = 0; i < n ; i++)
	{
		if (meme_etats(tableau_index[i], etat)) {
			return i;
		}
	}
	return -1; // si changement ajout parametre avec numero etat mort
}

unsigned existe(struct transitions aux, struct transitions *tableau_index, int n) {

	int i;
	for (i = 0; i < n; i++) {
		if (meme_etats(aux, tableau_index[i])) {
			return 1;
		}
	}
	return 0;
}

unsigned existe_entier_tableau(int x, int *tab, int n) {
	for (int i = 0; i < n; i++)
	{
		if (x == tab[i])
			return 1;
	}

	return 0;
}

struct transitions combiner(struct transitions tableau_index, struct automate_fini_non_deterministe a, int ch) {

	int i, j;
	int k = 0;
	struct transitions resultat;
	resultat.taille = 0;

	for (i = 0; i < tableau_index.taille; i++) {
		resultat.taille += a.delta[tableau_index.etat[i]][ch].taille;
	}

	resultat.etat = malloc(sizeof(int) * resultat.taille);
	for (i = 0; i < tableau_index.taille; i++) {
		for (j = 0; j < a.delta[tableau_index.etat[i]][ch].taille; j++) {
			if (!existe_entier_tableau(a.delta[tableau_index.etat[i]][ch].etat[j], resultat.etat, k)) { // doublons
				resultat.etat[k++] = a.delta[tableau_index.etat[i]][ch].etat[j];
			}
		}
	}
	resultat.taille = k;

	return resultat;
}

unsigned verif_final(struct transitions etat_compose, int *F, int accept_F) {
	int i;
	for (i = 0; i < etat_compose.taille; i++) {
		if (existe_entier_tableau(etat_compose.etat[i], F, accept_F)) {
			return 1;
		}
	}
	return 0;
}


 void debug_etat(struct transitions l) {
 	printf("{" );
 	for (int i = 0; i < l.taille; i++) {
 		printf("%d ", l.etat[i]);
 	}
 	printf("} \n" );
}
struct automate_fini_deterministe determinisation_automate(struct automate_fini_non_deterministe a) {
	

	struct automate_fini_deterministe langage;
	struct transitions **matrice_determinisation;
	struct transitions *tableau_index;
	int *etat_finaux;
	int taille_etat_finaux=0;
	//int max = 100;
	int index = 0;
	int index_fini = 0;
	int i, j;
	//construction tableau determinisation
	matrice_determinisation = malloc(sizeof(struct transitions*) * 100);
	tableau_index = malloc(sizeof(struct transitions) * 100);
	tableau_index[0].taille = 1;
	tableau_index[0].etat = malloc(sizeof(int) * 1);
	tableau_index[0].etat[0] = 0;
	index = 1;

	while (index > index_fini) {

		matrice_determinisation[index_fini] = malloc(sizeof(struct transitions) * szASCII);
		for (i = 0; i < szASCII; i++) {

			struct transitions aux = combiner(tableau_index[index_fini], a, i);
			
			matrice_determinisation[index_fini][i] = aux;

			if (!existe(aux, tableau_index, index)) {
				
				tableau_index[index] = aux;
				//printf("etat %d \n ", index);
				 //debug_etat(tableau_index[index]);
				index++;
				 //debug_etat(tableau_index[index]);
				//debug_etat(aux); printf(" %d\n",aux.taille);
			}

		}
		index_fini++;

	}
	//debug affichage
	 
	 printf("affichage de la matrice\n");
	for (int i = 0; i < index; i++) {
		printf("etat %d *", i);
		debug_etat(tableau_index[i]);
		printf("\n");
	 	for (int j = 0; j < szASCII; j++) {
	 		printf("par '%c'  ", j);
			debug_etat(matrice_determinisation[i][j]);
		}
	 }
	 printf("fin affichage de la matrice %d\n", index);

	//numeroation des etats
	printf("%d\n",index );
	int **matrice_deterministe = malloc(sizeof(int*) * (index));
	for (i = 0; i < index ; i++) {
		matrice_deterministe[i] = malloc(sizeof(int) * szASCII);
		for (j = 0; j < szASCII; j++) {

			matrice_deterministe[i][j] = trouver_numero_etat(tableau_index, index , matrice_determinisation[i][j], i);
			
		}
	}

	
	//init etats finaux
	etat_finaux = malloc(sizeof(int) * index );
	for ( i = 0 ; i < index ; i++) {
		if (verif_final(tableau_index[i], a.F, a.accept_F)) {
			etat_finaux[taille_etat_finaux++] = i;

		}
	}

	langage = alouer_AFD(index , taille_etat_finaux);
	langage.s = 0;
	langage.delta = matrice_deterministe;

	langage.F = etat_finaux;

	
	
	return langage;
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

void affiche_automate_fini_deterministe(struct automate_fini_deterministe a) {
	int i, j;
	printf("L'automate deterministe a %d etats\n", a.Q);
	printf("L'etat initial est %d\n", a.s);
	printf("Les etats finaux sont { " );
	for (i = 0; i < a.accept_F; i++) {
		printf(" %d " , a.F[i]);
	}
	printf("}\n ");
	printf("Les etats transitions sont :\n " );
	for (i = 0; i < a.Q; i++) {
		
		for (j = 0; j < szASCII; j++) {
			printf("\t %d '%c' %d\n ", i, j, a.delta[i][j]);


		}
	}
}


int main (int argc, char const *argv[])
{

struct automate_fini_non_deterministe  a,x,y ;
struct automate_fini_deterministe z;
/*x = alouer_AFN (3, 3);
	x.s = 0;
	x.delta[0]['a'].taille = 1;
	x.delta[0]['b'].taille = 1;
	x.delta[0]['a'].etat =  malloc (sizeof(int) * x.delta[0]['a'].taille);
	x.delta[0]['b'].etat =  malloc (sizeof(int) * x.delta[0]['b'].taille);
	x.delta[0]['a'].etat[0] = 1;
	x.delta[0]['b'].etat[0] = 2;
	x.F[0] = 1;
	x.F[1] = 2;
	x.F[2] = 0;*/

a = alouer_AFN (2, 1);
	a.s = 0;
	a.delta[0]['a'].taille = 2;
	a.delta[0]['a'].etat =  malloc (sizeof(int) * a.delta[0]['a'].taille);
	//a.delta[0]['b'].etat =  malloc (sizeof(int) * a.delta[0]['b'].taille);
	//a.delta[1]['a'].etat =  malloc (sizeof(int) * a.delta[1]['a'].taille);
	//a.delta[1]['b'].etat =  malloc (sizeof(int) * a.delta[1]['b'].taille);
	a.delta[0]['a'].etat[0] = 1;
	a.delta[0]['a'].etat[1] = 0;
	a.F[0] = 1;
	
	
//a= mot_seul_caractere('a');
//x= mot_seul_caractere('b');
//y= reunion_deux_langages(a,x);
z= determinisation_automate(a);
//affiche_automate_fini_deterministe(z);

return 0;
}
