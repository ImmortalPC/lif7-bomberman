#include "moteur_sdl.h"
#include "debug.h"

moteur_sdl* moteur_sdl::c_Instance = 0;
SDL_Color moteur_sdl::c_mainColor = {255,255,255,0/*unused*/};
const SDL_Color moteur_sdl::couleurBlanche = {255,255,255,0/*unused*/};
const SDL_Color moteur_sdl::couleurNoir = {0,0,0,0/*unused*/};
const SDL_Color moteur_sdl::couleurRouge = {255, 0, 0, 0/*unused*/};
const SDL_Color moteur_sdl::couleurOrange = {255, 184, 0, 0/*unused*/};

/***************************************************************************//*!
* @fn moteur_sdl::moteur_sdl()
* @brief Initialise la class moteur_sdl
*
* Initialise l'�cran sdl et pr�pare le "terrain" pour les traitements futurs
*/
moteur_sdl::moteur_sdl()
{
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 ) // D�marrage de la SDL. Si erreur alors...
		stdErrorE("Erreur d'initialisation de la SDL : %s", SDL_GetError()); // Ecriture de l'erreur

	if(TTF_Init() == -1)
		stdErrorE("Erreur d'initialisation de TTF_Init : %s", TTF_GetError());

	c_ecranGeneral = SDL_SetVideoMode(1024, 768, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if( !c_ecranGeneral )
		stdErrorE("Erreur lors de la cr�ation de l'�cran : %s", SDL_GetError());

	SDL_FillRect(c_ecranGeneral, NULL, SDL_MapRGB(c_ecranGeneral->format, 255, 255, 255));/* Efface l'�cran */

	//SDL_ShowCursor(SDL_DISABLE);// Masque le curseur de la souris

	SDL_WM_SetCaption("Bomberman", NULL);// Titre de la fen�tre

	if( !(c_policeTitre = TTF_OpenFont("Chicken Butt.ttf", 75)) )
		stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());
	TTF_SetFontStyle(c_policeTitre, TTF_STYLE_UNDERLINE);// On souligne le titre

	if( !(c_policeGeneral = TTF_OpenFont("Chicken Butt.ttf", 60)) )
		stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

	/***************************************************************************
	* Cr�ation du fond d'�cran
	*/
	SDL_Rect pos;
	pos.x=0;
	pos.y=0;

	// On charge l'image de background
	c_background = chargerImage("images/background.png");
	// Cr�ation d'une surface vide qui a la taille du background
	c_backgroundLayer = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, c_background->w, c_background->h, 32, 0, 0, 0, 0);
	// On copie le background sur la nouvelle surface
	SDL_BlitSurface(c_background, NULL, c_backgroundLayer, &pos);
	// Cr�ation d'un calque noir transparent
	SDL_Surface* layer = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, c_background->w, c_background->h, 32, 0, 0, 0, 0);
	// Application de la transparence sur la surface layer
	SDL_SetAlpha(layer, SDL_SRCALPHA, 100 );
	// On copie le layer sur la surface c_backgroundLayer
	SDL_BlitSurface(layer, NULL, c_backgroundLayer, &pos);
	SDL_FreeSurface(layer);

	// On charge le titre
	SDL_Surface* c_backgroundTitle = chargerImage("images/background_title.png");// On charge le titre
	// On blit le titre sur le background g�n�ral
	SDL_BlitSurface(c_backgroundTitle, NULL, c_background, &pos);
	SDL_FreeSurface(c_backgroundTitle);

	SDL_EnableUNICODE(1);// On ne veut pas un clavier QWERTY

	SDL_EnableKeyRepeat(0, 0);// Pas de r�p�tition des touches

	/***************************************************************************
	* On charge ici le d�cor
	*/
	// Initialisation du d�cor
	for( unsigned int i=0; i<__nombre_de_decors__; i++ )
		c_Decor[i] = 0;

	//murs
	c_Decor[vide] = chargerImage("images/vide.png");
	c_Decor[mur_destructible] = chargerImage("images/mur_destructible.png");
	c_Decor[mur_indestructible] = chargerImage("images/mur_indestructible.png");
	//joueur1
	c_Decor[joueur1_haut] = chargerImage("images/bomberman1_haut_Sprite.png");
	c_Decor[joueur1_bas] = chargerImage("images/bomberman1_bas_Sprite.png");
	c_Decor[joueur1_gauche] = chargerImage("images/bomberman1_gauche_Sprite.png");
	c_Decor[joueur1_droite] = chargerImage("images/bomberman1_droite_Sprite.png");
	//joueur2
	c_Decor[joueur2_haut] = chargerImage("images/bomberman2_haut_Sprite.png");
	c_Decor[joueur2_bas] = chargerImage("images/bomberman2_bas_Sprite.png");
	c_Decor[joueur2_gauche] = chargerImage("images/bomberman2_gauche_Sprite.png");
	c_Decor[joueur2_droite] = chargerImage("images/bomberman2_droite_Sprite.png");
	//joueur3
	c_Decor[joueur3_haut] = chargerImage("images/bomberman3_haut.png");
	c_Decor[joueur3_bas] = chargerImage("images/bomberman3_bas.png");
	c_Decor[joueur3_gauche] = chargerImage("images/bomberman3_gauche.png");
	c_Decor[joueur3_droite] = chargerImage("images/bomberman3_droite.png");
	//joueur4
	c_Decor[joueur4_haut] = chargerImage("images/bomberman4_haut.png");
	c_Decor[joueur4_bas] = chargerImage("images/bomberman4_bas.png");
	c_Decor[joueur4_gauche] = chargerImage("images/bomberman4_gauche.png");
	c_Decor[joueur4_droite] = chargerImage("images/bomberman4_droite.png");
	//armes
	c_Decor[flamme_origine] = chargerImage("images/flamme_origine.png");
	c_Decor[flamme_vertical] = chargerImage("images/milieu_flamme_verticale.png");
	c_Decor[flamme_horizontal] = chargerImage("images/milieu_flamme_horizontale.png");
	c_Decor[flamme_pointe_haut] = chargerImage("images/flamme_haut.png");
	c_Decor[bout_flamme_bas] = chargerImage("images/flamme_bas.png");
	c_Decor[bout_flamme_gauche] = chargerImage("images/flamme_gauche.png");
	c_Decor[bout_flamme_droite] = chargerImage("images/flamme_droite.png");
	c_Decor[bombe] = chargerImage("images/bombeSprite.png");
	// Bonus
	c_Decor[gain_bombe] = chargerImage("images/gain_bombe.png");
	c_Decor[gain_super_bombe] = chargerImage("images/gain_super_bombe.png");
	c_Decor[gain_declancheur] = chargerImage("images/detonateur.png");
	c_Decor[gain_puissance_flamme] = chargerImage("images/gain_puissance_flamme.png");
	c_Decor[gain_super_puissance_flamme] = chargerImage("images/gain_super_puissance_flamme.png");
	c_Decor[gain_pousse_bombe] = chargerImage("images/kick.png");
	c_Decor[gain_vie] = chargerImage("images/gain_vie.gif");
	c_Decor[bonus_teleporteur] = chargerImage("images/teleporteur.png");
	c_Decor[bonus_inversseur_touche] = chargerImage("images/bonus_inversseur_touche.png");
	c_Decor[bonus_force_explosion] = chargerImage("images/bonus_force_explosion.png");


	TTF_Font* tmp_police = TTF_OpenFont("Chicken Butt.ttf", 40);
	if( !tmp_police )
		stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

	// On gen�re le texte pour l'affichage de l'horloge
	for( unsigned int i=0; i<12; i++ )
		c_clock[i] = 0;
	c_clock[0] = ecritTexte("0", tmp_police);
	c_clock[1] = ecritTexte("1", tmp_police);
	c_clock[2] = ecritTexte("2", tmp_police);
	c_clock[3] = ecritTexte("3", tmp_police);
	c_clock[4] = ecritTexte("4", tmp_police);
	c_clock[5] = ecritTexte("5", tmp_police);
	c_clock[6] = ecritTexte("6", tmp_police);
	c_clock[7] = ecritTexte("7", tmp_police);
	c_clock[8] = ecritTexte("8", tmp_police);
	c_clock[9] = ecritTexte("9", tmp_police);
	c_clock[10] = ecritTexte("min", tmp_police);
	c_clock[11] = ecritTexte("secs", tmp_police);
	TTF_CloseFont(tmp_police);


	c_fenetreOuverte = true;

	c_mainColor.r = 255;
	c_mainColor.g = 255;
	c_mainColor.b = 255;
}


/***************************************************************************//*!
* @fn moteur_sdl& moteur_sdl::getInstance()
* @brief Renvoie l'instance de la class. Si pas d'instance, alors cr�ation d'une instance
*/
moteur_sdl& moteur_sdl::getInstance()
{
	if( !c_Instance )
		c_Instance = new moteur_sdl;

	return *c_Instance;
}


/***************************************************************************//*!
* @fn moteur_sdl::~moteur_sdl()
* @brief D�sinitialise la class moteur_sdl
*/
moteur_sdl::~moteur_sdl()
{
	for( unsigned int i=0; i<__nombre_de_decors__; i++ )
		if( c_Decor[i] ){
			SDL_FreeSurface(c_Decor[i]);
			c_Decor[i] = 0;
		}

	for( unsigned int i=0; i<12; i++ )
		if( c_clock[i] ){
			SDL_FreeSurface(c_clock[i]);
			c_clock[i] = 0;
		}

	c_ListSpriteJoueur.clear();

	if( c_background )
		SDL_FreeSurface(c_background);// On lib�re le background

	if( c_backgroundLayer )
		SDL_FreeSurface(c_backgroundLayer);// On lib�re le background

	if( c_policeTitre )
		TTF_CloseFont( c_policeTitre );// Fermeture de la police avant TTF_Quit

	if( c_policeGeneral )
		TTF_CloseFont(c_policeGeneral);// Fermeture de la police avant TTF_Quit

	TTF_Quit();// Arr�t de SDL_ttf (peut �tre avant ou apr�s SDL_Quit, peu importe)
	SDL_Quit();
}


