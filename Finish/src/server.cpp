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

# include "../inc/inc.hpp"

//=================
//Constructeur
//=================

/**
 * @brief Constructeur de la classe Server.
 *
 * Initialise un serveur IRC avec le port et le mot de passe fournis.
 * Configure l'adresse IP locale, le nom du serveur et les paramètres
 * d'administration, puis lance les routines principales d'initialisation
 * et d'exécution.
 *
 * @param port Chaîne représentant le port d'écoute du serveur.
 * @param pass Chaîne contenant le mot de passe d'accès au serveur.
 *
 * @note Appelle les fonctions loadAdminConfig(), displayAdminInfo(),
 *       Init() et Run() pour configurer et démarrer le serveur.
 */
Server::Server(std::string port, std::string pass)
{

	this->_Port = Utils::PortConvert(port);
	this->_Ip = Utils::IpConvert("localhost");
	this->_Pass = pass;
	this->_ServeurOn = true;

	this->_ServName = "IRC_42";
	
	this->_hasMainAdmin = false;
	this->_mainAdminNick = "";
	loadAdminConfig();
	displayAdminInfo();
	
	
	Init();
	Run();
	
	return ;
}

/**
 * @brief Destructeur de la classe Server.
 *
 * Libère les ressources éventuellement allouées par le serveur avant
 * sa destruction. Actuellement, aucune opération spécifique n'est
 * effectuée dans ce destructeur.
 */
Server::~Server()
{
	return ;
}


//===============
//Fonctions public
//===============

/**
 * @brief Arrête le serveur en modifiant son état interne.
 *
 * Met à false le membre _ServeurOn pour indiquer que le serveur
 * doit cesser son fonctionnement. Ne fait rien si le serveur est
 * déjà arrêté.
 */
void Server::ShutSign()
{
	if (!_ServeurOn)
		return;

	this->_ServeurOn = false;
}

/**
 * @brief Ferme proprement le serveur et libère toutes les ressources.
 *
 * Cette fonction :
 *  - Ferme tous les sockets des utilisateurs et vide la liste _User.
 *  - Supprime tous les canaux du serveur (_Chan).
 *  - Ferme le socket d'écoute si ouvert (_Listening).
 *  - Affiche des messages informatifs sur les étapes de fermeture.
 *
 * @note Garantit une fermeture propre sans fuite de ressources.
 */
void Server::Shutdown()
{
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;

	std::cout << YELLOW << "Close socket user:" << RESET << std::endl;
	for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it)
	{
		std::cout << YELLOW << "   - User: " << GREEN << it->second.getName() << YELLOW << " / Socket: " << GREEN << it->second.getSocket() << RESET << std::endl;
		it->second.closeSocket();
	}
	std::cout << YELLOW << "All socket closed" << RESET << std::endl;
	std::cout << YELLOW << "All user clear" << RESET << std::endl;
	this->_User.clear();
	std::cout << YELLOW << "User cleared" << RESET << std::endl;


	std::cout << YELLOW << "All channel clear" << RESET << std::endl;
	this->_Chan.clear();
	std::cout << YELLOW << "Channel cleared" << RESET << std::endl;


	std::cout << YELLOW << "Close Listening" << RESET << std::endl;
	if (this->_Listening != -1)
		close(this->_Listening);
	std::cout << YELLOW << "Listening Closed" << RESET << std::endl;


	std::cout << YELLOW << "Fermeture propre du serveur..." << RESET << std::endl;
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
}


//===============
//Fonctions private
//===============

/**
 * @brief Initialise le serveur et configure le socket d'écoute.
 *
 * Cette fonction :
 *  - Crée le socket d'écoute (_Listening) pour les connexions TCP.
 *  - Configure les options du socket (SO_REUSEADDR).
 *  - Associe le socket à l'adresse IP et au port spécifiés.
 *  - Met le socket en mode écoute avec la taille maximale de backlog.
 *  - Affiche des informations sur l'état du serveur.
 *
 * @note Appelle Error::ErrorServ() en cas d'échec de création, de
 *       configuration, de bind ou de listen.
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
 * @brief Accepte une nouvelle connexion client sur le serveur.
 *
 * Cette fonction :
 *  - Attend et accepte une connexion entrante sur le socket d'écoute.
 *  - Crée un objet User pour le client et l'ajoute à la map _User.
 *  - Affiche les informations de connexion (IP et port) du client.
 *
 * @note Appelle Error::ErrorServ() si accept() échoue.
 * @note Le descripteur du client est utilisé comme clé dans _User.
 */
void Server::AcceptClient()
{
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);

	int clientSocket = accept(this->_Listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == -1)
	{
		Error::ErrorServ(8, "accept failed");
		return;
	}

    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		Error::ErrorServ(9, "fcntl failed");
		close(clientSocket);
		return;
	}

	std::string incomingNick = "";
	this->_User.insert(std::make_pair(clientSocket, User(clientSocket)));


    if (DEBUG == true)
		std::cout << "[DEBUG] getSocket: " << this->_UserObj.getSocket() << std::endl;


	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
	std::cout << GREEN << inet_ntoa(client.sin_addr) << YELLOW
			  << " connected on port " << GREEN << ntohs(client.sin_port)
			  << RESET << std::endl;
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
}

