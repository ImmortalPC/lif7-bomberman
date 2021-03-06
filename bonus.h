#ifndef BONUS_h
#define BONUS_h

#include <vector>
#include "coordonnees.h"
#include "outils.h"

/*!
* @class bonus
* @brief G�re les bonus.
*
* Cette classe poss�de 2 couches de fonctionnement:
* -# La couche g�n�ral, qui regroupe les bonus disponibles pour la partie.<br />
*	(En commum avec toutes les instances de cette class)<br />
*	<b>A initialiser en 1er !via bonus::bonusProp()</b><br />
*	> Cette couche contient les probabilit�s de chaque objets et la quantit� Maximum ramassables par joueur.
* -# La couche instanciable, unique a chaque instance.<br />
*	> Cette couche contient la quatit� max en stock et la quantit� utilisable.
*
* @section aide Aide sur les variables
* Mais c'est quoi la diff�rence entre: quantite_utilisable, quantite_MAX_en_stock, quantite_MAX_Ramassable, probabiliter_pop ?<br />
*
* Prenons un exemple:<br />
*	- Soit un perso bomberman.
*	- Le perso est capable de porter 10 bombes s'il y a le sac ad�quate. ( quantite_MAX_Ramassable = 10 )
*	- Notre perso a un sac pouvant contenir 5 bombes maximum. ( quantite_MAX_en_stock = 5 )
*	- Le sac contient actuellement 3 bombes.( quantite_utilisable = 3)
*	- J'ai donc 2 bombes pos�es. Ces 2 bombes sont dans la variable @link bonus::c_listEvent c_listEvent@endlink en attandant qu'elles explosent ^^
*/
class bonus
{
	public:
		/*!
		* @enum t_Bonus
		* @brief Les types de bonus possible dans le jeu
		*
		* @attention En cas de modification du nombre d'element, modifier aussi
		* le define NB_ELEMENT_t_Bonus
		*/
		enum t_Bonus {
			bombe=0,//!< A laisser en 1ere position ! NE PAS TOUCHER !
			super_bombe,
			puissance_flamme,
			super_puissance_flamme,
			declancheur,
			pousse_bombe,
			vie,
			teleporteur,
			inversseur_touche,
			force_explosion,

			NB_ELEMENT_t_Bonus,
			// Signal pour les event
			SIGNAL_kill,

			// NE RIEN METTRE APRES CET ELEMENT. NE PAS COMPTER CETTE ELEMENT
			__RIEN__//!< Sert pour bonus::getBonusAleatoire();
		};
		enum {
			VITESSE_flammes = 90*1000,// Vitesse de d�flagration en micro-sec ( NOTE 1 000 000 milli-sec = 1 sec, 1 milliseconde = 1000 microsecondes )
			VITESSE_pousseBombe = 50*1000
		};

		/*!
		* @struct s_bonus_proprieter
		* @brief Cette structure accueil la propri�t� des bonus
		*/
		typedef struct {
			/**
			* @var bonus::s_bonus_proprieter::duree
			* La dur�e peut avoir 3 status<br />
			* - -1 => Pas de callback
			* - +0 => Callback direct
			* - >0 => Callback au bout de X secs
			*/
			s_timeval duree;// Dur�e d'utilisation
			//t_Bonus type; // <- Le type est stock� dans le num�ro du tableau [0] -> [t_Bonus::bombe]
			unsigned char probabiliter_pop;//!< Pourcentage q'un bonus pop (valeur: entre 0 et 100 au max)
			unsigned char quantite_MAX_Ramassable;//!< Quantit� max qu'un joueur peut avoir d'un objet
		} s_bonus_proprieter;


		/*!
		* @struct s_Event
		* @brief Cette sturcture accueil les Events pos�es
		*
		* Exemple: C'est par elle que l'on sait quand une bombe explose.<br />
		* Elle fourni, le O�  et Quand.<br />
		* @note Passe -Wpadded pour 32bit (16o)
		*/
		typedef struct {
			s_Coordonnees pos;//!< Position de la bombe pos� (8o)
			s_timeval finEvent;//!< Temps avant la fin de l'event (4o)
			t_Bonus type;//!< Le bonus (4o)
		} s_Event;

