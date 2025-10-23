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
 * @brief Constructeur du serveur : initialise, crée un canal par défaut et lance le serveur.
 *
 * @param port Port d'écoute du serveur.
 * @param pass Mot de passe du serveur.
 */
Server::Server(std::string port, std::string pass)
{

	this->_Port = Utils::PortConvert(port);
	this->_Ip = Utils::IpConvert("localhost");
	this->_Pass = pass;
	this->_ServeurOn = true;

	this->_ServName = "IRC_42";
	
	
	// Initialize admin system
	this->_hasMainAdmin = false;
	this->_mainAdminNick = "";
	loadAdminConfig();
	displayAdminInfo();
	
	
	Init();
	Run();
	
	return ;
}

/**
 * @brief Destructeur par défaut de la classe Server.
 */
Server::~Server()
{
	return ;
}


//===============
//Fonctions public
//===============

/**
 * @brief Arrête le serveur en mettant le drapeau _ServeurOn à false.
 */
void Server::ShutSign()
{
	if (!_ServeurOn)
		return;

	this->_ServeurOn = false;
}

/**
 * @brief Ferme proprement le serveur en nettoyant utilisateurs, canaux et sockets.
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
 * @brief Initialise le serveur en créant la socket, configurant les options et
 *        en liant l'adresse IP et le port. Met ensuite la socket en mode écoute.
 *
 * - Crée une socket TCP.
 * - Configure SO_REUSEADDR pour réutiliser l'adresse rapidement.
 * - Lie la socket à l'adresse IP et au port définis.
 * - Passe la socket en écoute pour accepter les connexions entrantes.
 * - Affiche les informations du serveur (port, IP, mot de passe) sur la console.
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
 * @brief Accepte une nouvelle connexion client et l'ajoute à la liste des utilisateurs.
 *
 * - Attend une connexion entrante sur la socket d'écoute.
 * - Crée un objet User associé au descripteur de socket du client.
 * - Ajoute le client dans la map _User pour le gérer.
 * - Affiche l'adresse IP et le port du client connecté.
 * - En mode DEBUG, affiche le descripteur de socket du client.
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
 * @brief Traite les données reçues d'un client spécifique.
 *
 * - Lit les données depuis le socket du client.
 * - Gère la déconnexion ou les erreurs de réception.
 * - Concatène les nouvelles données au buffer de réception de l'utilisateur.
 * - Analyse les commandes IRC : NICK, USER, PASS, PRIVMSG.
 * - Authentifie l'utilisateur si NICK, USER et PASS sont reçus.
 * - Ajoute l'utilisateur au canal par défaut après authentification.
 * - Diffuse les messages privés (PRIVMSG) aux membres du canal.
 * - Affiche des informations et messages DEBUG sur la console.
 *
 * @param clientSocket Descripteur de socket du client à traiter.
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
        
        //std::cout << line << std::endl;

        if (line.find("NICK ") == 0)
        {
            user.setNick(GetNick(line));
            /*
            if (getBanList().isBanned(user.getNick()))
            {
                std::string msg = ":" + std::string("server") +
                                " 465 " + user.getNick() +
                                " :You're banned from this server\r\n";
                send(clientSocket, msg.c_str(), msg.size(), 0);
                user.closeSocket();
                FD_CLR(clientSocket, &_Readfds);
                _User.erase(clientSocket);
                return;
            }
            */
            if (DEBUG == true)
                std::cout << "[DEBUG] NICK: " << user.getNick() << "-----" << user.getName()  << std::endl;
        }
        // NICKNAME
        else if (line.find("USER ") == 0)
        {
            user.setName(GetName(line, user.getAuth()));
            if (DEBUG == true)
                std::cout << "[DEBUG] USER: " << user.getName() << std::endl;
        }
        // PASSWORD
        else if (line.find("PASS ") == 0)
        {
            user.setPass(GetPwd(line));
            if (DEBUG == true)
                std::cout << "[DEBUG] PASS: " << user.getPass() << std::endl;
        }
        // KICK
        else if (line.find("KICK ") == 0)
        {
            handleKickCommand(clientSocket, line);
            continue;
        }
		// MODE - Suporte completo para modos IRC
        else if (line.find("MODE ") == 0)
        {
            // Primeiro verificar se é um comando de ban (manter compatibilidade)
            std::string rest = line.substr(5); // tout après "MODE "
    
            // 1) Extraire le channel
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
        
            // 2) Extraire le mode et éventuellement le mask
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

            // Verificar se é comando de ban (manter compatibilidade)
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
                // Usar novo sistema de modos
                handleModeCommand(clientSocket, line, this->_User, this->_Chan);
            }
            
            continue;
        }
        // INVITE - Convidar usuário para canal invite-only
        else if (line.find("INVITE ") == 0)
        {
            handleInviteCommand(clientSocket, line, this->_User, this->_Chan);
            continue;
        }
        // BAN - UBAN - BANLIST
        else if (line.find("MODE ") == 0)
        {
            std::string rest = line.substr(5); // tout après "MODE "
    
            // 1) Extraire le channel
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
        
            // 2) Extraire le mode et éventuellement le mask
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
        // MSG
        else if (line.find("PRIVMSG ") == 0)
        {
            size_t pos0 = line.find(" :");
            size_t pos1 = line.find(" #");
            size_t pos2 = line.find(" &");
            size_t pos3 = line.find(" +");
            size_t pos4 = line.find(" !");

            if (pos1 > pos0 && pos2 > pos0 && pos3 > pos0 && pos4 > pos0)
                handleBrodcastPrivateMsg(user,line);
            else
            {
                //extrait le nom du channel
                std::string chanName = "";
                size_t posSpNa = line.find(" ");
                for(size_t i = posSpNa + 2; i < pos0; i++)
                    chanName += line[i];

                int ref = -1;
                for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
                {
                    if (it->second.GetName().compare(chanName) == 0)
                    {
                        ref = it->second.GetId();
                        break;
                    }
                }

                // Uniquementis le channel a ete trouver
                if (ref > -1 && user.getIdChan(ref) == true)
                    handleBrodcastMsgChann(clientSocket, user,line, ref);
            }
        }
        //JOIN
        else if (line.find("JOIN ") == 0)
        {
            handleJoin(clientSocket, user, line);
        }
        //PART
        else if (line.find("PART ") == 0)
        {
            handleQuit(clientSocket, user, line);
        }
        //LIST
        else if (line.find("LIST") == 0)
        {
            handleList(user);
        }
        //NAMES
        else if (line.find("NAMES ") == 0)
        {
            handleNames(user, line);
        }
        //TOPIC
        else if (line.find("TOPIC ") == 0)
        {
            handleTopic(user, line);
        }
		//FSEND
		else if (line.find("FSEND ") == 0)
		{
			handleFileSend(user, line);
		}
		//FGET
		else if (line.find("FGET ") == 0)
		{
			handleFileGet(user, line);
		}
		//FLIST
		/*else if (line.find("FLIST ") == 0)
		{
			handleFileList();
		}*/
		// OPER - Comando para obter privilégios de operador
		else if (line.find("OPER ") == 0)
		{
			handleOperCommand(clientSocket, line);
			continue;
		}
		// ADMIN - Comando para autenticar como administrador
		else if (line.find("ADMIN ") == 0)
		{
			handleAdminCommand(clientSocket, line);
			continue;
		}
		// GRADES - Ver grades de todos os usuários
		else if (line.find("GRADES") == 0)
		{
			handleGradesCommand(clientSocket, line);
			continue;
		}
		// SETGRADE - Alterar grade de usuário (só admin)
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
 * @brief Boucle principale du serveur pour gérer les connexions et données clients.
 *
 * - Initialise le set de sockets à surveiller (fd_set).
 * - Ajoute la socket d'écoute et toutes les sockets clients au fd_set.
 * - Utilise select() pour attendre l'activité sur les sockets.
 * - Accepte les nouvelles connexions entrantes.
 * - Traite les données reçues des clients actifs.
 * - Continue la boucle tant que le serveur est actif (_ServeurOn).
 * - Ferme proprement le serveur à la sortie de la boucle.
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

            HandleClientData(clientSock);
        
            it = this->_User.find(clientSock);
            if (it != this->_User.end())
                ++it;
		}
	}

	Shutdown();
}

