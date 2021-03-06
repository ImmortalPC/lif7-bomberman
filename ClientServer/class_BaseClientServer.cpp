#include "class_BaseClientServer.h"
#include "../debug.h"

IF_WINDOWS(
	WSADATA baseClientServer::c_wsaData;
)

// Permet de d�connecter tout les client, de d�charger Winsock de la m�moire, puis de rapporter une erreur + exit
#define reportError( ... ) {this->~baseClientServer(); stdErrorE( __VA_ARGS__ )}


/***************************************************************************//*!
* @fn baseClientServer::baseClientServer()
* @brief Constructeur par defaut
*
* - Si la platforme est windows => on charge Winsock en m�moire
* - Param�trage pas d�faut.
*/
baseClientServer::baseClientServer()
{
	IF_WINDOWS(
		// MAKEWORD(2, 2) := La version que l'on veux avoir (2.2)
		// D�marrage winsock
		int err = WSAStartup(MAKEWORD(2, 2), &c_wsaData);
		if( err )
			stdErrorE("Erreur(%d) lors du d�mmarage de WinSock", err);
		//stdError("Info sur le serveur: wVersion=%u, wHighVersion=%u, szDescription=%s, szSystemStatus=%s", c_wsaData.wVersion, c_wsaData.wHighVersion, c_wsaData.szDescription, c_wsaData.szSystemStatus);

		if( LOBYTE(c_wsaData.wVersion) != 2 || HIBYTE(c_wsaData.wVersion) != 2 )
			stdError("La version WinSock 2.2 n'est pas disponnible sur cette ordinateur.");
	);

	c_wait.tv_sec = 0;
	c_wait.tv_usec = 10000;// 0.01 secondes


	c_listenSocket = 0;
	c_port = 0;
}


/***************************************************************************//*!
* @fn baseClientServer::~baseClientServer()
* @brief Destructeur par defaut
*
* - Appel baseClientServer::endListen()
* - Si la platforme est windows => on d�charge Winsock de la m�moire
*/
baseClientServer::~baseClientServer()
{
	IF_WINDOWS(
		// Arr�t de WinSock
		if( WSACleanup() == SOCKET_ERROR )
			stdErrorE("Erreur(%d) lors de l'arr�t de WinSock", SOCKET_REPPORT_ERROR);
	)
}


/***************************************************************************//*!
* @fn void baseClientServer::setPort( unsigned int port )
* @brief Modifier le port d'�coute
* @param[in] port Le port a utiliser de 1 � 65 536
* @note Par d�faut, le port = 0 => port all�atoire.
*/
void baseClientServer::setPort( unsigned int port )
{
	if( port > 65536 )
		reportError("Vous ne pouvez pas indiquer un port > � 65 536");
	c_port = port;
}


/***************************************************************************//*!
* @fn void baseClientServer::setWait( long tv_sec, long tv_usec )
* @brief Modifier le temps d'attente d'une r�ponse
* @param[in] tv_sec Le temps en secs
* @param[in] tv_usec Le temps en ms
* @note Le temps d'attente par d�faut est de 0.01sec
*/
void baseClientServer::setWait( long tv_sec, long tv_usec )
{
	if( tv_sec < 0 || tv_usec < 0 )
		reportError("Temps d'attente incorrect !");

	c_wait.tv_sec = tv_sec;
	c_wait.tv_usec = tv_usec;
}


/***************************************************************************//*!
* @fn int baseClientServer::readClient( SOCKET sock, char* buffer, unsigned int bufferSize )
* @brief R�cup�re les donn�es envoy�es par un client
* @param[in] sock Le socket du client
* @param[in,out] buffer La variables o� seront stock�es les donn�es
* @param[in] bufferSize Taille du buffer
* @return Le nombre de byte re�ut
*
* @note si retour = 0 => Client d�connect� (Le client est automatiquement vir� de la liste dans ce cas la)<br />
* En cas d'erreur, le client sera d�connect�
*/
int baseClientServer::readClient( SOCKET sock, char* buffer, unsigned int bufferSize )
{
	int nbByte = recv(sock, buffer, bufferSize, 0);

	stdError("rcv %s", buffer);

	if( nbByte == SOCKET_ERROR ){
		stdError("Erreur(%d) lors de la lecture du client %d. Detail: <%s>", SOCKET_REPPORT_ERROR, sock, strerror(SOCKET_REPPORT_ERROR));
		nbByte = 0;// if recv error we disonnect the client
	}

	return nbByte;
}