	private:
		/*!
		* @struct s_bonus
		* @brief Cette sturcture accueil un bonus d'un joueur.
		*
		* ( un tableau de cette structure forme l'ensemble des bonus d'un joueur )
		* quantite_utilisable: Nombre d'objet poss�d� actuellement et utilisable
		* quantite_MAX_en_stock: Quantit� Maxi d'un objet ( ex: 5 bonus de type bombe => 5 Bombes max en stock ! )
		* @note Passe -Wpadded pour 32bit (4o)
		*/
		typedef struct {
			unsigned char quantite_utilisable;//!< Nombre d'objet poss�d� actuellement et utilisable (1o)
			unsigned char quantite_MAX_en_stock;//!< Quantit� Maxi d'un objet (1o)
		} s_bonus;

		// struct {
			std::vector<s_Event>		c_listEvent;//!< Tableau contenant les bombes pos�es (12o)
			// Varaible G�n�rale ( Variables global � toutes les class )
			static s_bonus_proprieter*	C_bonusProp;//!< Liste des bonus charg� et utilisables sur la map ( cette var n'est remplis q'une fois ! (static) )
			// Varaibles Local � la class ( � l'instance de chaque class )
			s_bonus						c_liste[NB_ELEMENT_t_Bonus];//!< Liste des bonus attrap� par un player. ( bonus utilisable )
		// }


	public:
		bonus();// Constructeur
		~bonus();// Destructeur
		static s_bonus_proprieter* bonusProp();// Constructeur pour C_bonusProp
		static void unInitBonusProp();// Destructeur pour inclut C_bonusProp

		// Accesseurs
		unsigned char quantiteUtilisable( t_Bonus b ) const;
		unsigned char quantiteMAX( t_Bonus b ) const;
		unsigned char quantiteMAX_Ramassable( t_Bonus b ) const;
		unsigned char probabiliter( t_Bonus b ) const;

		// Modificateurs
		bool incQuantiteUtilisable( t_Bonus b );// Incr�menter quantite_utilisable
		bool incQuantiteUtilisable_Event( t_Bonus b );// Incr�menter quantite_utilisable + activation du syst�me d'event
		bool incQuantiteMAX_en_stock( t_Bonus b );// Incr�menter quantite_MAX_en_stock
		bool incQuantiteMAX_en_stock_Event( t_Bonus b );// Incr�menter quantite_MAX_en_stock + activation du syst�me d'event
		bool decQuantiteUtilisable( t_Bonus b );// D�cr�menter quantite_utilisable
		bool decQuantiteUtilisable( t_Bonus b, unsigned int x, unsigned int y );
		inline bool decQuantiteUtilisable( t_Bonus b, s_Coordonnees pos );
		bool decQuantiteMAX_en_stock( t_Bonus b );// D�cr�menter quantite_MAX_en_stock
		void param_Par_Defaut();
		void defQuantiteUtilisable( t_Bonus b, unsigned char quantite_utilisable );
		void defQuantiteMAX( t_Bonus b, unsigned char quantite_MAX_en_stock );
		static void defQuantiteMAX_Ramassable( t_Bonus b, unsigned char quantite_MAX_Ramassable );
		static void defProbabiliter( t_Bonus b, unsigned char probabiliter_pop );
		void defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock );
		void defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop );
		inline std::vector<s_Event>* modEvent();
		s_Event* getEvent( unsigned int x, unsigned int y );
		void forceTimeOut();
		void forceTimeOut( unsigned int x, unsigned int y );
		void forceTimeOut( t_Bonus b );
		void kill();

		// Autres
		bool isEvent( s_Event* pos );
		inline unsigned int nbEvent() const;
		static t_Bonus getBonusAleatoire();
};

#include "bonus.inl"

#endif
