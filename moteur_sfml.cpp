#include "moteur_sfml.h"
#include "debug.h"

moteur_sfml* moteur_sfml::c_Instance = 0;

#define chargerImage( Image, src ) if( !Image.LoadFromFile(src) ) stdErrorE("Erreur lors du chargement de l'image " src)

using namespace sf;

/***************************************************************************//*!
* @fn moteur_sfml::moteur_sfml()
* @brief Initialise la class moteur_sfml
*
* Initialise l'�cran sdl et pr�pare le "terrain" pour les traitements futurs
*/
moteur_sfml::moteur_sfml()
{
	c_App.Create(VideoMode(1024, 768, 32), "Bomberman");

	// 50 : Size of characters in bitmap - the bigger, the higher quality (30 by default)
	if( !c_policeGeneral.LoadFromFile("Chicken Butt.ttf", 50) )
		stdErrorE("Erreur lors du chargement de la police.")

	chargerImage(c_background, "images/background.png");

	/***************************************************************************
	* On charge ici le d�cor
	*/
	c_Decor = new Image[__nombre_de_decors__];

	//murs
	chargerImage(c_Decor[vide], "images/vide.png");
	chargerImage(c_Decor[mur_destructible], "images/mur_destructible.png");
	chargerImage(c_Decor[mur_indestructible], "images/mur_indestructible.png");
	//joueur1
	chargerImage(c_Decor[joueur1_haut], "images/bomberman1_haut.png");
	chargerImage(c_Decor[joueur1_bas], "images/bomberman1_bas.png");
	chargerImage(c_Decor[joueur1_gauche], "images/bomberman1_gauche.png");
	chargerImage(c_Decor[joueur1_droite], "images/bomberman1_droite.png");
	//joueur2
	chargerImage(c_Decor[joueur2_haut], "images/bomberman2_haut.png");
	chargerImage(c_Decor[joueur2_bas], "images/bomberman2_bas.png");
	chargerImage(c_Decor[joueur2_gauche], "images/bomberman2_gauche.png");
	chargerImage(c_Decor[joueur2_droite], "images/bomberman2_droite.png");
	//joueur3
	chargerImage(c_Decor[joueur3_haut], "images/bomberman3_haut.png");
	chargerImage(c_Decor[joueur3_bas], "images/bomberman3_bas.png");
	chargerImage(c_Decor[joueur3_gauche], "images/bomberman3_gauche.png");
	chargerImage(c_Decor[joueur3_droite], "images/bomberman3_droite.png");
	//joueur4
	chargerImage(c_Decor[joueur4_haut], "images/bomberman4_haut.png");
	chargerImage(c_Decor[joueur4_bas], "images/bomberman4_bas.png");
	chargerImage(c_Decor[joueur4_gauche], "images/bomberman4_gauche.png");
	chargerImage(c_Decor[joueur4_droite], "images/bomberman4_droite.png");
	//armes
	chargerImage(c_Decor[flamme_origine], "images/flamme_origine.png");
	chargerImage(c_Decor[flamme_vertical], "images/milieu_flamme_verticale.png");
	chargerImage(c_Decor[flamme_horizontal], "images/milieu_flamme_horizontale.png");
	chargerImage(c_Decor[bout_flamme_haut], "images/flamme_haut.png");
	chargerImage(c_Decor[bout_flamme_bas], "images/flamme_bas.png");
	chargerImage(c_Decor[bout_flamme_gauche], "images/flamme_gauche.png");
	chargerImage(c_Decor[bout_flamme_droite], "images/flamme_droite.png");
	chargerImage(c_Decor[bombe], "images/bombe.gif");
	// Bonus
	chargerImage(c_Decor[gain_bombe], "images/gain_bombe.png");
	chargerImage(c_Decor[gain_declancheur], "images/detonateur.png");
	chargerImage(c_Decor[gain_puissance_flamme], "images/gain_puissance_flamme.png");
	chargerImage(c_Decor[gain_pousse_bombe], "images/kick.png");
	chargerImage(c_Decor[gain_vie], "images/gain_vie.gif");
}


