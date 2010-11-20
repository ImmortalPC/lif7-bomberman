#ifndef PARTIE_h
#define PARTIE_h

#include "options.h"
#include "perso.h"
#include "map.h"

extern options G_OPTIONS;// Variable globale

class partie
{
	private:
		/*!
		* @struct s_Event
		* @brief Permet de g�rer les event graphiques.
		*
		* Exemple: La dur�er d'une animation
		*/
		typedef struct {
			bonus::t_Bonus type;//!< Type de bonus
			s_Coordonnees pos;//!< Position Originel de l'event
			unsigned char joueur;//!< Le joueur qui est la cause de l'event
			unsigned char Nb_Repetition;//!< Nombre de r�p�tition actuel pour l'event
			clock_t repetionSuivante;//!< Time de la prochaine r�p�tion
		} s_Event;


	public:
		enum MODE {
			Combat_Libre,		// Free For All 	( F4A )
			Attrape_drapeau		// Capture the Flag	( CTF )
		};
		/*!
		* @typedef libAff
		* @brief Signature de la fonction a utiliser pour l'affichage de la map
		*
		* La fonction en question doit prendre en param�tre la partie actuel.<br />
		* Elle doit afficher la map.<br />
		* Puis elle doit renvoyer la touche actuellement appuy�e.
		*
		* @see partie::main(libAff * afficherMapEtEvent);
		*/
		typedef SYS_CLAVIER(libAff)(const partie*);


	private:
		// struct {
			map*					c_map;// SIMPLE POINTEUR !
			unsigned char			c_nb_joueurs;
			unsigned char			c_nb_MAX_joueurs;
			perso*					c_joueurs;// Tableau
			MODE					c_mode;
			std::vector<s_Event>	c_listEvent;
		// }

		void deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur );
		void checkInternalEvent();


	public:
		partie();
		~partie();
		// Modificateurs
		void genMap();
		void def_nbJoueurs( unsigned char nb );
		void def_nbMAX_joueurs( unsigned char nb );
		void def_modeJeu( MODE m );


		// Accesseurs
		unsigned char nbJoueurs() const;
		unsigned char nbMAX_joueurs() const;
		MODE modeJeu() const;
		perso* joueur( unsigned int joueur_numero ) const;
		map* getMap() const;
		unsigned char nbJoueurVivant() const;

		// Autres
		void main(libAff * afficherMapEtEvent);
};

#endif