/**
 * @brief Traite les données reçues d'un client connecté.
 *
 * Cette fonction :
 *  - Lit les données depuis le socket du client.
 *  - Gère la déconnexion si le client ferme la connexion ou en cas
 *    d'erreur de réception.
 *  - Analyse et traite chaque ligne de commande IRC reçue :
 *      - NICK, USER, PASS pour l'authentification.
 *      - JOIN, PART, PRIVMSG, KICK, MODE, INVITE, TOPIC.
 *      - Gestion des fichiers FSEND, FGET, FLIST.
 *      - Commandes administratives : OPER, ADMIN, GRADES, SETGRADE.
 *  - Met à jour le buffer de réception et l'état de l'utilisateur.
 *
 * @param clientSocket Descripteur du socket du client à traiter.
 *
 * @note Utilise la map _User pour accéder à l'objet User correspondant.
 * @note Supprime l'utilisateur et ferme son socket en cas de déconnexion.
 */
void Server::HandleClientData(int clientSocket)
{
	char buf[BUF_SIZE];
	int bytesRecv = recv(clientSocket, buf, BUF_SIZE, 0);

	if (bytesRecv <= 0)
	{
		User &user = this->_User[clientSocket];

		if (bytesRecv == 0)
			std::cout << "[INFO] User " << user.getNick() << "@" << user.getName() << " disconnected !" << std::endl;
		else
			std::cerr << "recv failed" << std::endl;
        
        std::set<int> idChan = user.getLstIdChan();
        std::set<int>::iterator itId = idChan.begin();
        while (itId != idChan.end())
        {
            int chanId = *itId;
            std::map<int, Channel>::iterator itChan = this->_Chan.find(chanId);
            if (itChan != this->_Chan.end())
            {
                Channel &chan = itChan->second;
                chan.RemoveUser(user.getSocket());
        
                if (chan.GetNbUser() == 0)
                {
                    std::cout << "[INFO] Channel " << chan.GetName() << " is now empty and will be deleted." << std::endl;
                    this->_Chan.erase(itChan);
                }
            }
            ++itId;
        }

		user.closeSocket();
		FD_CLR(clientSocket, &_Readfds);
		_User.erase(clientSocket);

        return;
    }

    User &user = this->_User[clientSocket];
    user.setRcvBuff(user.getRcvBuff() + std::string(buf, bytesRecv));


    std::string &data = user.getRcvBuff();
    size_t pos;

    if (DEBUG == true)
    {
        std::cout << "------- [DEBUG] data recep -------" << std::endl;
        std::cout << data;
        std::cout << "---------------------------------- " << std::endl;
    }

    while ((pos = data.find("\r\n")) != std::string::npos)
    {
        std::string line = data.substr(0, pos);
        data.erase(0, pos + 2);

        if (line.find("NICK ") == 0)
        {
            user.setNick(GetNick(line));

            if (DEBUG == true)
                std::cout << "[DEBUG] NICK: " << user.getNick() << "-----" << user.getName()  << std::endl;
        }
        else if (line.find("USER ") == 0)
        {
            user.setName(GetName(line, user.getAuth()));

            if (DEBUG == true)
                std::cout << "[DEBUG] USER: " << user.getName() << std::endl;
        }
        else if (line.find("PASS ") == 0)
        {
            user.setPass(GetPwd(line));

            if (DEBUG == true)
                std::cout << "[DEBUG] PASS: " << user.getPass() << std::endl;
        }
        else if (line.find("KICK ") == 0)
        {
            handleKickCommand(clientSocket, line);
            continue;
        }
        else if (line.find("MODE ") == 0)
        {
            std::string rest = line.substr(5);
    
            size_t sp = rest.find(' ');
            std::string chanName;
            std::string param;
            
            if (sp == std::string::npos)
            {
                chanName = rest;
                param = "";
            }
            else
            {
                chanName = rest.substr(0, sp);
                param = rest.substr(sp + 1);
            }
        
            sp = param.find(' ');
            std::string mode;
            std::string mask;
        
            if (sp == std::string::npos)
            {
                mode = param;
                mask = "";
            } 
            else
            {
                mode = param.substr(0, sp);
                mask = param.substr(sp + 1);
            }

            if (mode == "+b" && !mask.empty()) 
            {
                handleBanCommand(clientSocket, chanName, mask);
            }
            else if (mode == "-b" && !mask.empty())
            {
                handleUnbanCommand(clientSocket, chanName, mask);
            }
            else if (mode == "+b" && mask.empty())
            {
                handleBanlistCommand(clientSocket, chanName);
            }
            else
            {
                handleModeCommand(clientSocket, line, this->_User, this->_Chan);
            }
            continue;
        }
        else if (line.find("INVITE ") == 0)
        {
            handleInviteCommand(clientSocket, line, this->_User, this->_Chan);
            continue;
        }
        else if (line.find("MODE ") == 0)
        {
            std::string rest = line.substr(5);
    
            size_t sp = rest.find(' ');
            std::string chanName;
            std::string param;
            
            if (sp == std::string::npos)
            {
                chanName = rest;
                param = "";
            }
            else
            {
                chanName = rest.substr(0, sp);
                param = rest.substr(sp + 1);
            }
        

            sp = param.find(' ');
            std::string mode;
            std::string mask;
        
            if (sp == std::string::npos)
            {
                mode = param;
                mask = "";
            } 
            else
            {
                mode = param.substr(0, sp);
                mask = param.substr(sp + 1);
            }

            if (mode == "+b" && !mask.empty()) 
                handleBanCommand(clientSocket, chanName, mask);
            else if (mode == "-b" && !mask.empty())
                handleUnbanCommand(clientSocket, chanName, mask);
            else if (mode == "+b" && mask.empty())
                handleBanlistCommand(clientSocket, chanName);
            continue;
        }
        else if (line.find("PRIVMSG ") == 0)
        {
            if(line.size() == 8)
                return;

            size_t posSpNa = line.find(" ");
            std::string tmp = line.substr(posSpNa + 1);

            size_t tmppos = tmp.find(" ");
            if(tmppos > 512)
                return;

            std::string tmp0 = "";
            tmp0 = tmp.substr(tmppos);

            if (tmp0[1] != ':')
                return;

            
            if ('#' != tmp[0] && '&' == tmp[0] && '+' == tmp[0] && '!' == tmp[0])
                handleBrodcastPrivateMsg(user,line);
            else
            {
                size_t pos0 = tmp.find(":");
                std::string chanName = "";

                for(size_t i = 0 + 1; i < pos0 - 1; i++)
                    chanName += tmp[i];

                int ref = -1;
                for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
                {
                    if (it->second.GetName().compare(chanName) == 0)
                    {
                        ref = it->second.GetId();
                        break;
                    }
                }

                if (ref > -1 && user.getIdChan(ref) == true)
                    handleBrodcastMsgChann(clientSocket, user,line, ref);
            }
        }
        else if (line.find("JOIN ") == 0)
        {
            handleJoin(clientSocket, user, line);
        }
        else if (line.find("PART ") == 0)
        {
            handleQuit(clientSocket, user, line);
        }
        else if (line.find("LIST") == 0)
        {
            handleList(user);
        }
        else if (line.find("NAMES ") == 0)
        {
            handleNames(user, line);
        }
        else if (line.find("TOPIC ") == 0)
        {
            handleTopic(user, line);
        }
		else if (line.find("FSEND ") == 0)
		{
			handleFileSend(user, line);
		}
		else if (line.find("FGET ") == 0)
		{
			handleFileGet(user, line);
		}
		else if (line.find("FLIST") == 0)
		{
			handleFileList();
		}
		else if (line.find("OPER ") == 0)
		{
			handleOperCommand(clientSocket, line);
			continue;
		}
		else if (line.find("ADMIN ") == 0)
		{
			handleAdminCommand(clientSocket, line);
			continue;
		}
		else if (line.find("GRADES") == 0)
		{
			handleGradesCommand(clientSocket, line);
			continue;
		}
		else if (line.find("SETGRADE ") == 0)
		{
			handleSetGradeCommand(clientSocket, line);
			continue;
		}

	}

    
    if (!user.getNick().empty() && !user.getName().empty() && !user.getPass().empty())
        if (!user.getAuth())
            handleLogin(clientSocket, user);
    
}

