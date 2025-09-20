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

    /*insertion of channel*/
    this->_Chan.insert(std::make_pair(0, Channel(0, "default")));

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

    // Check ban list by nick (can be improved to check by IP or username)
    std::string incomingNick = ""; // Will be set after NICK command
    // Add user temporarily
    this->_User.insert(std::make_pair(clientSocket, User(clientSocket)));
    // Wait for NICK command in HandleClientData, then check ban list before authenticating
    // ...existing code...


   // if (DEBUG == true)
    //    std::cout << "[DEBUG] getSocket: " << this->_UserObj.getSocket() << std::endl;


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

        else if (line.find("USER ") == 0)
        {
            user.setName(GetName(line, user.getAuth()));
            if (DEBUG == true)
                std::cout << "[DEBUG] USER: " << user.getName() << std::endl;
        }
        else if (line.find("NAMES ") == 0)
        {
            user.setName(GetName(line, user.getAuth()));
            if (DEBUG == true)
                std::cout << "[DEBUG] NAMES: " << user.getName() << std::endl;
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
        //Ban = +b mask
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
        
            std::cout << "Channel: " << chanName << std::endl;
            std::cout << "Param: " << param << std::endl;
        
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
        
            std::cout << "Mode: " << mode << std::endl;
            std::cout << "Mask: " << mask << std::endl;
        
            // 3) Gestion +b / -b
            if (mode == "+b" && !mask.empty()) 
                handleBanCommand(clientSocket, chanName, mask);
            /*else if (mode == "-b" && !mask.empty())
                handleUnbanCommand(clientSocket, chanName, mask);
            else if (mode == "+b" && mask.empty())
                sendBanList(clientSocket, chanName);*/
        
            continue;
        }
        else if (line.find("PRIVMSG ") == 0)
        {
            if (user.getIdChan() >= 0)
            {
                std::map<int, Channel>::iterator chanIt = this->_Chan.find(user.getIdChan());

                size_t pos = line.find(" :");
                std::string msg;
    
                if (pos != std::string::npos)
                    msg = line.substr(pos + 2);
                else
                    msg = "";
    
                std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG #" + chanIt->second.GetName() + " :" + msg + "\r\n";
    
                if (DEBUG == true)
                    std::cout << "[DEBUG] ircMsg: " << ircMsg << std::endl;
    
                chanIt->second.Broadcast(ircMsg, clientSocket);
    
                std::cout << CYAN << chanIt->second.GetName() << " / " << YELLOW << user.getName() << RESET << ": " << msg << std::endl;
            }
            else
            {
                size_t pos = line.find(" :");
                std::string msg;
    
                if (pos != std::string::npos)
                    msg = line.substr(pos + 2);
                else
                    msg = "";
            
                std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + user.getNick() + " :Vous avez été kick ou ban. Le message n'a pas été envoyer (" + msg + ")\r\n";
                send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);

                std::cout << CYAN << "USER KICKED/BAN MSG RECEP" << " / " << YELLOW << user.getName() << RESET << ": " << msg << std::endl;
            }
        }
	}

    if (!user.getNick().empty() && !user.getName().empty() && !user.getPass().empty())
    {
        // Ban check before authentication
        /*
        for (size_t i = 0; i < this->_BanList.size(); ++i)
        {
            if (user.getNick() == this->_BanList[i])
            {
                send(clientSocket, "You are banned from this server.\n", 30, 0);
                user.closeSocket();
                FD_CLR(clientSocket, &_Readfds);
                this->_User.erase(clientSocket);
                std::cout << "[INFO] Banned user " << user.getNick() << " tried to connect." << std::endl;
                return;
            }
        }
        */
        if (!user.getAuth())
        {
            if (!PassCont(user.getPass()))
            {
                send(clientSocket, "Invalid password\n", 17, 0);
                close(clientSocket);
                this->_User.erase(clientSocket);
                return;
            }
    
            user.setGrade(0);
            user.setAuth(true);
            user.setIdChan(0);
    
            std::cout << "[INFO] User " << user.getNick() << "@" << user.getName() << " authentifié !" << std::endl;


            std::map<int, Channel>::iterator chanIt = this->_Chan.find(0);
            if (chanIt != this->_Chan.end())
            {
                // Gestion du ban a la tentative de join le chan par default
                if (chanIt->second.GetUserBan(user) == true)
                {
                    send(clientSocket, "You are banned from this channel.\n", 30, 0);
                    
                    std::cout << RED << "[INFO] Banned user " << user.getNick() << " tried to connect to chan: " << chanIt->second.GetName() << std::endl;
                    
                    return;
                }

                chanIt->second.AddUser(user);
                std::cout << "[INFO] User " << user.getNick() << " ajouté au channel " << chanIt->second.GetName() << std::endl;
                
                std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";

                chanIt->second.BroadcastAll(joinMsg);
            }

        }
    }
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
            ++it;
            if (FD_ISSET(clientSock, &_Readfds))
                HandleClientData(clientSock);
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


/*
Kick& Server::getBanList() {
    return _kick;
}
*/

/*
/KICK <Nickname>
/quote UNBAN <Nickname>
*/