/***************************************************************************//*!
* @fn void baseClientServer::send_message( SOCKET sock, const char* buffer, unsigned int bufferSize )
* @brief Envoie un message (buffer) au client
* @param[in] sock Le socket du client
* @param[in,out] buffer La variable sont stock�es les donn�es � envoyer
* @param[in] bufferSize Taille du buffer
*/
void baseClientServer::send_message( SOCKET sock, const char* buffer, unsigned int bufferSize )
{
	stdError("send %s", buffer);
	if( send(sock, buffer, bufferSize, 0) == SOCKET_ERROR )
	   stdError("Erreur(%d) lors de l'envoie du message. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));
}


//***************************************************************************//*!
//* @fn template<class Type> Type baseClientServer::reverseBit( Type var )
//* @brief Inverse les bits.
//* @param[in] var La variable a qui il faut invers� les bits
//* @return Renvoie la {var} avec les bits invers�
//* @warning FONCTION comment� car Innutile pour l'instant
//*/
/*
template<class Type> Type baseClientServer::reverseBit( Type var )// Inverse les bits
{
	const unsigned int nbByte = sizeof(Type)*CHAR_BIT;

	Type out = 0;
	unsigned int i=0;

	do{
		out|= var&0x01;		//copie du bit de poids faible
		i++;				//compter les bits copi�s
		if( i == nbByte)	//si tous les bits sont trait�s...
			break;			//...arr�ter
		var >>= 1;			//vider vers la droite (pr�parer le suivant)...
		out<<= 1;			//...pour remplir vers la gauche (pr�parer la place)
	}while(1);

	return out;
}
*/


/***************************************************************************//*!
* @fn template<class Type> Type baseClientServer::reverseByte( Type var )
* @brief Conversion Little Endian <-> Big Endian (Inversion des octets)
* @param[in] var La variable a qui il faut invers� les octets
* @return Renvoie la {var} avec les octets invers�
*/
template<class Type> Type baseClientServer::reverseByte( Type var )// Inverse les octets
{
	Type out = 0;
	const	char* pIn = (const		char*) &var;
			char* pOut= (			char*)(&out+1)-1;

	for( unsigned int i=sizeof(Type); i>0 ; i-- )
	{
		*pOut-- = *pIn++;
	}
	return out;
}


/***************************************************************************//*!
* @fn bool baseClientServer::isLittleEndian()
* @brief Permet de savoir si l'on est sur une machine Intel, une machine qui lit
* les octects en Little Endian.
* @see baseClientServer::reverseByte( Type var )
*/
bool baseClientServer::isLittleEndian()// isIntel
{
	long l = 1;
	return (bool) *((char *)(&l));
}


/***************************************************************************//*!
* @fn void baseClientServer::setTCP_NoDelay( SOCKET s, bool activer )
* @brief Permet de modifier le param�tre TCP_NoDelay ( Algorithme de Nagle )
* @param[in] s Le socket a modifier
* @param[in] activer D�sactiver ?
*/
void baseClientServer::setTCP_NoDelay( SOCKET s, bool activer )
{
	IF_WINDOWS(
		char use = (char)activer;
	)
	IF_LINUX(
		int use = (int)activer;
	)
	if( setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &use, sizeof(use)) == SOCKET_ERROR )
		reportError("Erreur(%d). Impossible de modifier l'utilisation de l'Algorithme de Nagle ! setsockopt(). Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));
}


/***************************************************************************//*!
* @fn bool baseClientServer::TCP_NoDelay( SOCKET s )
* @brief Permet de savoir l'�tat du param�tre TCP_NoDelay ( Algorithme de Nagle )
* @param[in] s Le socket a analyser
*/
bool baseClientServer::TCP_NoDelay( SOCKET s )
{
	IF_WINDOWS(
		int optLen = sizeof(char);
		char use = 0;
	)
	IF_LINUX(
		int use = 0;
		socklen_t optLen = sizeof(socklen_t);
	)
	if( getsockopt(s, IPPROTO_TCP, TCP_NODELAY, &use, &optLen) == SOCKET_ERROR )
		reportError("Erreur(%d). Impossible d'acc�der au param�tre de l'Algorithme de Nagle ! getsockopt(). Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));

	return (bool)use;
}


/***************************************************************************//*!
* @fn static void baseClientServer::printAllBuffer( const char buffer[], unsigned int bufferSize, const char file[], unsigned int line )
* @brief Permet de voir TOUT le buffer
* @param[in] buffer		Le buffer a afficher
* @param[in] bufferSize	La taille du buffer. [0 � bufferSize[
* @param[in] file		Le fichier d'o� est appel� la fonction (__FILE__)
* @param[in] line		La ligne du fichier d'o� est appel� la fonction (__LINE__)
*/
void baseClientServer::printAllBuffer( const char buffer[], unsigned int bufferSize, const char file[], unsigned int line )
{
	fprintf(stderr, "[file %s, line %u]: Paquet:<", file, line);
	for( unsigned int i=0; i<bufferSize; i++ )
		fprintf(stderr, "%c", buffer[i]);
	fprintf(stderr, ">\n");
}


/***************************************************************************//*!
* @fn char* baseClientServer::getLocalIPv4() const
* @brief Permet d'obtenir UNE adresse IP local
* @return L'adresse IP sous forme de chaine de caract�res ( termin�e par \0 )
* @note Normalement, on peut r�cup les adresses IP locales.
*/
char* baseClientServer::getLocalIPv4() const
{
	// Get host adresses
	hostent* pHost;
	char hostname[256]={0};
	static char IP[17] = {0};// IPv4 !
	int j, p;

	if( gethostname(hostname, 256) == SOCKET_ERROR )
		stdError("Erreur(%d) lors de l'obtention du nom de l'ordinateur. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));


	if( !(pHost = gethostbyname(hostname)) )
		stdError("Erreur(%d) lors de la r�solution name -> host . Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));

	//for( int i=0, j=0, p=0; pHost->h_addr_list[i]!= NULL; i++ )// Renvoie LES adresses IP locales
	//{
	// Le For avec le i permet d'obtenir toutes les adresses IP locales de l'ordinateur
	// Pour cette fonction, on ne veut qu'une seule adresse IP, donc pas de boucle
	// Pour activer la fonctionnalit� multi-IP, d�commenter la boucle for + {}, et commenter le enum
	enum{ i=0 };

		for( j=0, p=0; j <pHost->h_length; j++ )// On obtient une adresse IP
		{
			sprintf(IP+p, "%u.", (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]);
			if( (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j] < 10 )
			{
				p+=2;
			}else if( (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j] < 100 )
			{
				p+=3;
			}else{
				p+=4;
			}
		}
		IP[p-1] = 0;// IP contient ici, une adresse IP ( attention ! cette IP sera perdu au prochain tour du for i
	//}
	return IP;
}