/**
 * @brief Boucle principale d'exécution du serveur.
 *
 * Cette fonction :
 *  - Attend les événements sur le socket d'écoute et les sockets clients.
 *  - Accepte les nouvelles connexions entrantes.
 *  - Traite les données reçues de chaque client via HandleClientData().
 *  - Utilise select() pour gérer simultanément plusieurs sockets.
 *  - Continue tant que _ServeurOn est true.
 *  - Ferme proprement le serveur à la fin en appelant Shutdown().
 *
 * @note Met à jour le fd_set _Readfds à chaque itération pour le select().
 * @note Garantit la gestion des clients multiples sans blocage.
 */
void Server::Run()
{
	int maxFd;

	while (this->_ServeurOn)
	{
		FD_ZERO(&_Readfds);
		FD_SET(this->_Listening, &_Readfds);
		maxFd = this->_Listening;

		for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end(); ++it)
		{
			FD_SET(it->second.getSocket(), &_Readfds);
			
            if (it->second.getSocket() > maxFd)
				maxFd = it->second.getSocket();
		}

		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int ret = select(maxFd + 1, &_Readfds, NULL, NULL, &tv);

		if (ret < 0)
			break;

		if (ret == 0)
			continue;

		if (FD_ISSET(this->_Listening, &_Readfds))
			AcceptClient();

		for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end();)
		{
            int clientSock = it->second.getSocket();
			++it;
			if (FD_ISSET(clientSock, &_Readfds))
				HandleClientData(clientSock);
		}
	}

	Shutdown();
}

/**
 * @brief Vérifie si le mot de passe fourni correspond au serveur.
 *
 * Compare la chaîne fournie avec le mot de passe interne _Pass.
 *
 * @param str Mot de passe à vérifier.
 * @return true si le mot de passe est correct, false sinon.
 */
bool Server::PassCont(const std::string& str)
{
	if (str == this->_Pass)
		return true;
	
	return false;
}

/**
 * @brief Extrait le mot de passe d'une commande PASS IRC.
 *
 * Analyse la chaîne fournie et retourne le mot de passe si la
 * commande commence par "PASS ".
 *
 * @param str Chaîne contenant la commande IRC.
 * @return std::string Mot de passe extrait ou chaîne vide si non trouvé.
 *
 * @note Affiche le mot de passe en debug si DEBUG est activé.
 */
