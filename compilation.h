struct transitions{
	int taille;
	int* etat;
};

struct automate_fini_non_deterministe{
	int Q;  // nombre d'etats
	int s;	// etat initial
	int* F; // etats accepteurs
	int accept_F; // taille F
	struct transitions** delta; //liste transitions 
};