/***************************************************************************//*!
* @fn unsigned int moteur_sdl::menu( const char titre[], const char *choix[], unsigned int nb_choix )
* @brief Afficher l'image correspondante suivant le menu o� on est
* @param[in] titre		Titre de la sous fen�tre
* @param[in] choix		Liste des choix a mettre dans la sous fen�tre. ( Tableau de chaine de caract�res )
* @param[in] nb_choix	Nombre d'�lement dans la liste des choix
* @return Le choix selectionn� (sous forme d'un nombre: le i�me element de la liste a �t� selectionn�)
*
* La sous fen�tre propose de naviguer parmis une liste ( @a choix )<br />
* Puis renvoie le choix (sous forme d'un nombre: i�me element de la liste)<br />
* @code
* // Exemple
* const char* choix[] = {
*	"Choix 1",
*	"Choix 2",
*	"Choix 3",
*	"Choix 4",
*	"Choix 5"
* };
* unsigned int choix_selectionner = menu( "Liste de mes choix", choix, 5 );
* @endcode
*/
unsigned int moteur_sdl::menu( const char titre[], const char *choix[], unsigned int nb_choix )
{
	unsigned int highLight = 1;
	bool continuer = 1;
	SDL_Event event;
	SDL_Rect position;
	SDL_Surface* sfr_titre=0;
	SDL_Surface* sfr_titre_bg=0;
	SDL_Surface** textes={0};
	unsigned int i;
	bool dessiner = 1;

	textes = new SDL_Surface*[nb_choix*2];

	for( i=0; i<nb_choix; i++ )/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
		textes[i] = ecritTexte(choix[i]);

	// Cr�ation du texte HighLight
	for( i=nb_choix; i<nb_choix*2; i++ )/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
		textes[i] = ecritTexte(choix[i-nb_choix], couleurOrange);

	// Cr�ation du titre
	if( titre && strlen(titre) ){
		sfr_titre = ecritTexte(titre, c_policeTitre);
		sfr_titre_bg = ecritTexte(titre, c_policeTitre, couleurNoir);
	}


	while( continuer )
	{
		if( !dessiner )
			SDL_WaitEvent(&event);

		if( !dessiner )
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = nb_choix;// On veux quitter !
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: {
				switch( event.key.keysym.sym )
				{
					case SDLK_UP: { /* Appui sur la touche Echap, on arr�te le programme */
						if( highLight == 1 ){// On bouge notre highLight vers le haut
							highLight = nb_choix;
						}else{
							highLight--;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_DOWN: {// On bouge notre highLight vers le bas
						if( highLight == nb_choix ){
							highLight = 1;
						}else{
							highLight++;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_KP_ENTER:
					case SDLK_RETURN: {// On a valider notre choix par entrer
						continuer = 0;
						break;
					}
					case SDLK_ESCAPE: { /* Appui sur la touche Echap, on arr�te le programme */
						highLight = nb_choix;// On veux quitter !
						continuer = 0;
						break;
					}
					default:
						break;
				}
				break;
			}
			/*******************************************************************
			* Prise en charge de la souris
			*/
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONUP: {
				position.y = Marge_menu+100;
				for( i=0; i<nb_choix; i++ )
				{
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					if( position.x <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
						if( highLight != i+1 ){
							highLight = i+1;
							dessiner = 1;
						}
						if( event.type == SDL_MOUSEBUTTONUP )
							continuer = 0;
						break;
					}
					position.y += textes[i]->h;// Espacement entre les textes ( blit )
				}
				break;
			}
			default:
				break;
		}

		/***********************************************************************
		* Blitage g�n�ral
		* > On blit uniquement s'il y a eu une modification
		*/
		if( dessiner ){
			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

			position.y = Marge_menu;// Point de d�part
			if( sfr_titre ){
				position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
				position.x -= 5;
				position.y -= 5;
				SDL_BlitSurface(sfr_titre_bg, NULL, c_ecranGeneral, &position);// Blit background
				position.x += 5;
				position.y += 5;
				SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit title
			}
			position.y += 100;
			// On blit le texte
			for( i=0; i<nb_choix; i++ )
			{
				position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
				if( i == highLight-1 ){
					SDL_BlitSurface(textes[i+nb_choix], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}else{
					SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}
				position.y += textes[i]->h;// Espacement entre les textes ( blit )
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}
	}

	for( i=0; i<nb_choix*2; i++ )// On lib�re les textes
		SDL_FreeSurface(textes[i]);
	delete[] textes;// On lib�re les textes

	if( sfr_titre )
		SDL_FreeSurface(sfr_titre);

	if( sfr_titre_bg )
		SDL_FreeSurface(sfr_titre_bg);

	return highLight;
}


/***************************************************************************//*!
* @fn void moteur_sdl::afficherConfigurationClavier( unsigned char joueur )
* @brief Menu permettant d'afficher et de modifier la configuration du clavier d'un joueur
* @param[in] joueur		Le num�ro du joueur ( de 1 � ... )
*/
void moteur_sdl::afficherConfigurationClavier( unsigned char joueur )
{
	bool continuer = 1;
	SDL_Event event;
	unsigned int highLight = 1;
	bool dessiner = 1;
	int i;
	SDL_Rect position, position_droite; /* position de blittage du texte */
	// On r�cup�re la config du joueur
	clavier* cl = options::getInstance()->clavierJoueur( joueur-1 );

	// POUR def une touche :
	// cl->defTouche((clavier::t_touche)(clavier::haut+highLight-1), key);
	// POUR r�cup une touche dans la configuration
	// cl->touche(  clavier::haut );

	SDL_Surface* background = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, 1024, 768, 32, 0, 0, 0, 0);
	SDL_SetAlpha(background, SDL_SRCALPHA, 100 );

	// Cr�ation titre
	SDL_Surface* sfr_titre;
	SDL_Surface* sfr_titre_bg;
	char tempTexte[] = "Configuration clavier du joueur 0";// Ne pas oublier le '\0' !
	tempTexte[strlen(tempTexte)-1] += joueur;
	sfr_titre = ecritTexte(tempTexte, c_policeTitre);
	sfr_titre_bg = ecritTexte(tempTexte, c_policeTitre, couleurNoir);


	// Cr�ation du texte de retour
	SDL_Surface* sfr_retour[2];
	sfr_retour[0] = ecritTexte("Retour");
	sfr_retour[1] = ecritTexte("Retour", couleurOrange);

	// Ecriture des textes
	SDL_Surface* sfr_msg; /* pour dialoguer avec l'utilisateur */
	SDL_Surface* sfr_HAUT[2];
	SDL_Surface* sfr_HAUT_touche[2];
	SDL_Surface* sfr_BAS[2];
	SDL_Surface* sfr_BAS_touche[2];
	SDL_Surface* sfr_DROITE[2];
	SDL_Surface* sfr_DROITE_touche[2];
	SDL_Surface* sfr_GAUCHE[2];
	SDL_Surface* sfr_GAUCHE_touche[2];
	SDL_Surface* sfr_LANCER_BOMBE[2];
	SDL_Surface* sfr_LANCER_BOMBE_touche[2];
	SDL_Surface* sfr_DETONATEUR[2];
	SDL_Surface* sfr_DETONATEUR_touche[2];

	TTF_Font* police = TTF_OpenFont("Chicken Butt.ttf", 40);
	if( !police )
		stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

	sfr_HAUT[0] = ecritTexte("Haut", police);
	sfr_HAUT[1] = ecritTexte("Haut", police, couleurOrange);
	sfr_HAUT_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), police);
	sfr_HAUT_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), police, couleurOrange);
	sfr_BAS[0] = ecritTexte("Bas", police);
	sfr_BAS[1] = ecritTexte("Bas", police, couleurOrange);
	sfr_BAS_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ), police);
	sfr_BAS_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ), police, couleurOrange);
	sfr_DROITE[0] = ecritTexte("Droite", police);
	sfr_DROITE[1] = ecritTexte("Droite", police, couleurOrange);
	sfr_DROITE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ), police);
	sfr_DROITE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ), police, couleurOrange);
	sfr_GAUCHE[0] = ecritTexte("Gauche", police);
	sfr_GAUCHE[1] = ecritTexte("Gauche", police, couleurOrange);
	sfr_GAUCHE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ), police);
	sfr_GAUCHE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ), police, couleurOrange);
	sfr_LANCER_BOMBE[0] = ecritTexte("Lancer bombe", police);
	sfr_LANCER_BOMBE[1] = ecritTexte("Lancer bombe", police, couleurOrange);
	sfr_LANCER_BOMBE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ), police);
	sfr_LANCER_BOMBE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ), police, couleurOrange);
	sfr_DETONATEUR[0] = ecritTexte("D�tonateur", police);
	sfr_DETONATEUR[1] = ecritTexte("D�tonateur", police, couleurOrange);
	sfr_DETONATEUR_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ), police);
	sfr_DETONATEUR_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ), police, couleurOrange);

	TTF_CloseFont(police);

	do{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: { /*	si on clique sur la croix rouge de la fen�tre -> on sort et highLight */
				highLight = 7; /*	passe � 7 (valeur de sortie) */
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: { /* si touche appuy�e */
				switch( traductionClavier(&event.key) ) /* on traduit la touche */
				{
					case SDLK_ESCAPE: { /*	si on appuit sur la touche echap */
						highLight = 7; /*	passe � 7 (valeur de sortie) */
						continuer = 0;
						break;
					}
					case SDLK_UP: { /* fl�che du haut */
						if( highLight == 1 ){ /* si on est tout en haut, alors on va tout en bas */
							highLight = 7;
						}else{ /* sinon highLight --*/
							highLight--;
						}
						dessiner = 1; /* mouvement donc on redessine */
						break;
					}
					case SDLK_DOWN: { /* fl�che bas */
						if( highLight == 7 ){ /* si on est tout en bas, alors on va tout en haut */
							highLight = 1;
						}else{ /* sinon highLight ++ */
							highLight++;
						}
						dessiner = 1; /* mouvement donc on redessine */
						break;
					}
					case SDLK_KP_ENTER:
					case SDLK_RETURN: { /* touche entrer appuy�e */
						if( highLight == 7 ){ /* entrer + derni�re ligne -> sortie */
							highLight = 7;
							continuer = 0;
							break;
						}

						switch( highLight )
						{
							/* liens modifiables : de 1 � 6 */
							case 1:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour HAUT", couleurRouge, 50);
								break;
							case 2:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour BAS", couleurRouge, 50);
								break;
							case 3:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour DROITE", couleurRouge, 50);
								break;
							case 4:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour GAUCHE", couleurRouge, 50);
								break;
							case 5:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour la BOMBE", couleurRouge, 50);
								break;
							case 6:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour le DETONATEUR", couleurRouge, 50);
								break;
						}

						position.x = 0;
						position.y = 0;
						SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);/* blittage du background */
						position.x = (c_ecranGeneral->w-sfr_msg->w)/2;
						position.y = (c_ecranGeneral->h-sfr_msg->h)/2;
						SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position); /* blittage du texte */
						SDL_Flip(c_ecranGeneral); /* raffraichissement */
						do{
							SDL_WaitEvent(&event); /* en attente d'un �v�nement */
							if( event.type == SDL_QUIT ){ /* si croix rouge -> on sort */
								continuer = 0;
								highLight = 7;
								c_fenetreOuverte = false;
								break;
							}
							/* sinon lire la touche et l'enregistrer */
							if( event.type == SDL_KEYDOWN && traductionClavier(&event.key) != SDLK_ESCAPE ){
								/* red�finition de la touche : */
								cl->defTouche((clavier::t_touche)(clavier::haut+highLight-1), traductionClavier(&event.key));
								switch( highLight )
								{
									/* liens modifiables : de 1 � 6 */
									case 1:
										SDL_FreeSurface(sfr_HAUT_touche[0]);
										SDL_FreeSurface(sfr_HAUT_touche[1]);
										sfr_HAUT_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
										sfr_HAUT_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
										break;
									case 2:
										SDL_FreeSurface(sfr_BAS_touche[0]);
										SDL_FreeSurface(sfr_BAS_touche[1]);
										sfr_BAS_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ));
										sfr_BAS_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ), couleurOrange);
										break;
									case 3:
										SDL_FreeSurface(sfr_DROITE_touche[0]);
										SDL_FreeSurface(sfr_DROITE_touche[1]);
										sfr_DROITE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ));
										sfr_DROITE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ), couleurOrange);
										break;
									case 4:
										SDL_FreeSurface(sfr_GAUCHE_touche[0]);
										SDL_FreeSurface(sfr_GAUCHE_touche[1]);
										sfr_GAUCHE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ));
										sfr_GAUCHE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ), couleurOrange);
										break;
									case 5:
										SDL_FreeSurface(sfr_LANCER_BOMBE_touche[0]);
										SDL_FreeSurface(sfr_LANCER_BOMBE_touche[1]);
										sfr_LANCER_BOMBE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ));
										sfr_LANCER_BOMBE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ), couleurOrange);
										break;
									case 6:
										SDL_FreeSurface(sfr_DETONATEUR_touche[0]);
										SDL_FreeSurface(sfr_DETONATEUR_touche[1]);
										sfr_DETONATEUR_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ));
										sfr_DETONATEUR_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ), couleurOrange);
										break;
								}
							}
						}while( event.type != SDL_KEYDOWN );
						SDL_FreeSurface(sfr_msg); /* lib�ration de sfr_msg */
						dessiner = 1;
						break;
					}

					default:// On eject les autres cas
						break;
				} /* fin switch */
			} /* fin case return */
		} /* fin switch */

		if( dessiner )
		{
			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position); /* blittage du background */
			// Filtre noir en background
			SDL_BlitSurface(background, NULL, c_ecranGeneral, &position);


			position.y = Marge_menu;
			position.x = (c_ecranGeneral->w-sfr_titre->w)/2; /* centrage */
			position.x -= 5;
			position.y -= 5;
			SDL_BlitSurface(sfr_titre_bg, NULL, c_ecranGeneral, &position); /* blittage du titre */
			position.x += 5;
			position.y += 5;
			SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position); /* blittage du titre */


			position.x = 100;
			position_droite.x = c_ecranGeneral->w-250;

			position.y += 100;
			position_droite.y = position.y;
			if( highLight == 1 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_HAUT[1], NULL, c_ecranGeneral, &position);
				SDL_BlitSurface(sfr_HAUT_touche[1], NULL, c_ecranGeneral, &position_droite); /* Blit du texte par-dessus */
			}else{
				SDL_BlitSurface(sfr_HAUT[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_HAUT_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += sfr_HAUT[0]->h;
			position_droite.y = position.y;
			if( highLight == 2 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_BAS[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_BAS_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_BAS[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_BAS_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += sfr_BAS[0]->h;
			position_droite.y = position.y;
			if( highLight == 3 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_DROITE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DROITE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_DROITE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DROITE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += sfr_DROITE[1]->h;
			position_droite.y = position.y;
			if( highLight == 4 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_GAUCHE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_GAUCHE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_GAUCHE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_GAUCHE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += sfr_GAUCHE[0]->h;
			position_droite.y = position.y;
			if( highLight == 5 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_LANCER_BOMBE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_LANCER_BOMBE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_LANCER_BOMBE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_LANCER_BOMBE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += sfr_LANCER_BOMBE[0]->h;
			position_droite.y = position.y;
			if( highLight == 6 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_DETONATEUR[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DETONATEUR_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_DETONATEUR[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DETONATEUR_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.x = (c_ecranGeneral->w-sfr_retour[0]->w)/2;
			position.y += sfr_DETONATEUR[0]->h;
			if( highLight == 7 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_retour[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
			}else{
				SDL_BlitSurface(sfr_retour[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}

	}while( continuer );/* fin while */

	// Free
	for( i=0; i<2; i++ )
	{
		SDL_FreeSurface(sfr_retour[i]);
		SDL_FreeSurface(sfr_HAUT[i]);
		SDL_FreeSurface(sfr_HAUT_touche[i]);
		SDL_FreeSurface(sfr_BAS[i]);
		SDL_FreeSurface(sfr_BAS_touche[i]);
		SDL_FreeSurface(sfr_DROITE[i]);
		SDL_FreeSurface(sfr_DROITE_touche[i]);
		SDL_FreeSurface(sfr_GAUCHE[i]);
		SDL_FreeSurface(sfr_GAUCHE_touche[i]);
		SDL_FreeSurface(sfr_LANCER_BOMBE[i]);
		SDL_FreeSurface(sfr_LANCER_BOMBE_touche[i]);
		SDL_FreeSurface(sfr_DETONATEUR[i]);
		SDL_FreeSurface(sfr_DETONATEUR_touche[i]);
	}

	SDL_FreeSurface(sfr_titre);
	SDL_FreeSurface(sfr_titre_bg);
	SDL_FreeSurface(background);
}


/***************************************************************************//*!
* @fn int moteur_sdl::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour )
* @brief Cr�er un menu pour r�cup�rer un nombre entr� par un utilisateur
* @param[in] titre			 Le titre du menu
* @param[in] valeurParDefaut La valeur par d�faut
* @param[in] valeurMin		 La valeur minimum
* @param[in] valeurMax		 La valeur maximum
* @param[out] valeurRetour	 Dans cette variable sera stock�, le nombre obtenu a la fin de la fonction
* @return
*	- 2 : Nombre valid� et acc�pt�
*	- 3 : Action annul�e
*/
int moteur_sdl::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour )
{
	unsigned int highLight = 2;
	bool continuer = 1;
	SDL_Event event;
	SDL_Rect position;
	SDL_Surface* sfr_titre = 0;
	SDL_Surface* sfr_titre_bg = 0;
	SDL_Surface* sfr_msg = 0;
	SDL_Surface* textes[3*2]={0};
	unsigned int i;
	bool dessiner = 1;
	bool dessinerNombre = 0;

	char valeurRetourTexte[/*Taille max d'un unsigned int*/getSizeOfNumber<-1>::value+1/*Pour le signe*/+4*2/*Pour les <--*/+1/*Le \0*/];// <- Calcule :: lenght(2^(sizeof(int)*8))+4*2

	if( valeurMin > valeurMax )
		stdError("Donn�es incorrect ! valeurMin(%u) > valeurMax(%u)", valeurMin, valeurMax);

	if( valeurMin > valeurParDefaut || valeurParDefaut > valeurMax ){
		stdError("Valeur incorrect ! valeurMin(%d) <= valeurParDefaut(%d) && valeurParDefaut(%d) <= valeurMax(%d)", valeurMin, valeurParDefaut, valeurParDefaut, valeurMax);
		valeurParDefaut = valeurMin+(valeurMax - valeurMin)/2;
	}

	*valeurRetour = valeurParDefaut;
	sprintf(valeurRetourTexte, "<-- %d -->", *valeurRetour);

	/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
	textes[0] = ecritTexte(valeurRetourTexte);
	textes[1] = ecritTexte("Ok");
	textes[2] = ecritTexte("Retour");

	// Cr�ation du texte HighLight
	textes[3] = ecritTexte(valeurRetourTexte, couleurOrange);
	textes[4] = ecritTexte("Ok", couleurOrange);
	textes[5] = ecritTexte("Retour", couleurOrange);

	// Cr�ation du titre
	//if( !strlen(titre) )
	//	stdErrorE("Veuillez mettre un titre NON vide ! moteur_sdl::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour )");
	sfr_titre = ecritTexte(titre, c_policeTitre);
	sfr_titre_bg = ecritTexte(titre, c_policeTitre, couleurNoir);

	while( continuer && c_fenetreOuverte )
	{
		if( !dessiner )
			SDL_WaitEvent(&event);

		if( !dessiner )
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = 3;// On veut quitter !
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: {
				switch( event.key.keysym.sym )
				{
					case SDLK_UP: { /* Appui sur la touche Echap, on arr�te le programme */
						if( highLight == 1 ){// On bouge notre highLight vers le haut
							highLight = 3;
						}else{
							highLight--;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_DOWN: {// On bouge notre highLight vers le bas
						if( highLight == 3 ){
							highLight = 1;
						}else{
							highLight++;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_RIGHT: {
						if( highLight != 1 )// Pour �tre certain que l'on ne change les nombre que si l'on est sur le 1er element
							break;

						if( *valeurRetour+1 > valeurMax )
							*valeurRetour = valeurMin;
						else
							(*valeurRetour)++;

						// On redessine le nombre
						dessinerNombre = 1;
						dessiner = 1;
						break;
					}
					case SDLK_LEFT: {
						if( highLight != 1 )// Pour �tre certain que l'on ne change les nombre que si l'on est sur le 1er element
							break;

						if( *valeurRetour-1 < valeurMin )
							*valeurRetour = valeurMax;
						else
							(*valeurRetour)--;

						// On redessine le nombre
						dessinerNombre = 1;
						dessiner = 1;
						break;
					}
					case SDLK_KP_ENTER:
					case SDLK_RETURN: {// On a valid� notre choix par entrer
						if( highLight == 1 ){// On a selectionn� un lien de modification de texte
							dessiner = 1;
							sfr_msg = ecritTexte("Entrez le nombre manuellement", couleurRouge);
							do{
								SDL_WaitEvent(&event);
								if( event.type == SDL_QUIT ){
									continuer = 0;
									highLight = 3;
									c_fenetreOuverte = false;
								}

								if( event.type == SDL_KEYDOWN )
								switch( traductionClavier(&event.key) )
								{
									case SDLK_RIGHT:
									case SDLK_UP: {
										if( *valeurRetour+1 > valeurMax )
											*valeurRetour = valeurMin;
										else
											(*valeurRetour)++;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_LEFT:
									case SDLK_DOWN: {
										if( *valeurRetour-1 < valeurMin )
											*valeurRetour = valeurMax;
										else
											(*valeurRetour)--;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_BACKSPACE: {
										*valeurRetour /= 10;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_ESCAPE: {
										*valeurRetour = valeurParDefaut;
										continuer = 0;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_KP_ENTER:
									case SDLK_RETURN: {
										if( !(valeurMin <= *valeurRetour && *valeurRetour <= valeurMax) ){
											*valeurRetour = valeurMin;
											// On redessine le nombre
											dessinerNombre = 1;
										}else{
											highLight = 2;
										}
										continuer = 0;
										break;
									}
									case SDLK_PLUS:
									case SDLK_MINUS:
									case SDLK_KP_PLUS:
									case SDLK_KP_MINUS: {
										if( valeurMin <= *valeurRetour*(-1) && *valeurRetour*(-1) <= valeurMax )
											*valeurRetour *= -1;
										dessiner = 1;// On redessine
										break;
									}
									default: {
										// Ajout de chiffre � la main
										if( SDLK_0 <= event.key.keysym.unicode && event.key.keysym.unicode <= SDLK_9 && *valeurRetour*10+(int)(event.key.keysym.unicode-SDLK_0) <= valeurMax )
											*valeurRetour = *valeurRetour*10+(event.key.keysym.unicode-SDLK_0);
										if( SDLK_KP0 <= event.key.keysym.unicode && event.key.keysym.unicode <= SDLK_KP0 && *valeurRetour*10+(int)(event.key.keysym.unicode-SDLK_KP0) <= valeurMax )
											*valeurRetour = *valeurRetour*10+(event.key.keysym.unicode-SDLK_KP0);
										dessiner = 1;// On redessine
										break;
									}
								}

								/***********************************************************************
								* Blitage g�n�ral
								* > On blit uniquement s'il y a eu une modification
								*/
								if( dessiner ){
									sprintf(valeurRetourTexte, "<-- %d -->", *valeurRetour);
									SDL_FreeSurface(textes[0]);
									SDL_FreeSurface(textes[3]);
									textes[0] = ecritTexte(valeurRetourTexte);
									textes[3] = ecritTexte(valeurRetourTexte, couleurRouge);

									position.x = 0;
									position.y = 0;
									SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

									position.y = Marge_menu;
									position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
									SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background

									// On blit le texte
									for( i=0; i<3; i++ )
									{
										position.y += 100;// Espacement entre les textes ( blit )
										position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
										if( i == highLight-1 ){
											SDL_BlitSurface(textes[i+3], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
										}else{
											SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
										}
									}

									SDL_Flip(c_ecranGeneral);
									dessiner = 0;
								}
							}while( continuer );
							SDL_FreeSurface(sfr_msg);
							SDL_FreeSurface(textes[3]);
							textes[3] = ecritTexte(valeurRetourTexte, couleurOrange);
							dessiner = 1;
							continuer = 1;
						}else
							continuer = 0;// On sort de la boucle
						break;
					}
					case SDLK_ESCAPE: { /* Appui sur la touche Echap, on arr�te le programme */
						highLight = 3;// On veut quitter !
						continuer = 0;
						break;
					}
					default:
						break;
				}
				break;
			}
			/*******************************************************************
			* Prise en charge de la souris
			*/
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONUP: {
				position.y = Marge_menu;
				for( i=0; i<3; i++ )
				{
					position.y += 100;// Espacement entre les textes ( blit )
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					if( position.x <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
						if( highLight != i+1 ){
							highLight = i+1;
							dessiner = 1;
						}
						if( event.type == SDL_MOUSEMOTION )
							break;

						if( highLight == 1 ){
							if( position.x <= event.button.x && event.button.x <= position.x+35 && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
								if( *valeurRetour-1 < valeurMin )
									*valeurRetour = valeurMax;
								else
									(*valeurRetour)--;

								// On redessine le nombre
								dessinerNombre = 1;
								dessiner = 1;
								break;
							}
							if( position.x+textes[i]->w-35 <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
								if( *valeurRetour+1 > valeurMax )
									*valeurRetour = valeurMin;
								else
									(*valeurRetour)++;

								// On redessine le nombre
								dessinerNombre = 1;
								dessiner = 1;
								break;
							}
						}else{
							continuer = 0;
						}
						break;
					}
				}
				break;
			}
			default:
				break;
		}

		/***********************************************************************
		* Blitage g�n�ral
		* > On blit uniquement s'il y a eu une modification
		*/
		if( dessiner ){
			if( dessinerNombre ){
				// On redessine le nombre
				sprintf(valeurRetourTexte, "<-- %d -->", *valeurRetour);
				SDL_FreeSurface(textes[0]);
				SDL_FreeSurface(textes[3]);
				textes[0] = ecritTexte(valeurRetourTexte);
				textes[3] = ecritTexte(valeurRetourTexte, couleurOrange);
				dessinerNombre = 0;
			}

			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

			position.y = Marge_menu;// Point de d�part
			position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
			position.x -= 5;
			position.y -= 5;
			SDL_BlitSurface(sfr_titre_bg, NULL, c_ecranGeneral, &position);// Blit background
			position.x += 5;
			position.y += 5;
			SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background

			// On blit le texte
			for( i=0; i<3; i++ )
			{
				position.y += 100;// Espacement entre les textes ( blit )
				position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
				if( i == highLight-1 ){
					SDL_BlitSurface(textes[i+3], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}else{
					SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}
	}

	for( i=0; i<3*2; i++ )// On lib�re les textes
		SDL_FreeSurface(textes[i]);

	SDL_FreeSurface(sfr_titre);
	SDL_FreeSurface(sfr_titre_bg);

	return highLight;
}


/***************************************************************************//*!
* @fn int moteur_sdl::getTexte( const char titre[], char texteRetour[21] )
* @brief Permet d'obtenir du texte
* @param[in] titre				Le titre du menu
* @param[out] texteRetour		Dans cette variable sera stock�, le texte obtenu a la fin de la fonction
* @return
*	- 2 : Texte valid� et acc�pt�
*	- 3 : Action annul�e
*/
int moteur_sdl::getTexte( const char titre[], char texteRetour[21] )
{
	unsigned int highLight = 1;
	bool continuer = 1;
	SDL_Event event;
	SDL_Rect position;
	SDL_Surface* sfr_titre=0;
	SDL_Surface* sfr_titre_bg=0;
	SDL_Surface* sfr_msg = 0;
	SDL_Surface* textes[3*2]={0};
	unsigned int tailleTexteRetour;
	unsigned int i;
	bool dessiner = 1;
	bool dessinerMot = 0;

	tailleTexteRetour = strlen(texteRetour);

	// Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal)
	if( tailleTexteRetour )
		textes[0] = ecritTexte(texteRetour);
	else
		textes[0] = ecritTexte(completerMot(texteRetour, tailleTexteRetour));
	textes[1] = ecritTexte("Ok");
	textes[2] = ecritTexte("Retour");

	// Cr�ation du texte HighLight
	textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
	textes[4] = ecritTexte("Ok", couleurOrange);
	textes[5] = ecritTexte("Retour", couleurOrange);

	// Cr�ation du titre
	sfr_titre = ecritTexte(titre, c_policeTitre);
	sfr_titre_bg = ecritTexte(titre, c_policeTitre, couleurNoir);

	int taille = 70;
	while( sfr_titre->w > c_ecranGeneral->w ){
		SDL_FreeSurface(sfr_titre);

		TTF_Font* police;
		if( !(police = TTF_OpenFont("Chicken Butt.ttf", taille)) )
			stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

		TTF_SetFontStyle(police, TTF_STYLE_UNDERLINE);
		sfr_titre = ecritTexte(titre, police);
		taille-=5;

		TTF_CloseFont(police);
	}
	if( sfr_titre->w != sfr_titre_bg->w )
	{
		SDL_FreeSurface(sfr_titre_bg);

		TTF_Font* police;
		if( !(police = TTF_OpenFont("Chicken Butt.ttf", taille+5)) )
			stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

		TTF_SetFontStyle(police, TTF_STYLE_UNDERLINE);
		sfr_titre_bg = ecritTexte(titre, police, couleurNoir);

		TTF_CloseFont(police);
	}


	while( continuer )
	{
		if( !dessiner )
			SDL_WaitEvent(&event);

		if( !dessiner )
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = 3;// On veut quitter !
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: {
				switch( traductionClavier(&event.key) )
				{
					case SDLK_UP: { /* Appui sur la touche Echap, on arr�te le programme */
						if( highLight == 1 ){// On bouge notre highLight vers le haut
							highLight = 3;
						}else{
							highLight--;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_DOWN: {// On bouge notre highLight vers le bas
						if( highLight == 3 ){
							highLight = 1;
						}else{
							highLight++;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_BACKSPACE: {
						if( highLight != 1 )// Si on est pas sur le texte => on fait rien
							break;

						if( tailleTexteRetour == 0 )
							break;
						tailleTexteRetour--;
						texteRetour[tailleTexteRetour] = '_';
						dessiner = 1;// On redessine
						dessinerMot = 1;
						break;
					}
					case SDLK_KP_ENTER:
					case SDLK_RETURN: {// On a valid� notre choix par entrer
						if( highLight == 1 ){// On a selectionn� un lien de modification de texte
							trimString(texteRetour);
							if( tailleTexteRetour ){
								highLight = 2;
							}else{
								if( sfr_msg )
									SDL_FreeSurface(sfr_msg);
								sfr_msg = ecritTexte("Le texte dois avoir 1 caractere min", couleurRouge);
							}
							dessiner = 1;
						}else{
							if( highLight == 2 ){
								if( tailleTexteRetour ){
									continuer = 0;// On sort de la boucle
								}else{
									if( sfr_msg )
										SDL_FreeSurface(sfr_msg);
									sfr_msg = ecritTexte("Le texte dois avoir 1 caractere min", couleurRouge);
									dessiner = 1;
								}
							}else{
								continuer = 0;// On sort de la boucle
							}
						}
						break;
					}
					case SDLK_ESCAPE: { /* Appui sur la touche Echap, on arr�te le programme */
						highLight = 3;// On veut quitter !
						continuer = 0;
						break;
					}
					default: {
						if( highLight != 1 )// Si on est pas sur le texte => on fait rien
							break;

						//' ' <= key && key <= '~'
						// Ajout de chiffre � la main
						if( SDLK_SPACE <= event.key.keysym.unicode && event.key.keysym.unicode <= 126 && tailleTexteRetour+1 < 21  ){
							texteRetour[tailleTexteRetour] = event.key.keysym.unicode;
							tailleTexteRetour++;
							completerMot(texteRetour, tailleTexteRetour);
						}
					//	if( SDLK_KP0 <= event.key.keysym.unicode && event.key.keysym.unicode <= SDLK_KP0 && *valeurRetour*10+(int)(event.key.keysym.unicode-SDLK_KP0) <= valeurMax )
					//		*valeurRetour = *valeurRetour*10+(event.key.keysym.unicode-SDLK_KP0);
						dessiner = 1;// On redessine
						dessinerMot = 1;
						break;
					}
				}
				break;
			}
			/*******************************************************************
			* Prise en charge de la souris
			*/
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONUP: {
				position.y = Marge_menu;
				for( i=0; i<3; i++ )
				{
					position.y += 100;// Espacement entre les textes ( blit )
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					if( position.x <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
						if( highLight != i+1 ){
							highLight = i+1;
							dessiner = 1;
						}
						if( event.type == SDL_MOUSEMOTION )
							break;

						if( (highLight == 2/*Ok*/ && tailleTexteRetour) || highLight == 3/*Retour*/ )
							continuer = 0;

						if( highLight == 2 && !tailleTexteRetour ){
							if( sfr_msg )
								SDL_FreeSurface(sfr_msg);
							sfr_msg = ecritTexte("Le texte dois avoir 1 caractere min", couleurRouge);
							highLight = 1;
							dessiner = 1;
						}
						break;
					}
				}
				break;
			}
			default:
				break;
		}

		/***********************************************************************
		* Blitage g�n�ral
		* > On blit uniquement s'il y a eu une modification
		*/
		if( dessiner ){
			if( dessinerMot ){
				SDL_FreeSurface(textes[0]);
				SDL_FreeSurface(textes[3]);

				texteRetour[tailleTexteRetour] = 0;
				trimString(texteRetour);
				tailleTexteRetour = strlen(texteRetour);

				if( tailleTexteRetour )
					textes[0] = ecritTexte(texteRetour);
				else
					textes[0] = ecritTexte(completerMot(texteRetour, tailleTexteRetour));
				textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
				dessinerMot = 0;
			}

			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

			position.y = Marge_menu;
			if( sfr_msg ){
				position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
				SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background
			}else{
				position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
				position.x -= 5;
				position.y -= 5;
				SDL_BlitSurface(sfr_titre_bg, NULL, c_ecranGeneral, &position);// Blit background
				position.x += 5;
				position.y += 5;
				SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background
			}
			// On blit le texte
			for( i=0; i<3; i++ )
			{
				position.y += 100;// Espacement entre les textes ( blit )
				if( i == highLight-1 ){
					position.x = (c_ecranGeneral->w-textes[i+3]->w)/2;// Centrage auto des blit ^^
					SDL_BlitSurface(textes[i+3], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}else{
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}
	}

	for( i=0; i<3*2; i++ )// On lib�re les textes
		SDL_FreeSurface(textes[i]);

	SDL_FreeSurface(sfr_titre);
	SDL_FreeSurface(sfr_titre_bg);

	if( sfr_msg )
		SDL_FreeSurface(sfr_msg);

	texteRetour[tailleTexteRetour] = 0;
	trimString(texteRetour);
	tailleTexteRetour = strlen(texteRetour);

	return highLight;
}

/* ********************************************************************* */
/*obtenirPixel : permet de r�cup�rer la couleur d'un pixel
Param�tres d'entr�e/sortie :
SDL_Surface *surface : la surface sur laquelle on va r�cup�rer la couleur d'un pixel
int x : la coordonn�e en x du pixel � r�cup�rer
int y : la coordonn�e en y du pixel � r�cup�rer

Uint32 resultat : la fonction renvoie le pixel aux coordonn�es (x,y) dans la surface
*/
Uint32 obtenirPixel(SDL_Surface *surface, int x, int y)
{
	if( surface->w <= x || surface->h <= y )
		return 0;

	/*nbOctetsParPixel repr�sente le nombre d'octets utilis�s pour stocker un pixel.
	En multipliant ce nombre d'octets par 8 (un octet = 8 bits), on obtient la profondeur de couleur
	de l'image : 8, 16, 24 ou 32 bits.*/
	int nbOctetsParPixel = surface->format->BytesPerPixel;
	/* Ici p est l'adresse du pixel que l'on veut connaitre */
	/*surface->pixels contient l'adresse du premier pixel de l'image*/
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

	/*Gestion diff�rente suivant le nombre d'octets par pixel de l'image*/
	switch(nbOctetsParPixel)
	{
		case 1:
			return *p;

		case 2:
			return *(Uint16 *)p;

		case 3:
			/*Suivant l'architecture de la machine*/
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		case 4:
			return *(Uint32 *)p;

		/*Ne devrait pas arriver, mais �vite les erreurs*/
		default:
			return 0;
	}
}

/***************************************************************************//*!
* @fn int moteur_sdl::afficherGagnant( const partie* p )
* @brief Permet d'obtenir d'afficher le gagnant de la partie
* @return
*	- 1 : Rejouer sur la m�me map
*	- 2 : Nouvelle Map
*	- 3 : Action annul�e / Retour
*/
int moteur_sdl::afficherGagnant( const partie* p, int retourPartie )
{
	unsigned int highLight = 1;
	bool continuer = 1;
	SDL_Event event;
	SDL_Rect position;
	SDL_Surface* sfr_titre=0;
	SDL_Surface* sfr_titre_bg=0;
	SDL_Surface* textes[3*2]={0};
	SDL_Surface* gameScreenOrg;
	SDL_Surface* gameScreenMin;
	SDL_Surface* gameScreenMinPos;
	unsigned int i;
	bool dessiner = 1;

	const char* choix[] = {
		"Refaire une partie",
		"Changer de map",
		"Retour"
	};
	enum {nb_choix=3};


	/***************************************************************************
	* On enregistre l'�cran actuel
	*/
	gameScreenOrg = SDL_CreateRGBSurface
		(
			c_ecranGeneral->flags, c_ecranGeneral->w, c_ecranGeneral->h,
			c_ecranGeneral->format->BitsPerPixel, c_ecranGeneral->format->Rmask,
			c_ecranGeneral->format->Gmask, c_ecranGeneral->format->Bmask,
			c_ecranGeneral->format->Amask
		);

	if( !gameScreenOrg )
		stdErrorE("Erreur lors de la cr�ation de l'�cran avec SDL_CreateRGBSurface");

	// Copie
	if( SDL_BlitSurface(c_ecranGeneral, NULL, gameScreenOrg, NULL) == -1 )
		stdErrorE("Erreur lors de la copie d'�cran avec SDL_BlitSurface");

	// Cr�ation de la miniature
	SDL_Surface* rotoScreen = rotozoomSurface(c_ecranGeneral, 0, 0.3, 1);
	if( !rotoScreen )
		stdErrorE("Erreur lors de la cr�ation de l'�cran avec rotozoomSurface");

	// Cr�ation du fond de la miniature ( 4px plus grand que la "vrais" miniature, pour ajouter des bordures)
	gameScreenMin = SDL_CreateRGBSurface
		(
			rotoScreen->flags, rotoScreen->w+4, rotoScreen->h+4,
			rotoScreen->format->BitsPerPixel, rotoScreen->format->Rmask,
			rotoScreen->format->Gmask, rotoScreen->format->Bmask,
			rotoScreen->format->Amask
		);

	if( !gameScreenMin )
		stdErrorE("Erreur lors de la cr�ation de l'�cran avec SDL_CreateRGBSurface");

	// Permet de cr�er un marge
	position.x = 2;
	position.y = 2;
	// Copie du rotoScreen vers la miniature
	if( SDL_BlitSurface(rotoScreen, NULL, gameScreenMin, &position) == -1 )
		stdErrorE("Erreur lors de la copie d'�cran avec SDL_BlitSurface");

	// rotoScreen est devenu inutil
	SDL_FreeSurface(rotoScreen);

	// On fait une rotation de la miniature
	rotoScreen = rotozoomSurface(gameScreenMin, 15, 1.0, 1);
	// L'ancienne miniature est inutil
	SDL_FreeSurface(gameScreenMin);
	gameScreenMin = rotoScreen;

	// Cr�ation d'une surface pour ce balader sur les pixels
	gameScreenMinPos = rotozoomSurface(gameScreenMin, 0, 1.0, 1);

	/***************************************************************************
	* Ecriture des textes
	*/
	for( i=0; i<nb_choix; i++ )// Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal)
	{
		textes[i] = ecritTexte(choix[i]);
		textes[i+nb_choix] = ecritTexte(choix[i], couleurOrange);// Cr�ation du texte HighLight
	}

	/***************************************************************************
	* Cr�ation du titre
	*/
	if( retourPartie >= 0 ){
		if( p->getWinnerID() > p->nbJoueurs() )
			stdErrorE("Erreur avec getWinnerID ! p->getWinnerID(){%d} > p->nbJoueurs(){%d}", p->getWinnerID(), p->nbJoueurs());
		if( !p->joueur(p->getWinnerID()-1) )
			stdErrorE("Le joueur demand�: %d, renvoie un pointeur NULL pour p->joueur(p->getWinnerID()-1)==NULL", p->getWinnerID()-1);

		char tmp[22+21+1+getSizeOfNumber<-1>::value] = {0};
		snprintf(tmp, 22+21+getSizeOfNumber<-1>::value, "Le joueur(%d) %s a gagne !", retourPartie, p->joueur(p->getWinnerID()-1)->nom()->c_str());
		sfr_titre = ecritTexte(tmp, c_policeTitre);
		sfr_titre_bg = ecritTexte(tmp, c_policeTitre, couleurNoir);

	}else if( retourPartie == 0 ){
		sfr_titre = ecritTexte("Match nul ! Pas de gagnant !", c_policeTitre);
		sfr_titre_bg = ecritTexte("Match nul ! Pas de gagnant !", c_policeTitre, couleurNoir);

	}else if( retourPartie < 0 ){
		sfr_titre = ecritTexte("...", c_policeTitre);
		sfr_titre_bg = ecritTexte("...", c_policeTitre, couleurNoir);

	}else
		stdErrorE("Cas non pris en charge : %d", retourPartie);


	while( continuer )
	{
		if( !dessiner )
			SDL_WaitEvent(&event);

		if( !dessiner )
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = nb_choix;// On veux quitter !
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: {
				switch( event.key.keysym.sym )
				{
					case SDLK_UP: { /* Appui sur la touche Echap, on arr�te le programme */
						if( highLight == 1 ){// On bouge notre highLight vers le haut
							highLight = nb_choix;
						}else{
							highLight--;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_DOWN: {// On bouge notre highLight vers le bas
						if( highLight == nb_choix ){
							highLight = 1;
						}else{
							highLight++;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_KP_ENTER:
					case SDLK_RETURN: {// On a valider notre choix par entrer
						continuer = 0;
						break;
					}
					case SDLK_ESCAPE: { /* Appui sur la touche Echap, on arr�te le programme */
						highLight = nb_choix;// On veux quitter !
						continuer = 0;
						break;
					}
					default:
						break;
				}
				break;
			}
			/*******************************************************************
			* Prise en charge de la souris
			*/
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONUP: {
				position.y = Marge_menu+100;
				for( i=0; i<nb_choix; i++ )
				{
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					if( position.x <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
						if( highLight != i+1 ){
							highLight = i+1;
							dessiner = 1;
						}
						if( event.type == SDL_MOUSEBUTTONUP )
							continuer = 0;
						break;
					}
					position.y += textes[i]->h;// Espacement entre les textes ( blit )
				}

				if( event.type == SDL_MOUSEBUTTONUP && c_ecranGeneral->w-gameScreenMin->w <= event.button.x && c_ecranGeneral->h-gameScreenMin->h <= event.button.y ){
					if( *(Uint32 *)((Uint8 *)gameScreenMinPos->pixels + (event.button.y-c_ecranGeneral->h+gameScreenMinPos->h) * gameScreenMinPos->pitch + (event.button.x-c_ecranGeneral->w+gameScreenMinPos->w) * gameScreenMinPos->format->BytesPerPixel) != 0 ){
						SDL_BlitSurface(gameScreenOrg, NULL, c_ecranGeneral, NULL);// Blit l'ecran de jeu
						SDL_Flip(c_ecranGeneral);
						do{
							SDL_WaitEvent(&event);
						}while( event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN );
						dessiner = 1;
					}
				}
				break;
			}
			default:
				break;
		}

		/***********************************************************************
		* Blitage g�n�ral
		* > On blit uniquement s'il y a eu une modification
		*/
		if( dessiner ){
			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

			position.x = c_ecranGeneral->w-gameScreenMin->w;
			position.y = c_ecranGeneral->h-gameScreenMin->h;
			SDL_BlitSurface(gameScreenMin, NULL, c_ecranGeneral, &position);// Blit l'ecran de jeu

			position.y = Marge_menu;// Point de d�part
			if( sfr_titre ){
				position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
				position.x -= 5;
				position.y -= 5;
				SDL_BlitSurface(sfr_titre_bg, NULL, c_ecranGeneral, &position);// Blit background
				position.x += 5;
				position.y += 5;
				SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit title
			}
			position.y += 100;
			// On blit le texte
			for( i=0; i<nb_choix; i++ )
			{
				position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
				if( i == highLight-1 ){
					SDL_BlitSurface(textes[i+nb_choix], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}else{
					SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}
				position.y += textes[i]->h;// Espacement entre les textes ( blit )
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}
	}

	SDL_FreeSurface(gameScreenOrg);
	SDL_FreeSurface(gameScreenMin);
	SDL_FreeSurface(gameScreenMinPos);

	for( i=0; i<nb_choix*2; i++ )// On lib�re les textes
		SDL_FreeSurface(textes[i]);

	if( sfr_titre )
		SDL_FreeSurface(sfr_titre);

	if( sfr_titre_bg )
		SDL_FreeSurface(sfr_titre_bg);

	return highLight;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER moteur_sdl::afficherMapEtEvent( partie* p )
* @brief Affiche une map
* @param[in] p	La partie en cours a afficher
* @return La touche actuellement appuy�
*/
SYS_CLAVIER moteur_sdl::afficherMapEtEvent( partie* p )
{
	if( !c_Instance )
		stdErrorE("Veuillez instancier la class moteur_sdl !");

	bool dessiner = 0;
	bool refresh = 0;
	SDL_Surface* sfr_tmp;
	map* l_map = p->getMap();// l_map pour local variable map
	// D�calage
	unsigned int	xpos=(c_Instance->c_ecranGeneral->w-l_map->X()*32)/2,
					ypos=(c_Instance->c_ecranGeneral->h-l_map->Y()*32)/2;
	ypos = ypos+ypos/2;

	SDL_Rect pos;
	SDL_Rect imgRect;

	// Tester ici si la taille de la map n'est pas trop grande pour la fenetre.
	if ( l_map->X() > (unsigned int)c_Instance->c_ecranGeneral->w/32 || l_map->Y() > (unsigned int)c_Instance->c_ecranGeneral->h/32 )
		stdErrorE("%u > %d || %u > %d", l_map->X(), c_Instance->c_ecranGeneral->w/32, l_map->Y(), c_Instance->c_ecranGeneral->h/32);

	if( c_Instance->c_premierAffichage ){
		pos.x = 0;
		pos.y = 0;
		// On met un fond bleu
		//SDL_FillRect(c_Instance->c_ecranGeneral, NULL, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
		SDL_BlitSurface(c_Instance->c_backgroundLayer, NULL, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

		// On cr�� un tableau de sprite de joueurs
		{
			Sprite s = {{0,0}, (t_obj)0, 0, {0,0}};
			c_Instance->c_ListSpriteJoueur.clear();
			c_Instance->c_ListSpriteJoueur.resize(p->nbJoueurs(), s);
		}

		for( unsigned int x, y=0; y<l_map->Y(); y++ )
		{
			for( x=0; x<l_map->X(); x++ )
			{
				pos.x=x*32+xpos;
				pos.y=y*32+ypos;
				switch( l_map->getBlock(x, y)->element )
				{
					case map::vide: {
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::Mur_destructible: {
						SDL_BlitSurface(c_Instance->c_Decor[mur_destructible], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::Mur_INdestructible: {
						SDL_BlitSurface(c_Instance->c_Decor[mur_indestructible], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::UN_joueur: {
						if( !l_map->getBlock(x, y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y)->joueur=0", x, y);
						if( !l_map->getBlock(x, y)->joueur->size() )
							stdErrorE("l_map->getBlock(%u, %u)->joueur->size()=0", x, y);

						unsigned char joueur = l_map->getBlock(x, y)->joueur->at(0);
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
						break;
					}
					case map::plusieurs_joueurs: {
						if( !l_map->getBlock(x, y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y)->joueur=0", x, y);

						unsigned char joueur;
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						for( int i=l_map->getBlock(x, y)->joueur->size()-1; i>=0; i-- ){
							joueur = l_map->getBlock(x, y)->joueur->at(i);
							c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
						}
						break;
					}
					case map::bombe_poser: {
						if( c_Instance->isInSpriteList(coordonneeConvert(x, y)) == -1 ){
							Sprite b;
							b.pos = coordonneeConvert(x, y);
							b.objet = bombe;
							b.frame = 0;
							b.refresh.tv_sec = 0;
							b.refresh.tv_usec = 0;
							c_Instance->c_ListSprite.push_back(b);
						}

						imgRect.x = 0;
						imgRect.y = 0;
						imgRect.w = 32;
						imgRect.h = 32;
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::bombe_poser_AVEC_UN_joueur:
					case map::bombe_poser_AVEC_plusieurs_joueurs: {
						if( !l_map->getBlock(x, y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y)->joueur=0", x, y);

						if( l_map->getBlock(x, y)->joueur->size() <= 1 )
							stdErrorE("Nombre d'info joueur (%u) incorrect !", l_map->getBlock(x, y)->joueur->size());

						unsigned char joueur = l_map->getBlock(x, y)->joueur->at(1);

						if( c_Instance->isInSpriteList(coordonneeConvert(x, y)) == -1 ){
							Sprite b;
							b.pos = coordonneeConvert(x, y);
							b.objet = bombe;
							b.frame = 0;
							b.refresh.tv_sec = 0;
							b.refresh.tv_usec = 0;
							c_Instance->c_ListSprite.push_back(b);
						}

						imgRect.x = 0;
						imgRect.y = 0;
						imgRect.w = 32;
						imgRect.h = 32;
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
						break;
					}
					case map::flamme_origine: {
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[flamme_origine], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_horizontal: {
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[flamme_horizontal], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_vertical: {
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[flamme_vertical], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_haut: {
						SDL_BlitSurface(c_Instance->c_Decor[flamme_pointe_haut], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_bas: {
						SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_bas], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_droite: {
						SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_droite], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_gauche: {
						SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_gauche], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::bonus: {
						if( !l_map->getBlock(x, y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y).joueur=0", x, y);

						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						switch( (bonus::t_Bonus)l_map->getBlock(x, y)->joueur->at(0) )
						{
							case bonus::bombe: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::super_bombe: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_super_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::puissance_flamme: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::super_puissance_flamme: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_super_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::declancheur: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::pousse_bombe: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::vie: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::teleporteur: {
								SDL_BlitSurface(c_Instance->c_Decor[bonus_teleporteur], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::inversseur_touche: {
								SDL_BlitSurface(c_Instance->c_Decor[bonus_inversseur_touche], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::force_explosion: {
								SDL_BlitSurface(c_Instance->c_Decor[bonus_force_explosion], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							default: {
								stdError("Erreur lors de la lecture de la map, BONUS inconu <%d>", (int)l_map->getBlock(x, y)->joueur->at(0));
								break;
							}
						}
						break;
					}
					default: {
						stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(x, y)->element);
						break;
					}
				}
			}
		}

		SDL_Flip(c_Instance->c_ecranGeneral);// afficher la map
	}else{
		s_Coordonnees v_pos;
		while( l_map->getModifications(v_pos) )
		{
			dessiner = 1;
			pos.x=v_pos.x*32+xpos;
			pos.y=v_pos.y*32+ypos;
			switch( l_map->getBlock(v_pos)->element )
			{
				case map::vide: {
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::Mur_destructible: {
					SDL_BlitSurface(c_Instance->c_Decor[mur_destructible], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::Mur_INdestructible: {
					SDL_BlitSurface(c_Instance->c_Decor[mur_indestructible], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::UN_joueur: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					if( !l_map->getBlock(v_pos)->joueur->size() )
						stdErrorE("l_map->getBlock(%u, %u)->joueur->size()=0", v_pos.x, v_pos.y);

					unsigned char joueur = l_map->getBlock(v_pos)->joueur->at(0);
/*
					c_Instance->c_ListSpriteJoueur[joueur-1].pos.x = p->joueur(joueur-1)->old_X()*32+xpos;
					c_Instance->c_ListSpriteJoueur[joueur-1].pos.y = p->joueur(joueur-1)->old_Y()*32+ypos;
					switch( joueur )
					{
						case 1:
							c_Instance->c_ListSpriteJoueur[joueur-1].objet = joueur1_haut;
							break;
						case 2:
							c_Instance->c_ListSpriteJoueur[joueur-1].objet = joueur2_haut;
							break;
						case 3:
							c_Instance->c_ListSpriteJoueur[joueur-1].objet = joueur3_haut;
							break;
						case 4:
							c_Instance->c_ListSpriteJoueur[joueur-1].objet = joueur4_haut;
							break;
					}
					c_Instance->c_ListSpriteJoueur[joueur-1].frame = 0;
					c_Instance->c_ListSpriteJoueur[joueur-1].refresh = 0;
*/
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
					break;
				}
				case map::plusieurs_joueurs: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					unsigned char joueur;
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					for( int i=l_map->getBlock(v_pos)->joueur->size()-1; i>=0; i-- ){
						joueur = l_map->getBlock(v_pos)->joueur->at(i);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
					}
					break;
				}
				case map::bombe_poser: {
					if( c_Instance->isInSpriteList(v_pos) == -1 ){
						Sprite b;
						b.pos = v_pos;
						b.objet = bombe;
						b.frame = 0;
						b.refresh.tv_sec = 0;
						b.refresh.tv_usec = 0;
						c_Instance->c_ListSprite.push_back(b);
					}

					imgRect.x = 0;
					imgRect.y = 0;
					imgRect.w = 32;
					imgRect.h = 32;
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::bombe_poser_AVEC_UN_joueur: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					if( l_map->getBlock(v_pos)->joueur->size() <= 1 )
						stdErrorE("Nombre d'info joueur (%u) incorrect !", l_map->getBlock(v_pos)->joueur->size());

					unsigned char joueur = l_map->getBlock(v_pos)->joueur->at(1);

					if( c_Instance->isInSpriteList(v_pos) == -1 ){
						Sprite b;
						b.pos = v_pos;
						b.objet = bombe;
						b.frame = 0;
						b.refresh.tv_sec = 0;
						b.refresh.tv_usec = 0;
						c_Instance->c_ListSprite.push_back(b);
					}

					imgRect.x = 0;
					imgRect.y = 0;
					imgRect.w = 32;
					imgRect.h = 32;
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
					c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
					break;
				}
				case map::bombe_poser_AVEC_plusieurs_joueurs: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					if( l_map->getBlock(v_pos)->joueur->size() <= 1 )
						stdErrorE("Nombre d'info joueur (%u) incorrect !", l_map->getBlock(v_pos)->joueur->size());

					unsigned char joueur;

					if( c_Instance->isInSpriteList(v_pos) == -1 ){
						Sprite b;
						b.pos = v_pos;
						b.objet = bombe;
						b.frame = 0;
						b.refresh.tv_sec = 0;
						b.refresh.tv_usec = 0;
						c_Instance->c_ListSprite.push_back(b);
					}

					imgRect.x = 0;
					imgRect.y = 0;
					imgRect.w = 32;
					imgRect.h = 32;
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
					for( int i=l_map->getBlock(v_pos)->joueur->size()-1; i>=1; i-- ){
						joueur = l_map->getBlock(v_pos)->joueur->at(i);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
					}
					break;
				}
				case map::flamme_origine: {
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[flamme_origine], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_horizontal: {

					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[flamme_horizontal], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_vertical: {
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[flamme_vertical], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_haut: {
					SDL_BlitSurface(c_Instance->c_Decor[flamme_pointe_haut], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_bas: {
					SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_bas], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_droite: {
					SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_droite], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_gauche: {
					SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_gauche], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::bonus: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y).joueur=0", v_pos.x, v_pos.y);

					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					switch( (bonus::t_Bonus)l_map->getBlock(v_pos)->joueur->at(0) )
					{
						case bonus::bombe: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::super_bombe: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_super_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::puissance_flamme: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::super_puissance_flamme: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_super_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::declancheur: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::pousse_bombe: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::vie: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::teleporteur: {
							SDL_BlitSurface(c_Instance->c_Decor[bonus_teleporteur], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::inversseur_touche: {
							SDL_BlitSurface(c_Instance->c_Decor[bonus_inversseur_touche], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::force_explosion: {
							SDL_BlitSurface(c_Instance->c_Decor[bonus_force_explosion], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						default: {
							stdError("Erreur lors de la lecture de la map, BONUS inconu <%d>", (int)l_map->getBlock(v_pos)->joueur->at(0));
							break;
						}
					}
					break;
				}
				default: {
					stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(v_pos)->element);
					break;
				}
			}
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, 32, 32);
		}
	}


	s_timeval now;
	gettimeofday(&now, 0);


	/***************************************************************************
	* Blitage des animations
	*/
	for( unsigned int i=0; i<c_Instance->c_ListSprite.size(); i++ )
	{
		switch( c_Instance->c_ListSprite.at(i).objet )
		{
			case bombe: {
				switch( l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->element )
				{
					case map::bombe_poser: {
						if( c_Instance->c_ListSprite.at(i).refresh <= now ){
							pos.x=c_Instance->c_ListSprite.at(i).pos.x*32+xpos;
							pos.y=c_Instance->c_ListSprite.at(i).pos.y*32+ypos;
							imgRect.x = (c_Instance->c_ListSprite.at(i).frame%(c_Instance->c_Decor[bombe]->w/32))*32;
							imgRect.y = 0;
							imgRect.w = 32;
							imgRect.h = 32;
							SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
							SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
							c_Instance->c_ListSprite.at(i).frame++;
							c_Instance->c_ListSprite.at(i).refresh.tv_sec = now.tv_sec+VitesseSpriteBombe/1000000;
							c_Instance->c_ListSprite.at(i).refresh.tv_usec = now.tv_usec+VitesseSpriteBombe%1000000;
							if( c_Instance->c_ListSprite.at(i).refresh.tv_usec > 999999 ){
								c_Instance->c_ListSprite.at(i).refresh.tv_sec += c_Instance->c_ListSprite.at(i).refresh.tv_usec/1000000;
								c_Instance->c_ListSprite.at(i).refresh.tv_usec %= 1000000;
							}

							SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, 32, 32);
						}
						break;
					}
					case map::bombe_poser_AVEC_UN_joueur: {
						if( c_Instance->c_ListSprite.at(i).refresh <= now ){
							if( !l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur )
								stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(pos).joueur=0", c_Instance->c_ListSprite.at(i).pos.x, c_Instance->c_ListSprite.at(i).pos.y);

							unsigned char joueur = l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur->at(1);

							pos.x=c_Instance->c_ListSprite.at(i).pos.x*32+xpos;
							pos.y=c_Instance->c_ListSprite.at(i).pos.y*32+ypos;
							imgRect.x = (c_Instance->c_ListSprite.at(i).frame%(c_Instance->c_Decor[bombe]->w/32))*32;
							imgRect.y = 0;
							imgRect.w = 32;
							imgRect.h = 32;
							SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
							SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
							c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
							c_Instance->c_ListSprite.at(i).frame++;
							c_Instance->c_ListSprite.at(i).refresh.tv_sec = now.tv_sec+VitesseSpriteBombe/1000000;
							c_Instance->c_ListSprite.at(i).refresh.tv_usec = now.tv_usec+VitesseSpriteBombe%1000000;
							if( c_Instance->c_ListSprite.at(i).refresh.tv_usec > 999999 ){
								c_Instance->c_ListSprite.at(i).refresh.tv_sec += c_Instance->c_ListSprite.at(i).refresh.tv_usec/1000000;
								c_Instance->c_ListSprite.at(i).refresh.tv_usec %= 1000000;
							}

							SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, 32, 32);
						}
						break;
					}
					case map::bombe_poser_AVEC_plusieurs_joueurs: {
						if( c_Instance->c_ListSprite.at(i).refresh <= now ){
							if( !l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur )
								stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(pos).joueur=0", c_Instance->c_ListSprite.at(i).pos.x, c_Instance->c_ListSprite.at(i).pos.y);

							unsigned char joueur;

							pos.x=c_Instance->c_ListSprite.at(i).pos.x*32+xpos;
							pos.y=c_Instance->c_ListSprite.at(i).pos.y*32+ypos;
							imgRect.x = (c_Instance->c_ListSprite.at(i).frame%(c_Instance->c_Decor[bombe]->w/32))*32;
							imgRect.y = 0;
							imgRect.w = 32;
							imgRect.h = 32;
							SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
							SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
							for( int k=l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur->size()-1; k>=1; k-- ){
								joueur = l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur->at(k);
								c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos, 0 );
							}
							c_Instance->c_ListSprite.at(i).frame++;
							c_Instance->c_ListSprite.at(i).refresh.tv_sec = now.tv_sec+VitesseSpriteBombe/1000000;
							c_Instance->c_ListSprite.at(i).refresh.tv_usec = now.tv_usec+VitesseSpriteBombe%1000000;
							if( c_Instance->c_ListSprite.at(i).refresh.tv_usec > 999999 ){
								c_Instance->c_ListSprite.at(i).refresh.tv_sec += c_Instance->c_ListSprite.at(i).refresh.tv_usec/1000000;
								c_Instance->c_ListSprite.at(i).refresh.tv_usec %= 1000000;
							}

							SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, 32, 32);
						}
						break;
					}
					default: {
						c_Instance->c_ListSprite.erase(c_Instance->c_ListSprite.begin()+i);
						i--;
						break;
					}
				}
				break;
			}
			default: {
				stdError("Attention objet anim� inconnu ! X=%u, Y=%u, objet=%d", c_Instance->c_ListSprite.at(i).pos.x, c_Instance->c_ListSprite.at(i).pos.y, c_Instance->c_ListSprite.at(i).objet);
				c_Instance->c_ListSprite.erase(c_Instance->c_ListSprite.begin()+i);
				i--;
				break;
			}
		}
	}

	/***************************************************************************
	* Blitage des animations des joueurs
	*/
	for( unsigned int i=0; i<c_Instance->c_ListSpriteJoueur.size(); i++ )
	{
		if( c_Instance->c_ListSpriteJoueur.at(i).objet == (t_obj)0 )
			continue;

		if( c_Instance->c_ListSpriteJoueur.at(i).refresh <= now ){
			pos.x = c_Instance->c_ListSpriteJoueur.at(i).pos.x;
			pos.y = c_Instance->c_ListSpriteJoueur.at(i).pos.y;

			if( p->joueur(i)->X()*32+xpos == (unsigned int)pos.x && p->joueur(i)->Y()*32+ypos == (unsigned int)pos.y ){
				stdError("Fin Animation");
				SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
				c_Instance->joueur_orientation(p->joueur(i)->orientation(), i+1, &pos, 0);
				c_Instance->c_ListSpriteJoueur.at(i).objet = (t_obj)0;
				break;// Pas d'animation a faire
			}
			stdError("Animation");

			pos.x = p->joueur(i)->X()*32+xpos;
			pos.y = p->joueur(i)->Y()*32+ypos;
			SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x = p->joueur(i)->old_X()*32+xpos;
			pos.y = p->joueur(i)->old_Y()*32+ypos;
			SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);

			if( p->joueur(i)->X() < p->joueur(i)->old_X() )
				c_Instance->c_ListSpriteJoueur.at(i).pos.x--;
			else if( p->joueur(i)->X() > p->joueur(i)->old_X() )
				c_Instance->c_ListSpriteJoueur.at(i).pos.x++;

			if( p->joueur(i)->Y() < p->joueur(i)->old_Y() )
				c_Instance->c_ListSpriteJoueur.at(i).pos.y--;
			else if( p->joueur(i)->Y() > p->joueur(i)->old_Y() )
				c_Instance->c_ListSpriteJoueur.at(i).pos.y++;

			pos.x = c_Instance->c_ListSpriteJoueur.at(i).pos.x;
			pos.y = c_Instance->c_ListSpriteJoueur.at(i).pos.y;

			c_Instance->joueur_orientation(p->joueur(i)->orientation(), i+1, &pos, c_Instance->c_ListSpriteJoueur.at(i).frame);
			c_Instance->c_ListSpriteJoueur.at(i).frame++;
			c_Instance->c_ListSpriteJoueur.at(i).refresh.tv_sec = now.tv_sec+VitessePerso/1000000;
			c_Instance->c_ListSpriteJoueur.at(i).refresh.tv_usec = now.tv_usec+VitessePerso%1000000;
			if( c_Instance->c_ListSpriteJoueur.at(i).refresh.tv_usec > 999999 ){
				c_Instance->c_ListSpriteJoueur.at(i).refresh.tv_sec += c_Instance->c_ListSpriteJoueur.at(i).refresh.tv_usec/1000000;
				c_Instance->c_ListSpriteJoueur.at(i).refresh.tv_usec %= 1000000;
			}

			SDL_UpdateRect(c_Instance->c_ecranGeneral, p->joueur(i)->X()*32+xpos-32, p->joueur(i)->Y()*32+ypos-32, 32*2, 32*2);
		}

	}


	pos.x = 0;
	pos.y = 0;
	char tempsAff[30];


	/***************************************************************************
	* Affichage du temps de jeu restant
	*/
	static int old_timer = 0;
	if( p->timeOut() > time(0) ){
		int min = (p->timeOut()-time(0))/60;
		int secs = (p->timeOut()-time(0))%60;

		if( old_timer != min*100+secs){
			old_timer = min*100+secs;

			imgRect.x = 0;
			imgRect.y = 0;
			imgRect.w = c_Instance->c_clock[0]->w*4+c_Instance->c_clock[11]->w+c_Instance->c_clock[11]->w+50;
			imgRect.h = c_Instance->c_clock[11]->h;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos);// blittage du background

			SDL_BlitSurface(c_Instance->c_clock[min/10], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.x += c_Instance->c_clock[min/10]->w;
			SDL_BlitSurface(c_Instance->c_clock[min%10], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.x += c_Instance->c_clock[min%10]->w;

			SDL_BlitSurface(c_Instance->c_clock[10], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.x += c_Instance->c_clock[10]->w;

			SDL_BlitSurface(c_Instance->c_clock[secs/10], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.x += c_Instance->c_clock[secs/10]->w;
			SDL_BlitSurface(c_Instance->c_clock[secs%10], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.x += c_Instance->c_clock[secs%10]->w;

			// Secs
			SDL_BlitSurface(c_Instance->c_clock[11], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.x += c_Instance->c_clock[11]->w;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, 0, 0, pos.x+50, c_Instance->c_clock[11]->h);
		}
	}else if( old_timer ){
		imgRect.x = 0;
		imgRect.y = 0;
		imgRect.w = c_Instance->c_clock[0]->w*4+c_Instance->c_clock[11]->w+c_Instance->c_clock[11]->w+50;
		imgRect.h = c_Instance->c_clock[11]->h;
		SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos);// blittage du background

		SDL_BlitSurface(c_Instance->c_clock[0], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		pos.x += c_Instance->c_clock[0]->w;
		SDL_BlitSurface(c_Instance->c_clock[0], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		pos.x += c_Instance->c_clock[0]->w;

		// Min
		SDL_BlitSurface(c_Instance->c_clock[10], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		pos.x += c_Instance->c_clock[10]->w;

		SDL_BlitSurface(c_Instance->c_clock[0], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		pos.x += c_Instance->c_clock[0]->w;
		SDL_BlitSurface(c_Instance->c_clock[0], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		pos.x += c_Instance->c_clock[0]->w;

		// Secs
		SDL_BlitSurface(c_Instance->c_clock[11], NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		pos.x += c_Instance->c_clock[11]->w;

		SDL_UpdateRect(c_Instance->c_ecranGeneral, 0, 0, pos.x+50, c_Instance->c_clock[11]->h);
		old_timer = 0;
	}


	/***************************************************************************
	* Affichage du joueur 1
	*/
	// Affichage du nom du joueur 1
	pos.x = 10;
	pos.y = ypos;
	refresh = p->joueur(0)->need_refresh();
	if( refresh ){
		// Affichage du nom du joueur 1
		sfr_tmp = c_Instance->ecritTexte(p->joueur(0)->nom()->c_str(), c_mainColor, 55);
		// On remplis le fond
		imgRect.x = pos.x;
		imgRect.y = pos.y;
		imgRect.w = xpos-pos.x;
		imgRect.h = sfr_tmp->h;
		SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */
		if( (unsigned int)pos.x+(unsigned int)sfr_tmp->w > xpos ){
			imgRect.x = 0;
			imgRect.y = 0;
			SDL_BlitSurface(sfr_tmp, &imgRect, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		}else{
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		}
		SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
		SDL_FreeSurface(sfr_tmp);
	}
	pos.y += 25;// Espacement
	if( p->joueur(0)->armements() ){
		// Background
		imgRect.w = xpos-40;// 40 (espacement bonus)
		imgRect.h = 32;

		// Bombe
		static unsigned char nbBombe = 0;
		pos.x = 40;
		pos.y += 35;
		if( nbBombe != p->joueur(0)->armements()->quantiteMAX(bonus::bombe) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::bombe) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

			SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			nbBombe = p->joueur(0)->armements()->quantiteMAX(bonus::bombe);
		}

		// Puissance de flamme
		static unsigned char PuissFlamme = 0;
		pos.x = 40;
		pos.y += 35;
		if( PuissFlamme != p->joueur(0)->armements()->quantiteMAX(bonus::puissance_flamme) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::puissance_flamme) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

			SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			PuissFlamme = p->joueur(0)->armements()->quantiteMAX(bonus::puissance_flamme);
		}

		// D�clancheur
		static unsigned char Declancheur = 0;
		pos.x = 40;
		pos.y += 35;
		if( Declancheur != p->joueur(0)->armements()->quantiteMAX(bonus::declancheur) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::declancheur) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

			SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			Declancheur = p->joueur(0)->armements()->quantiteMAX(bonus::declancheur);
		}

		// Vie
		static unsigned char nbVie = 0;
		pos.x = 40;
		pos.y += 35;
		if( nbVie != p->joueur(0)->armements()->quantiteMAX(bonus::vie) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::vie) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

			SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			nbVie = p->joueur(0)->armements()->quantiteMAX(bonus::vie);
		}

		// Pousse
		static unsigned char PousseBombe = 0;
		pos.x = 40;
		pos.y += 35;
		if( PousseBombe != p->joueur(0)->armements()->quantiteMAX(bonus::pousse_bombe) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::pousse_bombe) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

			SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			PousseBombe = p->joueur(0)->armements()->quantiteMAX(bonus::pousse_bombe);
		}
	}

	/***************************************************************************
	* Affichage du joueur 4
	*/
	static bool cleanPlayer4=0;
	if( p->nbJoueurs() == 4 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(3)->isLocal() || ( !p->joueur(3)->isLocal() && p->joueur(3)->socket() != INVALID_SOCKET )))) ){
		refresh = p->joueur(3)->need_refresh();
		// Affichage du nom du joueur 4
		pos.x = 10;
		pos.y = l_map->Y()*32-(25+35*4);
		if( refresh || c_Instance->c_premierAffichage ){
			// Affichage du nom du joueur 4
			sfr_tmp = c_Instance->ecritTexte(p->joueur(3)->nom()->c_str(), c_mainColor, 55);
			// On remplis le fond
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			imgRect.w = xpos-pos.x;
			imgRect.h = sfr_tmp->h;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */
			if( (unsigned int)pos.x+(unsigned int)sfr_tmp->w > xpos ){
				imgRect.x = 0;
				imgRect.y = 0;
				SDL_BlitSurface(sfr_tmp, &imgRect, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}else{
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			SDL_FreeSurface(sfr_tmp);
		}
		pos.y += 25;// Espacement
		if( p->joueur(3)->armements() ){
			// Background
			imgRect.w = xpos-40;// 40 (espacement bonus)
			imgRect.h = 32;

			// Bombe
			static unsigned char nbBombe = 0;
			pos.x = 40;
			pos.y += 35;
			if( nbBombe != p->joueur(3)->armements()->quantiteMAX(bonus::bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbBombe = p->joueur(3)->armements()->quantiteMAX(bonus::bombe);
			}

			// Puissance de flamme
			static unsigned char PuissFlamme = 0;
			pos.x = 40;
			pos.y += 35;
			if( PuissFlamme != p->joueur(3)->armements()->quantiteMAX(bonus::puissance_flamme) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::puissance_flamme) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PuissFlamme = p->joueur(3)->armements()->quantiteMAX(bonus::puissance_flamme);
			}

			// D�clancheur
			static unsigned char Declancheur = 0;
			pos.x = 40;
			pos.y += 35;
			if( Declancheur != p->joueur(3)->armements()->quantiteMAX(bonus::declancheur) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::declancheur) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				Declancheur = p->joueur(3)->armements()->quantiteMAX(bonus::declancheur);
			}

			// Vie
			static unsigned char nbVie = 0;
			pos.x = 40;
			pos.y += 35;
			if( nbVie != p->joueur(3)->armements()->quantiteMAX(bonus::vie) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::vie) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbVie = p->joueur(3)->armements()->quantiteMAX(bonus::vie);
			}

			// Pousse Bombe
			static unsigned char PousseBombe = 0;
			pos.x = 40;
			pos.y += 35;
			if( PousseBombe != p->joueur(3)->armements()->quantiteMAX(bonus::pousse_bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::pousse_bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PousseBombe = p->joueur(3)->armements()->quantiteMAX(bonus::pousse_bombe);
			}
		}
		cleanPlayer4 = 1;
	}else{
		if( cleanPlayer4 ){
			// Blit background
			imgRect.x = 10;
			imgRect.y = l_map->Y()*32-(25+35*4);
			imgRect.w = xpos;
			imgRect.h = 25+35*6;
			pos.x = imgRect.x;
			pos.y = imgRect.y;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */
			SDL_UpdateRect(c_Instance->c_ecranGeneral, imgRect.x, imgRect.y, imgRect.w, imgRect.h);
			cleanPlayer4 = 0;
		}
	}

	/***************************************************************************
	* Affichage du joueur 2
	*/
	static bool cleanPlayer2=0;
	if( p->nbJoueurs() > 1 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(1)->isLocal() || ( !p->joueur(1)->isLocal() && p->joueur(1)->socket() != INVALID_SOCKET )))) ){
		refresh = p->joueur(1)->need_refresh();
		// Affichage du nom du joueur 2
		pos.x = l_map->X()*32+xpos+10;
		pos.y = l_map->Y()*32-(25+35*4);

		if( refresh || c_Instance->c_premierAffichage ){
			// Affichage du nom du joueur 2
			sfr_tmp = c_Instance->ecritTexte(p->joueur(1)->nom()->c_str(), c_mainColor, 55);

			// On remplis le fond
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			imgRect.w = xpos-10;
			imgRect.h = sfr_tmp->h;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */
			if( pos.x+sfr_tmp->w > c_Instance->c_ecranGeneral->w ){
				imgRect.x = 0;
				imgRect.y = 0;
				SDL_BlitSurface(sfr_tmp, &imgRect, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}else{
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			SDL_FreeSurface(sfr_tmp);
		}
		pos.y += 25;// Espacement
		if( p->joueur(1)->armements() ){
			// Background
			imgRect.w = xpos-40;// 40 (espacement bonus)
			imgRect.h = 32;

			// Bombe
			static unsigned char nbBombe = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( nbBombe != p->joueur(1)->armements()->quantiteMAX(bonus::bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbBombe = p->joueur(1)->armements()->quantiteMAX(bonus::bombe);
			}

			// Puissance de flamme
			static unsigned char PuissFlamme = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( PuissFlamme != p->joueur(1)->armements()->quantiteMAX(bonus::puissance_flamme) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::puissance_flamme) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PuissFlamme = p->joueur(1)->armements()->quantiteMAX(bonus::puissance_flamme);
			}

			// D�clancheur
			static unsigned char Declancheur = 255;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( Declancheur != p->joueur(1)->armements()->quantiteMAX(bonus::declancheur) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::declancheur) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;



				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				Declancheur = p->joueur(1)->armements()->quantiteMAX(bonus::declancheur);
			}

			// Vie
			static unsigned char nbVie = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( nbVie != p->joueur(1)->armements()->quantiteMAX(bonus::vie) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::vie) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbVie = p->joueur(1)->armements()->quantiteMAX(bonus::vie);
			}

			// Pousse Bombe
			static unsigned char PousseBombe = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( PousseBombe != p->joueur(1)->armements()->quantiteMAX(bonus::pousse_bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::pousse_bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PousseBombe = p->joueur(1)->armements()->quantiteMAX(bonus::pousse_bombe);
			}
		}
		cleanPlayer2 = 1;
	}else{
		if( cleanPlayer2 ){
			// Blit background
			imgRect.x = l_map->X()*32+xpos+10;
			imgRect.y = l_map->Y()*32-(25+35*4);
			imgRect.w = c_Instance->c_ecranGeneral->w-(l_map->X()*32+xpos);
			imgRect.h = 25+35*6;
			pos.x = imgRect.x;
			pos.y = imgRect.y;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */
			SDL_UpdateRect(c_Instance->c_ecranGeneral, imgRect.x, imgRect.y, imgRect.w, imgRect.h);
			cleanPlayer2 = 0;
		}
	}

	/***************************************************************************
	* Affichage du joueur 3
	*/
	static bool cleanPlayer3=0;
	if( p->nbJoueurs() > 2 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(2)->isLocal() || ( !p->joueur(2)->isLocal() && p->joueur(2)->socket() != INVALID_SOCKET )))) ){
		refresh = p->joueur(2)->need_refresh();
		// Affichage du nom du joueur 3
		pos.y = ypos;
		pos.x = l_map->X()*32+xpos+10;
		if( refresh || c_Instance->c_premierAffichage ){
			// Affichage du nom du joueur 3
			sfr_tmp = c_Instance->ecritTexte(p->joueur(2)->nom()->c_str(), c_mainColor, 55);

			// On remplis le fond
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			imgRect.w = c_Instance->c_ecranGeneral->w-(xpos+l_map->X()*32+10);
			imgRect.h = sfr_tmp->h;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */
			if( (unsigned int)pos.x+(unsigned int)sfr_tmp->w > imgRect.w ){
				imgRect.x = 0;
				imgRect.y = 0;
				SDL_BlitSurface(sfr_tmp, &imgRect, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}else{
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			SDL_FreeSurface(sfr_tmp);
		}
		pos.y += 25;// Espacement
		if( p->joueur(2)->armements() ){
			// Background
			imgRect.w = xpos-40;// 40 (espacement bonus)
			imgRect.h = 32;

			// Bombe
			static unsigned char nbBombe = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( nbBombe != p->joueur(2)->armements()->quantiteMAX(bonus::bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, c_mainColor, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbBombe = p->joueur(2)->armements()->quantiteMAX(bonus::bombe);
			}

			// Puissance de flamme
			static unsigned char PuissFlamme = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( PuissFlamme != p->joueur(2)->armements()->quantiteMAX(bonus::puissance_flamme) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::puissance_flamme) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PuissFlamme = p->joueur(2)->armements()->quantiteMAX(bonus::puissance_flamme);
			}

			// D�clancheur
			static unsigned char Declancheur = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( Declancheur != p->joueur(2)->armements()->quantiteMAX(bonus::declancheur) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::declancheur) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				Declancheur = p->joueur(2)->armements()->quantiteMAX(bonus::declancheur);
			}

			// Vie
			static unsigned char nbVie = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( nbVie != p->joueur(2)->armements()->quantiteMAX(bonus::vie) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::vie) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbVie = p->joueur(2)->armements()->quantiteMAX(bonus::vie);
			}

			// Pousse Bombe
			static unsigned char PousseBombe = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( PousseBombe != p->joueur(2)->armements()->quantiteMAX(bonus::pousse_bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::pousse_bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */

				SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PousseBombe = p->joueur(2)->armements()->quantiteMAX(bonus::pousse_bombe);
			}
		}
		cleanPlayer3 = 1;
	}else{
		if( cleanPlayer3 ){
			// Blit background
			imgRect.x = l_map->X()*32+xpos+10;
			imgRect.y = ypos+15;
			imgRect.w = c_Instance->c_ecranGeneral->w-(l_map->X()*32+xpos);
			imgRect.h = 25+35*5;
			pos.x = imgRect.x;
			pos.y = imgRect.y;
			SDL_BlitSurface(c_Instance->c_backgroundLayer, &imgRect, c_Instance->c_ecranGeneral, &pos); /* blittage du background */
			SDL_UpdateRect(c_Instance->c_ecranGeneral, imgRect.x, imgRect.y, imgRect.w, imgRect.h);
			cleanPlayer3 = 0;
		}
	}


	if( c_Instance->c_premierAffichage )
		c_Instance->c_premierAffichage = 0;

	SDL_Event event;
	SDL_PollEvent(&event);
	if( event.type == SDL_KEYDOWN )
		return traductionClavier(&event.key);
	else
		return SDLK_UNKNOWN;
}


/***************************************************************************//*!
* @fn SDLKey moteur_sdl::traductionClavier( const SDL_KeyboardEvent* touche )
* @brief Permet l'utilisation d'un clavier unicode en toute simplicit�
* @param[in] touche L'event: event.key
* @return La touche actuellement appuy�
*
* Exemple:
* @code
* SDL_Event event;
* SDL_WaitEvent(&event);
* switch( traductionClavier(&event.key) )
* {
*	case SDLK_LEFT:
*		break;
*	case SDLK_a:
*		break;
* }
* @endcode
*/
SDLKey moteur_sdl::traductionClavier( const SDL_KeyboardEvent* touche )
{
	if( touche->keysym.unicode != SDLK_UNKNOWN ){
		return (SDLKey)touche->keysym.unicode;
	}else{
		return (SDLKey)touche->keysym.sym;
	}
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::chargerImage( const char image[] ) const
* @brief Permet de charger une image.
* @param[in] image L'image a charger
* @return Une surface (contenant l'image) a lib�rer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::chargerImage( const char image[] ) const
{
	SDL_Surface* tmp = IMG_Load(image);
	if( !tmp )
		stdErrorE("Erreur lors du chargement de l'image <%s> : %s", image, SDL_GetError());
	return tmp;
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[] ) const
* @brief Permet d'�crire du texte avec la couleur par d�faut <b>partie::c_mainColor</b>
* @param[in] texte Le texte a �crire avec la couleur par d�faut <b>partie::c_mainColor</b>
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*
* Cette fonction est un alias de moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
* @see moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
*/
inline SDL_Surface* moteur_sdl::ecritTexte( const char texte[] ) const
{
	return ecritTexte( texte, c_mainColor );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b ) const
* @brief Permet d'�crire du texte dans une couleur RBG
* @param[in] texte Le texte a �crire
* @param[in] r,g,b Le taux d'utilisation de chaque couleurs ( de 0 � 255 )
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*
* Cette fonction est un alias de moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
* @see moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b ) const
{
	SDL_Color couleur = {r, g, b, 0/*unused*/};
	return ecritTexte( texte, couleur );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur ) const
* @brief Permet d'�crire du texte dans une couleur
* @param[in] texte Le texte a �crire
* @param[in] couleur La couleur du texte
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur ) const
{
	SDL_Surface* tmp = TTF_RenderText_Blended(c_policeGeneral, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la cr�ation du texte <%s>, Couleur: {%u, %u, %u}, Erreur renvoy�e: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

	return tmp;
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille ) const
* @brief Permet d'�crire du texte dans une couleur avec une certaine taille de police
* @param[in] texte Le texte a �crire
* @param[in] couleur La couleur a utiliser
* @param[in] taille La taille du texte
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille ) const
{
	if( texte[0] == 0 || !&couleur )
		stdErrorE("Param�tres incorrect ! texte[0]=%d, couleur=%p", texte[0], &couleur);

	TTF_Font* police = TTF_OpenFont("Chicken Butt.ttf", taille);
	if( !police )
		stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

	SDL_Surface* tmp = TTF_RenderText_Blended(police, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la cr�ation du texte <%s>, Couleur: {%u, %u, %u}, Erreur renvoy�e: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

	TTF_CloseFont(police);/* Fermeture de la police */

	return tmp;
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], TTF_Font* police ) const
* @brief Permet d'�crire du texte avec la couleur par defaut <b>partie::c_mainColor</b>
* @param[in] texte		Le texte a �crire avec la couleur par defaut <b>partie::c_mainColor</b>
* @param[in] police		La police a utiliser
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*
* @note Cette fonction est un alias de moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur, TTF_Font* police )
*/
inline SDL_Surface* moteur_sdl::ecritTexte( const char texte[], TTF_Font* police ) const
{
	return ecritTexte( texte, police, c_mainColor );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur, TTF_Font* police ) const
* @brief Permet d'�crire du texte
* @param[in] texte		Le texte a �crire
* @param[in] police		La police a utiliser
* @param[in] couleur	La couleur a utiliser
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], TTF_Font* police, const SDL_Color& couleur ) const
{
	if( texte[0] == 0 || !police || !&couleur )
		stdErrorE("Param�tres incorrect ! texte[0]=%d, police=%p, couleur=%p", texte[0], police, &couleur);

	SDL_Surface* tmp = TTF_RenderText_Blended(police, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la cr�ation du texte <%s>, Couleur: {%u, %u, %u}, Erreur renvoy�e: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

	return tmp;
}


/***************************************************************************//*!
* @fn char* moteur_sdl::completerMot( char texte[], unsigned int taille )
* @brief Complete le texte avec le caract�re _ jusqu'a avoir un texte de longueur 20
* @param[in,out] texte Le texte a combler
* @param[in] taille Taille r�el du texte ( sans les _ )
* @return La variable : texte
*/
char* moteur_sdl::completerMot( char texte[], unsigned int taille )
{
	if( taille == 20 )
		return texte;

	for( unsigned int i=taille; i<20; i++ )
		texte[i] = '_';

	return texte;
}



/***************************************************************************//*!
* @fn int moteur_sdl::isInSpriteList( s_Coordonnees pos ) const
* @brief Complete le texte avec le caract�re _ jusqu'a avoir un texte de longueur 20
* @return Si trouv�, alors renvoie la position dans la liste. Sinon -1
*/
int moteur_sdl::isInSpriteList( s_Coordonnees pos ) const
{
	unsigned int i;
	for( i=0; i<c_ListSprite.size(); i++ )
	{
		if( coordonneeEgal(c_ListSprite.at(i).pos, pos) )
			return i;
	}
	return -1;
}


/***************************************************************************//*!
* @fn void moteur_sdl::joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos, unsigned char frame ) const
* @brief Blit le joueur {joueur} a la position {pos} avec l'orientation {ori}
* @param[in] ori L'orientation du perso
* @param[in] joueur L'e nom�ro du joueur a blitter [1-...]
* @param[in] pos La position o� blitter le perso
*/
void moteur_sdl::joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos, unsigned char frame ) const
{
	SDL_Rect imRect;
	imRect.x = 0;
	imRect.y = 0;
	imRect.w = 32;
	imRect.h = 32;
	switch( ori )
	{
		case perso::ORI_haut: {
			switch( joueur )
			{
				case 1:
					imRect.x = (frame%(c_Instance->c_Decor[joueur1_haut]->w/32))*32;
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_haut], &imRect, c_Instance->c_ecranGeneral, pos);
					return ;
				case 2:
					imRect.x = (frame%(c_Instance->c_Decor[joueur2_haut]->w/32))*32;
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_haut], &imRect, c_Instance->c_ecranGeneral, pos);
					return;
				case 3:
					SDL_BlitSurface(c_Instance->c_Decor[joueur3_haut], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 4:
					SDL_BlitSurface(c_Instance->c_Decor[joueur4_haut], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				default:
					stdError("Cas(%d) NON gere !", (unsigned int)joueur);
			}
			return ;
		}

		case perso::ORI_bas: {
			switch( joueur )
			{
				case 1:
					imRect.x = (frame%(c_Instance->c_Decor[joueur1_bas]->w/32))*32;
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_bas], &imRect, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					imRect.x = (frame%(c_Instance->c_Decor[joueur2_bas]->w/32))*32;
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_bas], &imRect, c_Instance->c_ecranGeneral, pos);
					return;
				case 3:
					SDL_BlitSurface(c_Instance->c_Decor[joueur3_bas], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 4:
					SDL_BlitSurface(c_Instance->c_Decor[joueur4_bas], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				default:
					stdError("Cas(%d) NON gere !", (unsigned int)joueur);
			}
			return ;
		}

		case perso::ORI_droite: {
			switch( joueur )
			{
				case 1:
					imRect.x = (frame%(c_Instance->c_Decor[joueur1_droite]->w/32))*32;
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_droite], &imRect, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					imRect.x = (frame%(c_Instance->c_Decor[joueur2_droite]->w/32))*32;
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_droite], &imRect, c_Instance->c_ecranGeneral, pos);
					return;
				case 3:
					SDL_BlitSurface(c_Instance->c_Decor[joueur3_droite], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 4:
					SDL_BlitSurface(c_Instance->c_Decor[joueur4_droite], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				default:
					stdError("Cas(%d) NON gere !", (unsigned int)joueur);
			}
			return ;
		}

		case perso::ORI_gauche: {
			switch( joueur )
			{
				case 1:
					imRect.x = (frame%(c_Instance->c_Decor[joueur1_gauche]->w/32))*32;
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_gauche], &imRect, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					imRect.x = (frame%(c_Instance->c_Decor[joueur2_gauche]->w/32))*32;
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_gauche], &imRect, c_Instance->c_ecranGeneral, pos);
					return;
				case 3:
					SDL_BlitSurface(c_Instance->c_Decor[joueur3_gauche], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 4:
					SDL_BlitSurface(c_Instance->c_Decor[joueur4_gauche], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				default:
					stdError("Cas(%d) NON gere !", (unsigned int)joueur);
			}
			return ;
		}

		default:
			stdError("Cas NON gere !");
			return ;
	}
}