std::string Server::GetPwd(const std::string& str)
{
	if (str.substr(0, 4) == "PASS")
	{
		std::string::size_type spacePos = str.find(' ');
		if (spacePos != std::string::npos)
		{
			std::string pass = str.substr(spacePos + 1);

			if (DEBUG == true)
				std::cout << "[DEBUG] PASS trouvé: " << pass << std::endl;
			
			return pass;
		}
	}
	return "";
}

/**
 * @brief Extrait le pseudonyme (nickname) d'une commande NICK IRC.
 *
 * Analyse la chaîne fournie et retourne le pseudonyme si la
 * commande commence par "NICK ".
 *
 * @param str Chaîne contenant la commande IRC.
 * @return std::string Pseudonyme extrait ou chaîne vide si non trouvé.
 *
 * @note Affiche le pseudonyme en debug si DEBUG est activé.
 */
std::string Server::GetNick(const std::string& str)
{
	if (str.substr(0, 4) == "NICK")
	{
		std::string::size_type spacePos = str.find(' ');
		if (spacePos != std::string::npos)
		{
			std::string nick = str.substr(spacePos + 1);

			if (DEBUG == true)
				std::cout << "[DEBUG] NICK trouvé: " << nick << std::endl;
			
			return nick;
		}
	}
	return "";
}

/**
 * @brief Extrait le nom complet de l'utilisateur d'une commande IRC.
 *
 * Analyse la chaîne fournie pour récupérer le nom après "USER" ou
 * "NAMES", selon l'état d'authentification.
 *
 * @param str Chaîne contenant la commande IRC.
 * @param auth Booléen indiquant si l'utilisateur est déjà authentifié.
 * @return std::string Nom extrait ou chaîne vide si non trouvé.
 *
 * @note Affiche le nom en debug si DEBUG est activé.
 */
std::string Server::GetName(const std::string& str, bool auth)
{
	if (auth == false)
	{
		if (str.substr(0, 4) == "USER")
		{
			std::string::size_type colonPos = str.find(':');
			if (colonPos != std::string::npos)
			{
				std::string user = str.substr(colonPos + 1);
	
				if (DEBUG == true)
					std::cout << "[DEBUG] USER trouvé INIT: " << user << std::endl;
	
				return user;
			}
		}
	}
	else
	{
		if (str.substr(0, 4) == "USER" || str.substr(0, 5) == "NAMES")
		{
			std::string::size_type colonPos = str.find(' ');

			if (colonPos != std::string::npos)
			{
				std::string user = str.substr(colonPos + 1);
				std::cout << "[DEBUG] recepe names EDITE: " << user << std::endl;

				return user;
			}
		}
	}

	return "";
}

/**
 * @brief Vérifie si un pseudonyme est déjà utilisé par plusieurs utilisateurs.
 *
 * Parcourt la map _User et compte combien d'utilisateurs ont le
 * pseudonyme fourni. Retourne true si plus d'un utilisateur l'utilise.
 *
 * @param nick Pseudonyme à vérifier.
 * @return true si le pseudonyme est utilisé par plusieurs utilisateurs, false sinon.
 */
bool Server::NickIsList(std::string nick)
{
	int i = 0;

	for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end(); it++)
	{
		if (nick.compare(it->second.getNick()) == 0)
			i++;

		if (i > 1)
			return true;
	}
	return false;
}

/**
 * @brief Vérifie si un pseudonyme est déjà utilisé par un utilisateur.
 *
 * Parcourt la map _User et retourne true dès qu'un utilisateur avec
 * le pseudonyme fourni est trouvé.
 *
 * @param nick Pseudonyme à vérifier.
 * @return true si le pseudonyme est déjà utilisé, false sinon.
 */
bool Server::IsNickIsList(std::string nick)
{
	for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end(); it++)
	{
		if (nick.compare(it->second.getNick()) == 0)
			return true;
	}
	return false;
}


//==============
// ADMIN SYSTEM METHODS
//==============

/**
 * @brief Charge la configuration des administrateurs depuis un fichier.
 *
 * Tente de charger les paramètres d'administration depuis "admin.conf".
 * Affiche un avertissement si le fichier n'est pas trouvé ou non chargé.
 *
 * @note Utilise la méthode _AdminConfig.loadFromFile() pour la lecture.
 */
void Server::loadAdminConfig()
{
    if (!_AdminConfig.loadFromFile("admin.conf"))
    {
        std::cerr << "Warning: Admin config not loaded, using default settings" << std::endl;
    }
}

/**
 * @brief Vérifie si un utilisateur est banni.
 *
 * Cette fonction consulte la configuration administrateur pour déterminer
 * si le pseudo donné figure dans la liste des utilisateurs bannis.
 *
 * @param nickname Le nom de l'utilisateur à vérifier.
 * @return true si l'utilisateur est banni, false sinon.
 */
bool Server::checkUserBanned(const std::string& nickname)
{
    return _AdminConfig.isBannedUser(nickname);
}

/**
 * @brief Attribue automatiquement les privilèges d'opérateur à un utilisateur.
 *
 * Si le canal spécifié est configuré pour l'auto-op, et que l'utilisateur
 * possède les droits nécessaires selon la configuration administrateur,
 * cette fonction lui accorde le statut d'opérateur dans le canal.
 *
 * Un message MODE est ensuite envoyé au client pour notifier le changement
 * de statut, et une confirmation est affichée dans la console.
 *
 * @param clientSocket Le socket du client concerné.
 * @param channelName  Le nom du canal où appliquer l'auto-op.
 */