/**
 * @brief Vérifie si le mot de passe fourni correspond à celui du serveur.
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
 * - Vérifie que la commande commence par "PASS".
 * - Récupère la chaîne après le premier espace comme mot de passe.
 * - Affiche le mot de passe en mode DEBUG.
 *
 * @param str Ligne de commande reçue.
 * @return Mot de passe extrait ou chaîne vide si non trouvé.
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
 * @brief Extrait le pseudonyme (NICK) d'une commande IRC.
 *
 * - Vérifie que la commande commence par "NICK".
 * - Récupère la chaîne après le premier espace comme pseudonyme.
 * - Affiche le pseudonyme en mode DEBUG.
 *
 * @param str Ligne de commande reçue.
 * @return Pseudonyme extrait ou chaîne vide si non trouvé.
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
 * @brief Extrait le nom d'utilisateur (USER) d'une commande IRC.
 *
 * - Vérifie que la commande commence par "USER".
 * - Récupère la chaîne après le caractère ':' comme nom d'utilisateur.
 * - Affiche le nom en mode DEBUG.
 *
 * @param str Ligne de commande reçue.
 * @return Nom d'utilisateur extrait ou chaîne vide si non trouvé.
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

//controle les doublon nickname
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

//controle si l'user est dans la liste
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

void Server::loadAdminConfig() {
    if (!_AdminConfig.loadFromFile("admin.conf")) {
        std::cerr << "Warning: Admin config not loaded, using default settings" << std::endl;
    }
}

bool Server::checkUserBanned(const std::string& nickname) {
    return _AdminConfig.isBannedUser(nickname);
}

void Server::giveAutoOpPrivileges(int clientSocket, const std::string& channelName) {
    // Check if this channel should give auto-op
    if (_AdminConfig.isAutoOpChannel(channelName)) {
        std::map<int, User>::iterator userIt = _User.find(clientSocket);
        if (userIt != _User.end()) {
            std::string nickname = userIt->second.getNick();
            
            // Give op if user has operator privileges
            if (_AdminConfig.hasOperatorPrivs(nickname)) {
                // Find the channel
                for (std::map<int, Channel>::iterator chanIt = _Chan.begin(); chanIt != _Chan.end(); ++chanIt) {
                    if (chanIt->second.GetName() == channelName) {
                        chanIt->second.giveOp(nickname);
                        
                        // Send mode change notification
                        std::string modeMsg = ":" + _ServName + " MODE " + channelName + " +o " + nickname + "\r\n";
                        send(clientSocket, modeMsg.c_str(), modeMsg.length(), 0);
                        
                        std::cout << "Auto-op granted to " << nickname << " in " << channelName << std::endl;
                        break;
                    }
                }
            }
        }
    }
}

void Server::handleOperCommand(int clientSocket, const std::string& line) {
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 3) {
        std::string errorMsg = ":" + _ServName + " 461 * OPER :Not enough parameters\r\n";
        send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    
    std::string username = tokens[1];
    std::string password = tokens[2];
    
    // Check if password matches and user is authorized
    std::map<int, User>::iterator userIt = _User.find(clientSocket);
    if (userIt == _User.end()) {
        return;
    }
    
    std::string nickname = userIt->second.getNick();
    
    // Verificar se a senha está correta
    if (password == _AdminConfig.getOperPassword()) {
        // Qualquer usuário pode se tornar operador com a senha correta
        int currentGrade = userIt->second.getGrade();
        
        if (currentGrade == 0) {
            // Já é admin principal
            std::string msg1 = ":" + _ServName + " NOTICE " + nickname + " :👑 Você já é o ADMINISTRADOR PRINCIPAL!\r\n";
            send(clientSocket, msg1.c_str(), msg1.length(), 0);
            
            std::string msg2 = ":" + _ServName + " NOTICE " + nickname + " :⚡ Grade 0 é superior ao Grade 1 (Operador)\r\n";
            send(clientSocket, msg2.c_str(), msg2.length(), 0);
        } else {
            // Promover para operador (grade 1)
            userIt->second.setGrade(1);
            
            std::string separator = ":" + _ServName + " NOTICE " + nickname + " :═══════════════════════════════════\r\n";
            send(clientSocket, separator.c_str(), separator.length(), 0);
            
            std::string successMsg1 = ":" + _ServName + " NOTICE " + nickname + " :🎉 PROMOÇÃO REALIZADA COM SUCESSO!\r\n";
            send(clientSocket, successMsg1.c_str(), successMsg1.length(), 0);
            
            std::string successMsg2 = ":" + _ServName + " NOTICE " + nickname + " :⚡ Você agora é OPERADOR (Grade 1)\r\n";
            send(clientSocket, successMsg2.c_str(), successMsg2.length(), 0);
            
            std::string successMsg3 = ":" + _ServName + " NOTICE " + nickname + " :📋 Pode usar: MODE +o, KICK, BAN\r\n";
            send(clientSocket, successMsg3.c_str(), successMsg3.length(), 0);
            
            send(clientSocket, separator.c_str(), separator.length(), 0);
            
            std::cout << "OPER: " << nickname << " promoted from Grade " << currentGrade << " to Grade 1 (Operator)" << std::endl;
        }
    } else {
        std::string errorMsg = ":" + _ServName + " NOTICE " + nickname + " :❌ Senha incorreta para OPER\r\n";
        send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        std::cout << "OPER: Failed attempt by " << nickname << " with wrong password" << std::endl;
    }
}

void Server::sendWelcomeMessage(int clientSocket, const User& user) {
    std::string nickname = user.getNick();
    int grade = user.getGrade();
    
    // Message de bienvenue IRC standard
    std::string welcomeMsg = ":" + _ServName + " 001 " + nickname + " :Bienvenue sur " + _ServName + "!\r\n";
    send(clientSocket, welcomeMsg.c_str(), welcomeMsg.length(), 0);
    
    // Envoyer comme messages du serveur (pas NOTICE)
    std::string separator = ":" + _ServName + " 002 " + nickname + " :═══════════════════════════════════\r\n";
    send(clientSocket, separator.c_str(), separator.length(), 0);
    
    // Statut de l'utilisateur avec emoji et couleur
    std::string statusMsg = ":" + _ServName + " 003 " + nickname + " :";
    
    switch(grade) {
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
    send(clientSocket, statusMsg.c_str(), statusMsg.length(), 0);
    
    // Description des privilèges
    std::string privMsg = ":" + _ServName + " 004 " + nickname + " :Privilèges: ";
    
    switch(grade) {
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
    send(clientSocket, privMsg.c_str(), privMsg.length(), 0);
    
    // Instructions spécifiques par grade
    if (grade == 0) {
        std::string adminMsg1 = ":" + _ServName + " 005 " + nickname + " :🎯 Vous avez le CONTRÔLE TOTAL du serveur!\r\n";
        send(clientSocket, adminMsg1.c_str(), adminMsg1.length(), 0);
        
        std::string adminMsg2 = ":" + _ServName + " 006 " + nickname + " :📋 Commandes: MODE, KICK, BAN, INVITE, TOPIC\r\n";
        send(clientSocket, adminMsg2.c_str(), adminMsg2.length(), 0);
    }
    else if (grade == 1) {
        std::string operMsg1 = ":" + _ServName + " 005 " + nickname + " :⚡ Vous êtes OPÉRATEUR du serveur!\r\n";
        send(clientSocket, operMsg1.c_str(), operMsg1.length(), 0);
        
        std::string operMsg2 = ":" + _ServName + " 006 " + nickname + " :📋 Commandes: MODE +o, KICK, BAN dans les canaux\r\n";
        send(clientSocket, operMsg2.c_str(), operMsg2.length(), 0);
    }
    else if (grade == 2) {
        std::string voiceMsg = ":" + _ServName + " 005 " + nickname + " :🔊 Vous avez la VOIX - vous pouvez parler dans les canaux modérés\r\n";
        send(clientSocket, voiceMsg.c_str(), voiceMsg.length(), 0);
    }
    else if (grade == 3) {
        std::string userMsg1 = ":" + _ServName + " 005 " + nickname + " :📈 Pour obtenir plus de privilèges:\r\n";
        send(clientSocket, userMsg1.c_str(), userMsg1.length(), 0);
        
        std::string userMsg2 = ":" + _ServName + " 006 " + nickname + " :   💼 OPER " + nickname + " <motdepasse> → Devenir Opérateur\r\n";
        send(clientSocket, userMsg2.c_str(), userMsg2.length(), 0);
        
        if (nickname == _AdminConfig.getMainAdmin()) {
            std::string adminHint = ":" + _ServName + " 007 " + nickname + " :   👑 ADMIN " + nickname + " <motdepasse> → Devenir Admin Principal\r\n";
            send(clientSocket, adminHint.c_str(), adminHint.length(), 0);
        }
    }
    
    // Ligne séparatrice finale
    std::string separator2 = ":" + _ServName + " 008 " + nickname + " :═══════════════════════════════════\r\n";
    send(clientSocket, separator2.c_str(), separator2.length(), 0);
    
    // Message additionnel qui apparaît comme privmsg du serveur (plus visible)
    std::string visibleMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :";
    
    switch(grade) {
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
    send(clientSocket, visibleMsg.c_str(), visibleMsg.length(), 0);
}

void Server::handleAdminCommand(int clientSocket, const std::string& line) {
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 3) {
        std::string errorMsg = ":" + _ServName + " 461 * ADMIN :Not enough parameters\r\n";
        send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    
    std::string username = tokens[1];
    std::string password = tokens[2];
    
    // Check if user exists
    std::map<int, User>::iterator userIt = _User.find(clientSocket);
    if (userIt == _User.end()) {
        return;
    }
    
    std::string nickname = userIt->second.getNick();
    
    // Validate admin credentials
    if (_AdminConfig.validateAdminCredentials(username, password)) {
        // Grant admin privileges
        userIt->second.setGrade(0);
        
        std::string separator = ":" + _ServName + " NOTICE " + nickname + " :═══════════════════════════════════\r\n";
        send(clientSocket, separator.c_str(), separator.length(), 0);
        
        std::string successMsg1 = ":" + _ServName + " NOTICE " + nickname + " :🎉 AUTENTICAÇÃO ADMIN REALIZADA!\r\n";
        send(clientSocket, successMsg1.c_str(), successMsg1.length(), 0);
        
        std::string successMsg2 = ":" + _ServName + " NOTICE " + nickname + " :👑 Você agora é ADMINISTRADOR PRINCIPAL\r\n";
        send(clientSocket, successMsg2.c_str(), successMsg2.length(), 0);
        
        std::string successMsg3 = ":" + _ServName + " NOTICE " + nickname + " :⚡ Grade 0 - CONTROLE TOTAL DO SERVIDOR\r\n";
        send(clientSocket, successMsg3.c_str(), successMsg3.length(), 0);
        
        std::string successMsg4 = ":" + _ServName + " NOTICE " + nickname + " :📋 Todos os comandos disponíveis!\r\n";
        send(clientSocket, successMsg4.c_str(), successMsg4.length(), 0);
        
        send(clientSocket, separator.c_str(), separator.length(), 0);
        
        std::cout << "ADMIN: " << nickname << " authenticated as server administrator" << std::endl;
    } else {
        std::string errorMsg = ":" + _ServName + " NOTICE " + nickname + " :❌ Credenciais de admin inválidas\r\n";
        send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        std::cout << "ADMIN: Failed authentication attempt by " << nickname << std::endl;
    }
}

void Server::displayAdminInfo() {
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

void Server::handleGradesCommand(int clientSocket, const std::string& /* line */) {
    std::map<int, User>::iterator userIt = _User.find(clientSocket);
    if (userIt == _User.end()) {
        return;
    }
    
    std::string nickname = userIt->second.getNick();
    
    // Cabeçalho
    std::string header = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :📊 GRADES DOS USUÁRIOS CONECTADOS\r\n";
    send(clientSocket, header.c_str(), header.length(), 0);
    
    std::string separator = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :═════════════════════════════════════\r\n";
    send(clientSocket, separator.c_str(), separator.length(), 0);
    
    // Lista todos os usuários conectados
    for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it) {
        std::string userNick = it->second.getNick();
        int grade = it->second.getGrade();
        
        std::string gradeMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :";
        
        switch(grade) {
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
        send(clientSocket, gradeMsg.c_str(), gradeMsg.length(), 0);
    }
    
    // Rodapé
    send(clientSocket, separator.c_str(), separator.length(), 0);
    
    std::string footer = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + nickname + " :💡 Use: SETGRADE <nick> <0-3> para alterar\r\n";
    send(clientSocket, footer.c_str(), footer.length(), 0);
}