/***************************************************************************//*!
* @fn moteur_sfml& moteur_sfml::getInstance()
* @brief Renvoie l'instance de la class. Si pas d'instance, alors cr�ation d'une instance
*/
moteur_sfml& moteur_sfml::getInstance()
{
	if( !c_Instance )
		c_Instance = new moteur_sfml;

	return *c_Instance;
}


/***************************************************************************//*!
* @fn moteur_sfml::~moteur_sfml()
* @brief D�sinitialise la class moteur_sfml
*/
moteur_sfml::~moteur_sfml()
{
	delete[] c_Decor;
}


/***************************************************************************//*!
* @fn unsigned int moteur_sfml::menu( const char titre[], const char *choix[], unsigned int nb_choix )
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
unsigned int moteur_sfml::menu( const char titre[], const char *choix[], unsigned int nb_choix )
{
	unsigned int highLight = 1;
	bool continuer = 1;
	SDL_Event event;
	SDL_Color couleurOrange = {255, 184, 0, 0/*unused*/};
	SDL_Rect position;
	SDL_Surface* sfr_titre=0;
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
		TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_UNDERLINE);
		sfr_titre = ecritTexte(titre);
		TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_NORMAL);
	}

	while( continuer )
	{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = nb_choix;// On veux quitter !
				continuer = 0;
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

			position.y = 200;// Point de d�part
			if( sfr_titre ){
				position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
				SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background
			}
			// On blit le texte
			for( i=0; i<nb_choix; i++ )
			{
				position.y+=100;// Espacement entre les textes ( blit )
				position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
				if( i == highLight-1 ){
					SDL_BlitSurface(textes[i+nb_choix], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}else{
					SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}
	}

	for( i=0; i<nb_choix*2; i++ )// On lib�re les textes
		SDL_FreeSurface(textes[i]);
	delete[] textes;// On lib�re les textes

	SDL_FreeSurface(sfr_titre);

	return highLight;
}


/***************************************************************************//*!
* @fn void moteur_sfml::afficherConfigurationClavier( unsigned char joueur )
* @brief Menu permettant d'afficher et de modifier la configuration du clavier d'un joueur
* @param[in] joueur		Le num�ro du joueur ( de 1 � ... )
*/
void moteur_sfml::afficherConfigurationClavier( unsigned char joueur )
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

	SDL_Color couleurOrange = {255, 184, 0, 0/*unused*/};
	SDL_Color couleurRouge = {255, 0, 0, 0/*unused*/};

	// Cr�ation titre
	SDL_Surface* sfr_titre;
	char tempTexte[] = "Configuration clavier du joueur 0";// Ne pas oublier le '\0' !
	tempTexte[strlen(tempTexte)-1] += joueur;
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_UNDERLINE);
	sfr_titre = ecritTexte(tempTexte);
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_NORMAL);

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

	sfr_HAUT[0] = ecritTexte("Haut");
	sfr_HAUT[1] = ecritTexte("Haut", couleurOrange);
	sfr_HAUT_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
	sfr_HAUT_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
	sfr_BAS[0] = ecritTexte("Bas");
	sfr_BAS[1] = ecritTexte("Bas", couleurOrange);
	sfr_BAS_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ));
	sfr_BAS_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ), couleurOrange);
	sfr_DROITE[0] = ecritTexte("Droite");
	sfr_DROITE[1] = ecritTexte("Droite", couleurOrange);
	sfr_DROITE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ));
	sfr_DROITE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ), couleurOrange);
	sfr_GAUCHE[0] = ecritTexte("Gauche");
	sfr_GAUCHE[1] = ecritTexte("Gauche", couleurOrange);
	sfr_GAUCHE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ));
	sfr_GAUCHE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ), couleurOrange);
	sfr_LANCER_BOMBE[0] = ecritTexte("Lancer bombe");
	sfr_LANCER_BOMBE[1] = ecritTexte("Lancer bombe", couleurOrange);
	sfr_LANCER_BOMBE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ));
	sfr_LANCER_BOMBE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ), couleurOrange);
	sfr_DETONATEUR[0] = ecritTexte("D�tonateur");
	sfr_DETONATEUR[1] = ecritTexte("D�tonateur", couleurOrange);
	sfr_DETONATEUR_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ));
	sfr_DETONATEUR_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ), couleurOrange);

	do{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: { /*	si on clique sur la croix rouge de la fen�tre -> on sort et highLight */
				highLight = 7; /*	passe � 7 (valeur de sortie) */
				continuer = 0;
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
					case SDLK_UP:{ /* fl�che du haut */
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
										sfr_BAS_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
										sfr_BAS_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
										break;
									case 3:
										SDL_FreeSurface(sfr_DROITE_touche[0]);
										SDL_FreeSurface(sfr_DROITE_touche[1]);
										sfr_DROITE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
										sfr_DROITE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
										break;
									case 4:
										SDL_FreeSurface(sfr_GAUCHE_touche[0]);
										SDL_FreeSurface(sfr_GAUCHE_touche[1]);
										sfr_GAUCHE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
										sfr_GAUCHE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
										break;
									case 5:
										SDL_FreeSurface(sfr_LANCER_BOMBE_touche[0]);
										SDL_FreeSurface(sfr_LANCER_BOMBE_touche[1]);
										sfr_LANCER_BOMBE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
										sfr_LANCER_BOMBE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
										break;
									case 6:
										SDL_FreeSurface(sfr_DETONATEUR_touche[0]);
										SDL_FreeSurface(sfr_DETONATEUR_touche[1]);
										sfr_DETONATEUR_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
										sfr_DETONATEUR_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
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
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

			position.y = 200;
			position.x = (c_ecranGeneral->w-sfr_titre->w)/2; /* centrage */
			SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position); /* blittage du titre */


			position.x = 100;
			position_droite.x = c_ecranGeneral->w-250;

			position.y = 300;//+60
			position_droite.y = position.y;
			if( highLight == 1 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_HAUT[1], NULL, c_ecranGeneral, &position);
				SDL_BlitSurface(sfr_HAUT_touche[1], NULL, c_ecranGeneral, &position_droite); /* Blit du texte par-dessus */
			}else{
				SDL_BlitSurface(sfr_HAUT[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_HAUT_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 2 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_BAS[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_BAS_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_BAS[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_BAS_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 3 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_DROITE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DROITE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_DROITE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DROITE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 4 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_GAUCHE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_GAUCHE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_GAUCHE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_GAUCHE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 5 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_LANCER_BOMBE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_LANCER_BOMBE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_LANCER_BOMBE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_LANCER_BOMBE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 6 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> �gaux */
				SDL_BlitSurface(sfr_DETONATEUR[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DETONATEUR_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_DETONATEUR[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DETONATEUR_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.x = (c_ecranGeneral->w-sfr_retour[0]->w)/2;
			position.y += 60;
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
}


/***************************************************************************//*!
* @fn int moteur_sfml::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour )
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
int moteur_sfml::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour )
{
	unsigned int highLight = 2;
	bool continuer = 1;
	SDL_Event event;
	SDL_Color couleurOrange = {255, 184, 0, 0/*unused*/};
	SDL_Color couleurRouge = {255, 0, 0, 0/*unused*/};
	SDL_Rect position;
	SDL_Surface* sfr_titre = 0;
	SDL_Surface* sfr_msg = 0;
	SDL_Surface** textes;
	unsigned int i;
	bool dessiner = 1;
	char valeurRetourTexte[20];// <- Valable 32bit ONLY ! Calcule :: lenght(2^(sizeof(int)*8))+4*2

	if( !(valeurMin <= valeurParDefaut && valeurParDefaut <= valeurMax) )
		stdErrorE("Valeur incorrect ! valeurMin(%d) <= valeurParDefaut(%d) && valeurParDefaut(%d) <= valeurMax(%d)", valeurMin, valeurParDefaut, valeurParDefaut, valeurMax);

	*valeurRetour = valeurParDefaut;
	sprintf(valeurRetourTexte, "<-- %d -->", *valeurRetour);

	textes = new SDL_Surface*[3*2];

	/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
	textes[0] = ecritTexte(valeurRetourTexte);
	textes[1] = ecritTexte("Ok");
	textes[2] = ecritTexte("Retour");

	// Cr�ation du texte HighLight
	textes[3] = ecritTexte(valeurRetourTexte, couleurOrange);
	textes[4] = ecritTexte("Ok", couleurOrange);
	textes[5] = ecritTexte("Retour", couleurOrange);

	// Cr�ation du titre
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_UNDERLINE);
	sfr_titre = ecritTexte(titre);
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_NORMAL);


	while( continuer )
	{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = 3;// On veux quitter !
				continuer = 0;
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
					case SDLK_RETURN: {// On a valider notre choix par entrer
						if( highLight == 1 ){// On a selectionn� un lien de modification de texte
							dessiner = 1;
							sfr_msg = ecritTexte("Entrez le nombre manuellement", couleurRouge);
							do{
								SDL_WaitEvent(&event);
								if( event.type == SDL_QUIT ){
									continuer = 0;
									highLight = 3;
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
									case SDLK_BACKSPACE:{
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
									case SDLK_RETURN: {
										if( !(valeurMin <= *valeurRetour && *valeurRetour <= valeurMax) )
											*valeurRetour = valeurMin;
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

									position.y = 200;
									position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
									SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background

									// On blit le texte
									for( i=0; i<3; i++ )
									{
										position.y+=100;// Espacement entre les textes ( blit )
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
						highLight = 3;// On veux quitter !
						continuer = 0;
						break;
					}
					default:
						break;
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

			position.y = 200;// Point de d�part
			position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
			SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background

			// On blit le texte
			for( i=0; i<3; i++ )
			{
				position.y+=100;// Espacement entre les textes ( blit )
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
	delete[] textes;// On lib�re les textes

	SDL_FreeSurface(sfr_titre);

	return highLight;
}


/***************************************************************************//*!
* @fn int moteur_sfml::getTexte( const char titre[], char texteRetour[21] )
* @brief Permet d'obtenir du texte
* @param[in] titre				Le titre du menu
* @param[out] texteRetour		Dans cette variable sera stock�, le texte obtenu a la fin de la fonction
* @return
*	- 2 : Texte valid� et acc�pt�
*	- 3 : Action annul�e
*/
int moteur_sfml::getTexte( const char titre[], char texteRetour[21] )
{
	unsigned int highLight = 1;
	bool continuer = 1;
	SDL_Event event;
	SDL_Color couleurOrange = {255, 184, 0, 0/*unused*/};
	SDL_Color couleurRouge = {255, 0, 0, 0/*unused*/};
	SDL_Rect position;
	SDL_Surface* sfr_titre=0;
	SDL_Surface* sfr_msg = 0;
	SDL_Surface** textes;
	unsigned int tailleTexteRetour;
	unsigned int i;
	bool dessiner = 1;

	tailleTexteRetour = strlen(texteRetour);

	textes = new SDL_Surface*[3*2];

	// Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal)
	textes[0] = ecritTexte(completerMot(texteRetour, tailleTexteRetour));
	textes[1] = ecritTexte("Ok");
	textes[2] = ecritTexte("Retour");

	// Cr�ation du texte HighLight
	textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
	textes[4] = ecritTexte("Ok", couleurOrange);
	textes[5] = ecritTexte("Retour", couleurOrange);

	// Cr�ation du titre
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_UNDERLINE);
	sfr_titre = ecritTexte(titre);
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_NORMAL);

	while( continuer )
	{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = 3;// On veux quitter !
				continuer = 0;
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
					case SDLK_RETURN: {// On a valider notre choix par entrer
						if( highLight == 1 ){// On a selectionn� un lien de modification de texte
							dessiner = 1;
							sfr_msg = ecritTexte("Entrez le texte manuellement", couleurRouge);
							textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
							do{
								SDL_WaitEvent(&event);
								if( event.type == SDL_QUIT ){
									continuer = 0;
									highLight = 3;
								}

								if( event.type == SDL_KEYDOWN )
								switch( traductionClavier(&event.key) )
								{
									case SDLK_BACKSPACE:{
										if( tailleTexteRetour == 0 )
											break;
										tailleTexteRetour--;
										texteRetour[tailleTexteRetour] = '_';
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_ESCAPE: {
										continuer = 0;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_RETURN: {
										continuer = 0;
										break;
									}
									default: {
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
										break;
									}
								}

								/***********************************************************************
								* Blitage g�n�ral
								* > On blit uniquement s'il y a eu une modification
								*/
								if( dessiner ){
									SDL_FreeSurface(textes[0]);
									SDL_FreeSurface(textes[3]);
									textes[0] = ecritTexte(texteRetour);
									textes[3] = ecritTexte(texteRetour, couleurRouge);

									position.x = 0;
									position.y = 0;
									SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

									position.y = 200;
									position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
									SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background

									// On blit le texte
									for( i=0; i<3; i++ )
									{
										position.y+=100;// Espacement entre les textes ( blit )
										position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
										if( i == highLight-1 ){
											SDL_BlitSurface(textes[i+3], NULL, c_ecranGeneral, &position); // Blit du texte par-dessus
										}else{
											SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); // Blit du texte par-dessus
										}
									}

									SDL_Flip(c_ecranGeneral);
									dessiner = 0;
								}
							}while( continuer );
							SDL_FreeSurface(sfr_msg);
							sfr_msg = 0;
							SDL_FreeSurface(textes[0]);
							SDL_FreeSurface(textes[3]);
							// On vire les espaces
							if( tailleTexteRetour ){
								texteRetour[tailleTexteRetour] = 0;
								trimString(texteRetour);
								tailleTexteRetour = strlen(texteRetour);
							}
							if( tailleTexteRetour ){
								textes[0] = ecritTexte(texteRetour);
								textes[3] = ecritTexte(texteRetour, couleurOrange);
								highLight = 2;
							}else{
								textes[0] = ecritTexte(completerMot(texteRetour, tailleTexteRetour));
								textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
							}
							dessiner = 1;
							continuer = 1;
						}else{
							if( highLight == 2 ){
								if( tailleTexteRetour ){
									continuer = 0;// On sort de la boucle
								}else{
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
						highLight = 3;// On veux quitter !
						continuer = 0;
						break;
					}
					default:
						break;
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

			position.y = 200;
			if( sfr_msg ){
				position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
				SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background
			}else{
				position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
				SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background
			}
			// On blit le texte
			for( i=0; i<3; i++ )
			{
				position.y+=100;// Espacement entre les textes ( blit )
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
	delete[] textes;// On lib�re les textes

	SDL_FreeSurface(sfr_titre);

	if( sfr_msg )
		SDL_FreeSurface(sfr_msg);

	return highLight;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER moteur_sfml::afficherMapEtEvent( const partie* p )
* @brief Affiche une map
* @param[in] p	La partie en cours a afficher
* @return La touche actuellement appuy�
*/
SYS_CLAVIER moteur_sfml::afficherMapEtEvent( const partie* p )
{
	if( !c_Instance )
		stdErrorE("Veuillez instancier la class moteur_sfml !");

	bool dessiner = 0;
	map* l_map = p->getMap();// l_map pour local variable map
	// D�calage
	static unsigned int	xpos=(((c_Instance->c_ecranGeneral->w/32)-l_map->X())/2)*32,
						ypos=(((c_Instance->c_ecranGeneral->h/32)-l_map->Y())/2)*32;
	SDL_Rect pos;

	// Tester ici si la taille de la map n'est pas trop grande pour la fenetre.
	if ( l_map->X() > (unsigned int)c_Instance->c_ecranGeneral->w/32 || l_map->Y() > (unsigned int)c_Instance->c_ecranGeneral->h/32 )
		stdErrorE("%u > %d || %u > %d", l_map->X(), c_Instance->c_ecranGeneral->w/32, l_map->Y(), c_Instance->c_ecranGeneral->h/32);

	if( c_Instance->c_premierAffichage ){
		// Traitement a faire sur les cadres ICI
		// A FAIRE

		for( unsigned int x,y=0; y<l_map->Y(); y++ )
		{
			for( x=0; x<l_map->X(); x++ )
			{
				pos.x=x*32+xpos;
				pos.y=y*32+ypos;
				switch( l_map->getBlock(x,y)->element )
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
					case map::UN_joueur:
					case map::plusieurs_joueurs: {
						if( !l_map->getBlock(x,y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y)->joueur=0", x, y);

						unsigned char joueur = l_map->getBlock(x,y)->joueur->at(0);// renvoie un num�ro de joueur [1-...]

						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
						break;
					}
					case map::bombe_poser: {
						//couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[bombe], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::bombe_poser_AVEC_UN_joueur: {
						if( !l_map->getBlock(x,y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y).joueur=0", x, y);

						unsigned char joueur = l_map->getBlock(x,y)->joueur->at(0);// renvoie un num�ro de joueur [1-...]

						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[bombe], NULL, c_Instance->c_ecranGeneral, &pos);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );

						break;
					}
					case map::flamme_origine: {
						SDL_BlitSurface(c_Instance->c_Decor[flamme_origine], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_horizontal:{
							SDL_BlitSurface(c_Instance->c_Decor[flamme_horizontal], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
					}
					case map::flamme_vertical: {
						SDL_BlitSurface(c_Instance->c_Decor[flamme_vertical], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_haut: {
						SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_haut], NULL, c_Instance->c_ecranGeneral, &pos);
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
					default: {
						stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(x,y)->element);
						break;
					}
				}
			}
		}
		c_Instance->c_premierAffichage = false;
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
				case map::UN_joueur:
				case map::plusieurs_joueurs: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					unsigned char joueur = l_map->getBlock(v_pos)->joueur->at(0);

					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
					break;
				}
				case map::bombe_poser: {
					//couleur = getCouleurJoueur( l_map->getBlock(v_pos)->joueur->at(0) );
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::bombe_poser_AVEC_UN_joueur: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos).joueur=0", v_pos.x, v_pos.y);

					unsigned char joueur = l_map->getBlock(v_pos)->joueur->at(0);

					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], NULL, c_Instance->c_ecranGeneral, &pos);
					c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );

					break;
				}
				case map::flamme_origine: {
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[flamme_origine], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_horizontal:{
						SDL_BlitSurface(c_Instance->c_Decor[flamme_horizontal], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
				}
				case map::flamme_vertical: {
					SDL_BlitSurface(c_Instance->c_Decor[flamme_vertical], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_haut: {
					SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_haut], NULL, c_Instance->c_ecranGeneral, &pos);
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
				default: {
					stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(v_pos)->element);
					break;
				}
			}
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-32, pos.y-32, 32*3, 32*3);
		}
	}

	SDL_Event event;
	SDL_PollEvent(&event);
	if( event.type == SDL_KEYDOWN )
		return traductionClavier(&event.key);
	else
		return SDLK_UNKNOWN;
}


/***************************************************************************//*!
* @fn SDLKey moteur_sfml::traductionClavier( const SDL_KeyboardEvent* touche )
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
SDLKey moteur_sfml::traductionClavier( const SDL_KeyboardEvent* touche )
{
	if( touche->keysym.unicode != SDLK_UNKNOWN ){
		return (SDLKey)touche->keysym.unicode;
	}else{
		return (SDLKey)touche->keysym.sym;
	}
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sfml::ecritTexte( const char texte[] )
* @brief Permet d'�crire du texte en <b>NOIR</b>
* @param[in] texte Le texte a �crire en noir
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*
* Cette fonction est un alias de moteur_sfml::ecritTexte( const char texte[], const SDL_Color* couleur )
* @see moteur_sfml::ecritTexte( const char texte[], const SDL_Color* couleur )
*/
SDL_Surface* moteur_sfml::ecritTexte( const char texte[] )
{
	static SDL_Color couleurNoire = {0,0,0,0/*unused*/};
	return ecritTexte( texte, couleurNoire );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sfml::ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b )
* @brief Permet d'�crire du texte dans une couleur RBG
* @param[in] texte Le texte a �crire
* @param[in] r,g,b Le taux d'utilisation de chaque couleurs ( de 0 � 255 )
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*
* Cette fonction est un alias de moteur_sfml::ecritTexte( const char texte[], const SDL_Color* couleur )
* @see moteur_sfml::ecritTexte( const char texte[], const SDL_Color* couleur )
*/
SDL_Surface* moteur_sfml::ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b )
{
	SDL_Color couleur = {r, g, b, 0/*unused*/};
	return ecritTexte( texte, couleur );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sfml::ecritTexte( const char texte[], const SDL_Color& couleur )
* @brief Permet d'�crire du texte dans une couleur
* @param[in] texte Le texte a �crire
* @param[in] couleur La couleur du texte
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sfml::ecritTexte( const char texte[], const SDL_Color& couleur )
{
	SDL_Surface* tmp = TTF_RenderText_Blended(c_policeGeneral, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la cr�ation du texte <%s>, Couleur:{%u, %u, %u}, Erreur renvoy�e: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

	return tmp;
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sfml::ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille )
* @brief Permet d'�crire du texte dans une couleur avec une certaine taille de police
* @param[in] texte Le texte a �crire
* @param[in] couleur La couleur a utiliser
* @param[in] taille La taille du texte
* @return Une surface (contenant le texte) a lib�rer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sfml::ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille )
{
	TTF_Font* police = TTF_OpenFont("Chicken Butt.ttf", taille);
	if( !police )
		stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

	SDL_Surface* tmp = TTF_RenderText_Blended(police, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la cr�ation du texte <%s>, Couleur:{%u, %u, %u}, Erreur renvoy�e: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

	TTF_CloseFont(police);/* Fermeture de la police */

	return tmp;
}


/***************************************************************************//*!
* @fn char* moteur_sfml::completerMot( char texte[], unsigned int taille )
* @brief Complete le texte avec le caract�re _ jusqu'a avoir un texte de longueur 20
* @param[in,out] texte Le texte a combler
* @param[in] taille Taille r�el du texte ( sans les _ )
* @return La variable : texte
*/
char* moteur_sfml::completerMot( char texte[], unsigned int taille )
{
	if( taille == 20 )
		return texte;

	for( unsigned int i=taille; i<20; i++ )
	{
		texte[i] = '_';
	}
	return texte;
}



/***************************************************************************//*!
* @fn void moteur_sfml::joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos ) const
* @brief Blit le joueur {joueur} a la position {pos} avec l'orientation {ori}
* @param[in] ori L'orientation du perso
* @param[in] joueur L'e nom�ro du joueur a blitter [1-...]
* @param[in] pos La position o� blitter le perso
*/
void moteur_sfml::joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos ) const
{
	switch( ori )
	{
		case perso::ORI_haut: {
			switch( joueur )
			{
				case 1:
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_haut], NULL, c_Instance->c_ecranGeneral, pos);
					return ;
				case 2:
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_haut], NULL, c_Instance->c_ecranGeneral, pos);
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
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_bas], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_bas], NULL, c_Instance->c_ecranGeneral, pos);
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
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_droite], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_droite], NULL, c_Instance->c_ecranGeneral, pos);
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
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_gauche], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_gauche], NULL, c_Instance->c_ecranGeneral, pos);
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