void Server::giveAutoOpPrivileges(int clientSocket, const std::string& channelName)
{
    if (_AdminConfig.isAutoOpChannel(channelName))
    {
        std::map<int, User>::iterator userIt = _User.find(clientSocket);
        if (userIt != _User.end())
        {
            std::string nickname = userIt->second.getNick();
            
            if (_AdminConfig.hasOperatorPrivs(nickname))
            {
                for (std::map<int, Channel>::iterator chanIt = _Chan.begin(); chanIt != _Chan.end(); ++chanIt)
                {
                    if (chanIt->second.GetName() == channelName)
                    {
                        chanIt->second.giveOp(nickname);
                        
                        std::string modeMsg = ":" + _ServName + " MODE " + channelName + " +o " + nickname + "\r\n";
                        if (Utils::IsSocketWritable(clientSocket))
                            send(clientSocket, modeMsg.c_str(), modeMsg.length(), 0);
                        
                        std::cout << "Auto-op granted to " << nickname << " in " << channelName << std::endl;

                        break;
                    }
                }
            }
        }
    }
}

/**
 * @brief Gère la commande OPER pour promouvoir un utilisateur.
 *
 * Cette fonction permet à un utilisateur de devenir opérateur s’il fournit
 * le mot de passe administrateur correct. Si le mot de passe est valide et
 * que l’utilisateur n’est pas déjà administrateur principal (Grade 0),
 * son grade passe à 1 (opérateur).  
 *
 * Des messages informatifs sont envoyés au client pour confirmer la
 * réussite ou l’échec de la promotion, et un log est affiché en console.
 *
 * @param clientSocket Le socket du client envoyant la commande.
 * @param line         La ligne complète de la commande OPER reçue.
 */
void Server::handleOperCommand(int clientSocket, const std::string& line)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 3)
    {
        std::string errorMsg = ":" + _ServName + " 461 * OPER :Not enough parameters\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);

        return;
    }
    
    std::string username = tokens[1];
    std::string password = tokens[2];
    
    std::map<int, User>::iterator userIt = _User.find(clientSocket);
    if (userIt == _User.end())
    {
        return;
    }
    
    std::string nickname = userIt->second.getNick();
    
    if (password == _AdminConfig.getOperPassword())
    {
        int currentGrade = userIt->second.getGrade();
        
        if (currentGrade == 0)
        {
            std::string msg1 = ":" + _ServName + " NOTICE " + nickname + " :👑 Você já é o ADMINISTRADOR PRINCIPAL!\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, msg1.c_str(), msg1.length(), 0);
            
            std::string msg2 = ":" + _ServName + " NOTICE " + nickname + " :⚡ Grade 0 é superior ao Grade 1 (Operador)\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, msg2.c_str(), msg2.length(), 0);
        }
        else
        {
            userIt->second.setGrade(1);
            
            std::string separator = ":" + _ServName + " NOTICE " + nickname + " :═══════════════════════════════════\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, separator.c_str(), separator.length(), 0);
            
            std::string successMsg1 = ":" + _ServName + " NOTICE " + nickname + " :🎉 PROMOÇÃO REALIZADA COM SUCESSO!\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, successMsg1.c_str(), successMsg1.length(), 0);
            
            std::string successMsg2 = ":" + _ServName + " NOTICE " + nickname + " :⚡ Você agora é OPERADOR (Grade 1)\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, successMsg2.c_str(), successMsg2.length(), 0);
            
            std::string successMsg3 = ":" + _ServName + " NOTICE " + nickname + " :📋 Pode usar: MODE +o, KICK, BAN\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, successMsg3.c_str(), successMsg3.length(), 0);
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, separator.c_str(), separator.length(), 0);
            
            std::cout << "OPER: " << nickname << " promoted from Grade " << currentGrade << " to Grade 1 (Operator)" << std::endl;
        }
    }
    else
    {
        std::string errorMsg = ":" + _ServName + " NOTICE " + nickname + " :❌ Senha incorreta para OPER\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);

        std::cout << "OPER: Failed attempt by " << nickname << " with wrong password" << std::endl;
    }
}

/**
 * @brief Envoie un message de bienvenue personnalisé à un utilisateur.
 *
 * Cette fonction envoie au client un ensemble de messages IRC formatés 
 * contenant des informations sur son grade, ses privilèges et les 
 * commandes auxquelles il a accès.  
 * Le message varie selon le grade :  
 * - **Grade 0** : Administrateur principal  
 * - **Grade 1** : Opérateur  
 * - **Grade 2** : Voix  
 * - **Grade 3** : Utilisateur normal  
 *
 * @param clientSocket Le socket du client connecté.
 * @param user         Référence constante vers l'objet User concerné.
 */