void Server::handleSetGradeCommand(int clientSocket, const std::string& line) {
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 3) {
        std::string errorMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG * :❌ Uso: SETGRADE <nickname> <grade>\r\n";
        send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    
    // Verificar se quem executa tem privilégios
    std::map<int, User>::iterator adminIt = _User.find(clientSocket);
    if (adminIt == _User.end()) {
        return;
    }
    
    std::string adminNick = adminIt->second.getNick();
    int adminGrade = adminIt->second.getGrade();
    
    if (adminGrade != 0) {
        std::string errorMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + adminNick + " :❌ Seul l'Admin Principal (Grade 0) peut utiliser SETGRADE\r\n";
        send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    
    std::string targetNick = tokens[1];
    std::string gradeParam = tokens[2];
    
    // Remover # se estiver presente (usuário pode digitar #1 por engano)
    if (!gradeParam.empty() && gradeParam[0] == '#') {
        gradeParam = gradeParam.substr(1);
    }
    
    int newGrade = atoi(gradeParam.c_str());
    
    if (newGrade < 0 || newGrade > 3) {
        std::string errorMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + adminNick + " :❌ Le grade doit être 0, 1, 2 ou 3\r\n";
        send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    
    // Chercher l'utilisateur cible
    bool found = false;
    for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it) {
        if (it->second.getNick() == targetNick) {
            int oldGrade = it->second.getGrade();
            it->second.setGrade(newGrade);
            
            // Convertir int en char (compatible C++98)
            char oldGradeChar = '0' + oldGrade;
            char newGradeChar = '0' + newGrade;
            
            // Message pour l'admin
            std::string successMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + adminNick + " :✅ " + targetNick + " modifié du Grade " + 
                                    oldGradeChar + " au Grade " + newGradeChar + "\r\n";
            send(clientSocket, successMsg.c_str(), successMsg.length(), 0);
            
            // Notifier l'utilisateur cible avec switch
            std::string gradeName;
            switch(newGrade) {
                case 0: gradeName = "👑 Administrateur Principal"; break;
                case 1: gradeName = "⚡ Opérateur"; break;
                case 2: gradeName = "🔊 Voix"; break;
                case 3: gradeName = "👤 Utilisateur Normal"; break;
                default: gradeName = "❓ Inconnu"; break;
            }
            
            std::string notifyMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + targetNick + " :🎉 Votre grade a été modifié en: " + 
                                   gradeName + " (Grade " + newGradeChar + ")\r\n";
            send(it->first, notifyMsg.c_str(), notifyMsg.length(), 0);
            
            std::cout << "SETGRADE: " << adminNick << " changed " << targetNick << " from Grade " << oldGrade << " to Grade " << newGrade << std::endl;
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::string errorMsg = ":" + _ServName + "!" + _ServName + "@" + _ServName + " PRIVMSG " + adminNick + " :❌ Usuário '" + targetNick + "' não encontrado\r\n";
        send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
    }
}