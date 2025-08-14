/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/inc.hpp"

//=================
//Constructeur
//=================

Server::Server(std::string port, std::string pass)
{

    this->_Port = Utils::PortConvert(port);
    this->_Ip = Utils::IpConvert("localhost");
    this->_Pass = pass;
    this->_ServeurOn = true;

    Init();
    AcceptClient();
    Run();
    
    return ;
}


Server::~Server()
{
    
    return ;
}


//===============
//Fonctions public
//===============

/**
 * @brief Arrête proprement le serveur en fermant les sockets ouverts.
 *
 * Met le flag _ServeurOn à false pour signaler l'arrêt. Ferme la socket
 * client si elle est ouverte (_ClientSocket != -1), puis ferme la socket
 * d'écoute (_Listening) si elle est également ouverte. Affiche un message
 * indiquant que le serveur a été arrêté proprement.
 */
void Server::Shutdown()
{
    this->_ServeurOn = false;

    if (this->_ClientSocket != -1)
        close(this->_ClientSocket);

    if (this->_Listening != -1)
        close(this->_Listening);

    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
    std::cout << YELLOW << "Serveur arrêté proprement." << RESET << std::endl;
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;      
}


//===============
//Fonctions private
//===============

/**
 * @brief Initialise le socket du serveur pour l'écoute des connexions.
 *
 * Crée un socket IPv4 en mode TCP, configure les options pour permettre
 * la réutilisation de l'adresse, puis lie le socket à l'adresse IP locale
 * et au port stocké dans l'attribut _Port. Enfin, le socket est mis en
 * écoute pour accepter les connexions entrantes.
 *
 * En cas d'erreur lors de la création, configuration, liaison ou mise
 * en écoute du socket, une erreur est levée via Error::ErrorServ avec
 * un code spécifique (4 à 7 selon l'étape échouée).
 */
void Server::Init()
{
    sockaddr_in		hint;
    int             opt = 1;

	this->_Listening = socket(AF_INET, SOCK_STREAM, 0); 
	if (this->_Listening == -1)
        Error::ErrorServ(4, "");
    
    std::memset(&hint, 0, sizeof(hint));
	hint.sin_family = AF_INET;
	hint.sin_port = htons(this->_Port);

    
	if (setsockopt(this->_Listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        Error::ErrorServ(5, "");

    if (bind (this->_Listening, (struct sockaddr*)&hint, sizeof(hint)) == -1)
        Error::ErrorServ(6, "");
    
    if ( listen( this->_Listening, SOMAXCONN) == -1)
        Error::ErrorServ(7, "");
    

    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
    std::cout << GREEN << "Server OK" << RESET << std::endl;
    std::cout << YELLOW << "-- listening on Port:" << CYAN  << this->_Port << RESET << std::endl;
    std::cout << YELLOW << "-- Ip: " << CYAN << inet_ntoa(hint.sin_addr) <<  RESET << std::endl;
    std::cout << YELLOW << "-- Pwd: " << CYAN << this->_Pass <<  RESET << std::endl;
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;      
}

/**
 * @brief Accepte une connexion entrante d'un client sur le socket serveur.
 *
 * Cette fonction bloque jusqu'à ce qu'un client tente de se connecter.
 * Elle accepte la connexion, ferme le socket d'écoute pour empêcher
 * d'autres connexions (à retirer pour supporter plusieurs clients), puis
 * récupère et affiche le nom d'hôte et le service (port) du client.
 *
 * En cas d'erreur lors de l'acceptation, une erreur est levée via
 * Error::ErrorServ(8, "").
 */
void Server::AcceptClient()
{
	sockaddr_in	client;
	socklen_t	clientSize;
    char		host[NI_MAXHOST];   //1024
	char		svc[NI_MAXSERV];    //32 	

    clientSize = sizeof(client);

	this->_ClientSocket = accept(this->_Listening,  (sockaddr*)&client, &clientSize);
	close (this->_Listening); //Ferme le port d'ecoute a la connexion de l'utilisateur (a suprimer pour le multiple user)

	if (this->_ClientSocket == -1)
        Error::ErrorServ(8, "");

    memset (host, 0, NI_MAXHOST);
	memset (svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, svc, NI_MAXSERV, 0);


    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
    if (result == 0)
		std::cout << GREEN << host << YELLOW << " connected on " << GREEN << svc << RESET << std::endl;
	else
		std::cout << GREEN << inet_ntoa(client.sin_addr) << YELLOW << " connected on port " << GREEN << ntohs(client.sin_port) << RESET << std::endl;
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
}

void Server::Run()
{
    char buf[BUF_SIZE];

    while (this->_ServeurOn)
	{
		memset (buf, 0, BUF_SIZE);

		int bytesRecv = recv(this->_ClientSocket, buf, BUF_SIZE, 0 );

		if (bytesRecv == 0)
		{
			std::cerr << "Client Disconected " << std::endl;
			break ;
		}
		if (bytesRecv < 0)
		{
			std::cout << "There was connection issue " << std::endl;
			break ;
		}


        //Buffer de reception pour compiler les data receptionner car le protocoel
        //IRC envoie par packet
        this->_RecvBuffer.append(buf, bytesRecv);

        /*
        * fonction controle du password
        */
        if (PassCont() == false)
        {
			std::cerr << "Wrong Password " << std::endl;
			break ;
        }


		std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl; 
		
		/*resend message*/
		/*send(this->clientSocket, buf, bytesRecv , 0);*/
		send(this->_ClientSocket, "Hello, world!\n", 14, 0) ;

		if (send(this->_ClientSocket, buf, bytesRecv , 0) == -1)
		{
			std::cerr << "Send failed" << std::endl;
			break ;
		}
	
	}

    Shutdown();
}

/**
 * @brief Traite le buffer reçu pour détecter et valider la commande PASS.
 *
 * Cette fonction lit le contenu du buffer `_RecvBuffer` ligne par ligne
 * (délimité par "\r\n"). Si une ligne commence par "PASS", elle extrait
 * la valeur du mot de passe et la compare au mot de passe attendu `_Pass`.
 *
 * @return true si le mot de passe est correct ou si la commande PASS
 *         n’a pas encore été reçue, false si la commande PASS est
 *         reçue mais invalide.
 */
bool Server::PassCont()
{
    std::string::size_type pos;

    while ((pos = this->_RecvBuffer.find("\r\n")) != std::string::npos)
    {
        std::string line = this->_RecvBuffer.substr(0, pos);
        this->_RecvBuffer.erase(0, pos + 2);

        if (line.substr(0, 4) == "PASS")
        {
            std::string passValue = line.size() > 5 ? line.substr(5) : "";
            //std::cout << "[DEBUG] PASS trouvé: " << passValue << std::endl;
            return passValue == this->_Pass;
        }
    }

    return true;
}