void Server::sendWelcomeMessage(int clientSocket, const User& user)
{
    std::string nickname = user.getNick();
    int grade = user.getGrade();

    std::string welcomeMsg = ":" + _ServName + " 001 " + nickname + " :Bienvenue sur " + _ServName + "!\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, welcomeMsg.c_str(), welcomeMsg.length(), 0);
    
    std::string separator = ":" + _ServName + " 002 " + nickname + " :═══════════════════════════════════\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, separator.c_str(), separator.length(), 0);
    
    std::string statusMsg = ":" + _ServName + " 003 " + nickname + " :";
    
    switch(grade)
    {
        case 0:
            statusMsg += "👑 ADMINISTRATEUR PRINCIPAL (Grade 0)";
            break;
        case 1:
            statusMsg += "⚡ OPÉRATEUR (Grade 1)";
            break;
        case 2:
            statusMsg += "🔊 VOIX (Grade 2)";
            break;
        case 3:
            statusMsg += "👤 UTILISATEUR NORMAL (Grade 3)";
            break;
        default:
            statusMsg += "❓ INCONNU";
    }
    statusMsg += "\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, statusMsg.c_str(), statusMsg.length(), 0);
    
    std::string privMsg = ":" + _ServName + " 004 " + nickname + " :Privilèges: ";
    
    switch(grade)
    {
        case 0:
            privMsg += "Contrôle total du serveur";
            break;
        case 1:
            privMsg += "Modérer canaux, donner OP, kick/ban";
            break;
        case 2:
            privMsg += "Parler dans les canaux modérés (+m)";
            break;
        case 3:
            privMsg += "Accès de base aux canaux";
            break;
        default:
            privMsg += "Aucun";
    }
    privMsg += "\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, privMsg.c_str(), privMsg.length(), 0);
    
    if (grade == 0)
    {
        std::string adminMsg1 = ":" + _ServName + " 005 " + nickname + " :🎯 Vous avez le CONTRÔLE TOTAL du serveur!\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, adminMsg1.c_str(), adminMsg1.length(), 0);
        
        std::string adminMsg2 = ":" + _ServName + " 006 " + nickname + " :📋 Commandes: MODE, KICK, BAN, INVITE, TOPIC\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, adminMsg2.c_str(), adminMsg2.length(), 0);
    }
    else if (grade == 1)
    {
        std::string operMsg1 = ":" + _ServName + " 005 " + nickname + " :⚡ Vous êtes OPÉRATEUR du serveur!\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, operMsg1.c_str(), operMsg1.length(), 0);
        
        std::string operMsg2 = ":" + _ServName + " 006 " + nickname + " :📋 Commandes: MODE +o, KICK, BAN dans les canaux\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, operMsg2.c_str(), operMsg2.length(), 0);
    }
    else if (grade == 2)
    {
        std::string voiceMsg = ":" + _ServName + " 005 " + nickname + " :🔊 Vous avez la VOIX - vous pouvez parler dans les canaux modérés\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, voiceMsg.c_str(), voiceMsg.length(), 0);
    }
    else if (grade == 3)
    {
        std::string userMsg1 = ":" + _ServName + " 005 " + nickname + " :📈 Pour obtenir plus de privilèges:\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, userMsg1.c_str(), userMsg1.length(), 0);
        
        std::string userMsg2 = ":" + _ServName + " 006 " + nickname + " :   💼 OPER " + nickname + " <motdepasse> → Devenir Opérateur\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, userMsg2.c_str(), userMsg2.length(), 0);
        
        if (nickname == _AdminConfig.getMainAdmin())
        {
            std::string adminHint = ":" + _ServName + " 007 " + nickname + " :   👑 ADMIN " + nickname + " <motdepasse> → Devenir Admin Principal\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, adminHint.c_str(), adminHint.length(), 0);
        }
    }
    
    std::string separator2 = ":" + _ServName + " 008 " + nickname + " :═══════════════════════════════════\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, separator2.c_str(), separator2.length(), 0);
    
    std::string visibleMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :";
    
    switch(grade)
    {
        case 0:
            visibleMsg += "👑 Você é ADMINISTRADOR PRINCIPAL (Grade 0) - Controle total!";
            break;
        case 1:
            visibleMsg += "⚡ Você é OPERADOR (Grade 1) - Pode moderar canais";
            break;
        case 2:
            visibleMsg += "🔊 Você tem VOZ (Grade 2) - Pode falar em canais +m";
            break;
        case 3:
            visibleMsg += "👤 Usuário normal (Grade 3) - Use OPER para privilégios";
            break;
    }
    visibleMsg += "\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, visibleMsg.c_str(), visibleMsg.length(), 0);
}

/**
 * @brief Gère la commande ADMIN permettant à un utilisateur de devenir
 *        administrateur principal du serveur.
 *
 * Cette fonction vérifie les identifiants fournis par l'utilisateur via la
 * commande IRC `ADMIN <username> <password>`.  
 * Si la combinaison est valide (selon le fichier de configuration admin.conf),
 * l'utilisateur est promu **Grade 0** (Administrateur Principal) et reçoit
 * une série de messages de confirmation et d’information sur ses privilèges.  
 * En cas d’échec, un message d’erreur est envoyé.
 *
 * @param clientSocket Le socket du client exécutant la commande.
 * @param line         La ligne complète contenant la commande IRC ADMIN.
 *
 * @note 
 * - En cas de succès, le grade de l’utilisateur passe à 0 (niveau maximum).
 * - Les privilèges associés incluent le contrôle total du serveur :
 *   gestion des modes, bans, topics, et utilisateurs.
 */
