#include "perso.h"
#include "clavier.h"

/*******************************************************************************
*
*/
perso::perso()
{
	c_est_vivant = 1;
}


/*******************************************************************************
*
*/
perso::perso(std::string nom, unsigned int Xpos, unsigned int Ypos)
{
	c_nom = nom;
	c_X_pos = Xpos;
	c_Y_pos = Ypos;
}


/*******************************************************************************
*
*/
perso::~perso()
{
	c_est_vivant = 0;
}


/*******************************************************************************
*
*/
void perso::defPos(unsigned int Xpos, unsigned int Ypos)
{
	c_X_pos = Xpos;
	c_Y_pos = Ypos;
}


/*******************************************************************************
*
*/
void perso::defVivant( bool v )
{
	c_est_vivant = v;
}


/*******************************************************************************
*
*/
unsigned int perso::X()
{
	return c_X_pos;
}


/*******************************************************************************
*
*/
unsigned int perso::Y()
{
	return c_Y_pos;
}


/*******************************************************************************
*
*/
bool perso::estVivant()
{
	return c_est_vivant;
}


/*******************************************************************************
*
*/
bool perso::estAppuyer( SYS_CLAVIER t )
{
	return c_Touches.estAppuyer( t );
}


