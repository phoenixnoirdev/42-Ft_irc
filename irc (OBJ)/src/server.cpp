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

    this->_Chan.insert(std::make_pair(0, Channel(0, "default")));

    Init();
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

void Server::ShutSign()
{
    if (!_ServeurOn)
        return;

    this->_ServeurOn = false;
}

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

    this->_User.insert(std::make_pair(clientSocket, User(clientSocket)));

    std::cout << "[DEBUG] getSocket: " << this->_UserObj.getSocket() << std::endl;

    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
    std::cout << GREEN << inet_ntoa(client.sin_addr) << YELLOW
              << " connected on port " << GREEN << ntohs(client.sin_port)
              << RESET << std::endl;
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
}

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

    // Récupère une référence vers ton User
    User &user = this->_User[clientSocket];

    // Concatène dans son buffer
    user.setRcvBuff(user.getRcvBuff() + std::string(buf, bytesRecv));

    // Cherche des lignes terminées par \r\n
    std::string &data = user.getRcvBuff();
    size_t pos;
    while ((pos = data.find("\r\n")) != std::string::npos)
    {
        std::string line = data.substr(0, pos);
        data.erase(0, pos + 2);

        if (line.find("NICK ") == 0)
        {
            user.setNick(GetNick(line));
            std::cout << "[DEBUG] NICK: " << user.getNick() << std::endl;
        }
        else if (line.find("USER ") == 0)
        {
            user.setName(GetName(line));
            std::cout << "[DEBUG] USER: " << user.getName() << std::endl;
        }
        else if (line.find("PASS ") == 0)
        {
            user.setPass(GetPwd(line));
            std::cout << "[DEBUG] PASS: " << user.getPass() << std::endl;
        }
        else if (line.find("PRIVMSG ") == 0)
        {
            std::map<int, Channel>::iterator chanIt = this->_Chan.find(user.getIdChan());

            size_t pos = line.find(" :");
            std::string msg;
            if (pos != std::string::npos)
                msg = line.substr(pos + 2);
            else
                msg = "";


            std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG #" + chanIt->second.GetName() + " :" + msg + "\r\n";

            chanIt->second.Broadcast(ircMsg, clientSocket);

            std::cout << CYAN << chanIt->second.GetName() << " / " << YELLOW << user.getName() << RESET << ": " << msg << std::endl;
        }
    }

    // Vérifie si l'utilisateur est authentifié
    if (!user.getNick().empty() && !user.getName().empty() && !user.getPass().empty())
    {
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

            // → Ajout automatique dans le channel "default"
            std::map<int, Channel>::iterator chanIt = this->_Chan.find(0);
            if (chanIt != this->_Chan.end())
            {
                chanIt->second.AddUser(user);
                std::cout << "[INFO] User " << user.getNick() << " ajouté au channel " << chanIt->second.GetName() << std::endl;
                
                // Préparer le JOIN message
                std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";

                // Envoyer à tous les users du channel
                chanIt->second.BroadcastJoin(joinMsg);
            }


        }
        else
        {

        }
    }
}

//Version tmp
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

        // select bloquant
        int ret = select(maxFd + 1, &_Readfds, NULL, NULL, NULL);

        if (ret < 0)
        {
            if (errno == EINTR) // signal reçu → sortir de la boucle
                break;
            std::cerr << "select failed: " << strerror(errno) << std::endl;
            break;
        }

        // Nouveau client ?
        if (FD_ISSET(this->_Listening, &_Readfds))
            AcceptClient();

        // Données sur les clients
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


bool Server::PassCont(const std::string& str)
{
    if (str == this->_Pass)
        return true;
    
    return false;
}

std::string Server::GetPwd(const std::string& str)
{
    if (str.substr(0, 4) == "PASS")
    {
        std::string::size_type spacePos = str.find(' ');
        if (spacePos != std::string::npos)
        {
            std::string pass = str.substr(spacePos + 1);
            //std::cout << "[DEBUG] PASS trouvé: " << pass << std::endl;
            return pass;
        }
    }
    return "";
}

std::string Server::GetNick(const std::string& str)
{
    if (str.substr(0, 4) == "NICK")
    {
        std::string::size_type spacePos = str.find(' ');
        if (spacePos != std::string::npos)
        {
            std::string nick = str.substr(spacePos + 1);
            //std::cout << "[DEBUG] NICK trouvé: " << nick << std::endl;
            return nick;
        }
    }
    return "";
}

std::string Server::GetName(const std::string& str)
{
    if (str.substr(0, 4) == "USER")
    {
        std::string::size_type colonPos = str.find(':');
        if (colonPos != std::string::npos)
        {
            std::string user = str.substr(colonPos + 1);
            //std::cout << "[DEBUG] USER trouvé: " << user << std::endl;
            return user;
        }
    }
    return "";
}