void Server::handleAdminCommand(int clientSocket, const std::string& line)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 3)
    {
        std::string errorMsg = ":" + _ServName + " 461 * ADMIN :Not enough parameters\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    
    std::string username = tokens[1];
    std::string password = tokens[2];
    
    std::map<int, User>::iterator userIt = _User.find(clientSocket);
    if (userIt == _User.end())
    {
        return;
    }
    
    std::string nickname = userIt->second.getNick();
    
    if (_AdminConfig.validateAdminCredentials(username, password))
    {
        userIt->second.setGrade(0);
        
        std::string separator = ":" + _ServName + " NOTICE " + nickname + " :═══════════════════════════════════\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, separator.c_str(), separator.length(), 0);
        
        std::string successMsg1 = ":" + _ServName + " NOTICE " + nickname + " :🎉 AUTENTICAÇÃO ADMIN REALIZADA!\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, successMsg1.c_str(), successMsg1.length(), 0);
        
        std::string successMsg2 = ":" + _ServName + " NOTICE " + nickname + " :👑 Você agora é ADMINISTRADOR PRINCIPAL\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, successMsg2.c_str(), successMsg2.length(), 0);
        
        std::string successMsg3 = ":" + _ServName + " NOTICE " + nickname + " :⚡ Grade 0 - CONTROLE TOTAL DO SERVIDOR\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, successMsg3.c_str(), successMsg3.length(), 0);
        
        std::string successMsg4 = ":" + _ServName + " NOTICE " + nickname + " :📋 Todos os comandos disponíveis!\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, successMsg4.c_str(), successMsg4.length(), 0);
        
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, separator.c_str(), separator.length(), 0);
        
        std::cout << "ADMIN: " << nickname << " authenticated as server administrator" << std::endl;
    }
    else
    {
        std::string errorMsg = ":" + _ServName + " NOTICE " + nickname + " :❌ Credenciais de admin inválidas\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        std::cout << "ADMIN: Failed authentication attempt by " << nickname << std::endl;
    }
}

/**
 * @brief Affiche les informations de configuration administratives du serveur.
 *
 * Cette fonction affiche dans la console les identifiants et mots de passe
 * actuels pour l’administrateur principal et les opérateurs, tels que définis
 * dans le fichier `admin.conf`.  
 * Elle fournit également un récapitulatif clair des différents niveaux
 * d’autorisation ("grades") disponibles sur le serveur.
 *
 * @details
 * - Montre le nom et la commande pour se connecter en tant qu’Admin principal.  
 * - Indique la commande pour devenir opérateur.  
 * - Présente la hiérarchie complète des grades et leurs privilèges.
 *
 * @note Les mots de passe affichés ici ne devraient être visibles qu’en
 *       environnement de développement ou de test, jamais en production.
 */
void Server::displayAdminInfo()
{
    std::cout << "=====================================" << std::endl;
    std::cout << "📋 CONFIGURAÇÕES DE ADMINISTRAÇÃO" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "🔑 Admin Principal: " << _AdminConfig.getMainAdmin() << std::endl;
    std::cout << "   └─ Comando: ADMIN " << _AdminConfig.getMainAdmin() << " " << _AdminConfig.getAdminPassword() << std::endl;
    std::cout << std::endl;
    std::cout << "👥 Para Operadores (Grade 1):" << std::endl;
    std::cout << "   └─ Comando: OPER <nickname> " << _AdminConfig.getOperPassword() << std::endl;
    std::cout << std::endl;
    std::cout << "📊 Grades do Sistema:" << std::endl;
    std::cout << "   Grade 0: Admin Principal (controle total)" << std::endl;
    std::cout << "   Grade 1: Operador (moderar canais)" << std::endl;
    std::cout << "   Grade 2: Voice (falar em canais +m)" << std::endl;
    std::cout << "   Grade 3: Usuário normal" << std::endl;
    std::cout << "=====================================" << std::endl;
}

/**
 * @brief Gère la commande interne "GRADES" permettant d’afficher les rôles
 *        (grades) des utilisateurs actuellement connectés au serveur.
 *
 * @param clientSocket Le descripteur de socket du client qui a envoyé la commande.
 * @param line         Ligne de commande reçue (non utilisée ici).
 *
 * @details
 * Cette commande envoie au client une liste formatée de tous les utilisateurs
 * connectés, accompagnée de leur grade :
 * - 👑 Grade 0 : Administrateur Principal  
 * - ⚡ Grade 1 : Opérateur  
 * - 🔊 Grade 2 : Voix  
 * - 👤 Grade 3 : Utilisateur normal  
 *
 * @note Le message est envoyé via des lignes PRIVMSG afin d’être affiché
 *       directement dans le client IRC.
 *
 * @example
 * Commande :
 * @code
 * /GRADES
 * @endcode
 *
 * Résultat :
 * @code
 * 📊 GRADES DOS USUÁRIOS CONECTADOS
 * ─────────────────────────────────
 * 👑 Admin - Administrateur Principal (Grade 0)
 * ⚡ Oper1 - Opérateur (Grade 1)
 * 👤 UserA - Utilisateur Normal (Grade 3)
 * 💡 Use: SETGRADE <nick> <0-3> para alterar
 * @endcode
 */
void Server::handleGradesCommand(int clientSocket, const std::string& /* line */)
{
    std::map<int, User>::iterator userIt = _User.find(clientSocket);
    if (userIt == _User.end())
    {
        return;
    }
    
    std::string nickname = userIt->second.getNick();
    
    std::string header = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :📊 GRADES DOS USUÁRIOS CONECTADOS\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, header.c_str(), header.length(), 0);
    
    std::string separator = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :═════════════════════════════════════\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, separator.c_str(), separator.length(), 0);
    
    for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it)
    {
        std::string userNick = it->second.getNick();
        int grade = it->second.getGrade();
        
        std::string gradeMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :";
        
        switch(grade)
        {
            case 0:
                gradeMsg += "👑 " + userNick + " - Administrateur Principal (Grade 0)";
                break;
            case 1:
                gradeMsg += "⚡ " + userNick + " - Opérateur (Grade 1)";
                break;
            case 2:
                gradeMsg += "🔊 " + userNick + " - Voix (Grade 2)";
                break;
            case 3:
                gradeMsg += "👤 " + userNick + " - Utilisateur Normal (Grade 3)";
                break;
            default:
                gradeMsg += "❓ " + userNick + " - Desconhecido";
        }
        gradeMsg += "\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, gradeMsg.c_str(), gradeMsg.length(), 0);
    }
    
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, separator.c_str(), separator.length(), 0);
    
    std::string footer = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :💡 Use: SETGRADE <nick> <0-3> para alterar\r\n";
    if (Utils::IsSocketWritable(clientSocket))
        send(clientSocket, footer.c_str(), footer.length(), 0);
}

/**
 * @brief Gère la commande "SETGRADE" permettant à l’Administrateur Principal
 *        de modifier le grade d’un utilisateur connecté.
 *
 * @param clientSocket Le descripteur de socket du client ayant envoyé la commande.
 * @param line         La ligne complète contenant la commande SETGRADE et ses arguments.
 *
 * @details
 * Cette commande permet uniquement au grade 0 (Administrateur Principal)
 * de modifier le niveau de privilège d’un utilisateur parmi :
 * - 👑 Grade 0 : Administrateur Principal  
 * - ⚡ Grade 1 : Opérateur  
 * - 🔊 Grade 2 : Voix  
 * - 👤 Grade 3 : Utilisateur normal  
 *
 * Si la commande est correctement exécutée :
 * - L’administrateur reçoit un message de confirmation.  
 * - L’utilisateur ciblé est notifié du changement de grade.  
 *
 * En cas d’erreur (mauvaise syntaxe, utilisateur inexistant, permissions
 * insuffisantes, ou grade invalide), un message explicite est envoyé.
 *
 * @note
 * Seul un utilisateur avec le grade 0 peut utiliser cette commande.
 *
 * @example
 * Commande :
 * @code
 * /SETGRADE Alice 1
 * @endcode
 *
 * Résultat :
 * @code
 * ✅ Alice modifié du Grade 3 au Grade 1
 * 🎉 Votre grade a été modifié en: ⚡ Opérateur (Grade 1)
 * @endcode
 */
void Server::handleSetGradeCommand(int clientSocket, const std::string& line)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 3)
    {
        std::string errorMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG * :❌ Uso: SETGRADE <nickname> <grade>\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    

    std::map<int, User>::iterator adminIt = _User.find(clientSocket);
    if (adminIt == _User.end())
    {
        return;
    }
    
    std::string adminNick = adminIt->second.getNick();
    int adminGrade = adminIt->second.getGrade();
    
    if (adminGrade != 0)
    {
        std::string errorMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + adminNick + " :❌ Seul l'Admin Principal (Grade 0) peut utiliser SETGRADE\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    
    std::string targetNick = tokens[1];
    std::string gradeParam = tokens[2];
    
    if (!gradeParam.empty() && gradeParam[0] == '#')
    {
        gradeParam = gradeParam.substr(1);
    }
    
    int newGrade = atoi(gradeParam.c_str());
    
    if (newGrade < 0 || newGrade > 3)
    {
        std::string errorMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + adminNick + " :❌ Le grade doit être 0, 1, 2 ou 3\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
       
        return;
    }
    
    bool found = false;
    for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it)
    {
        if (it->second.getNick() == targetNick)
        {
            int oldGrade = it->second.getGrade();
            it->second.setGrade(newGrade);
            
            char oldGradeChar = '0' + oldGrade;
            char newGradeChar = '0' + newGrade;
            
            std::string successMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + adminNick + " :✅ " + targetNick + " modifié du Grade " + oldGradeChar + " au Grade " + newGradeChar + "\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(clientSocket, successMsg.c_str(), successMsg.length(), 0);
            
            std::string gradeName;
            switch(newGrade)
            {
                case 0: 
                    gradeName = "👑 Administrateur Principal";
                    break;

                case 1:
                    gradeName = "⚡ Opérateur";
                    break;

                case 2:
                    gradeName = "🔊 Voix";
                    break;

                case 3:
                    gradeName = "👤 Utilisateur Normal";
                    break;

                default:
                    gradeName = "❓ Inconnu";
                    break;
            }
            
            std::string notifyMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + targetNick + " :🎉 Votre grade a été modifié en: " + gradeName + " (Grade " + newGradeChar + ")\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                send(it->first, notifyMsg.c_str(), notifyMsg.length(), 0);
            
            std::cout << "SETGRADE: " << adminNick << " changed " << targetNick << " from Grade " << oldGrade << " to Grade " << newGrade << std::endl;
            found = true;
            break;
        }
    }
    
    if (!found)
    {
        std::string errorMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + adminNick + " :❌ Usuário '" + targetNick + "' não encontrado\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
    }
}
