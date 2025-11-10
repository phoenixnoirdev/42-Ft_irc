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
    this->_User = User();

    Init();
    //AcceptClient();
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

void Server::Shutdown()
{
    this->_ServeurOn = false;


    this->_User.closeAllSockets();

    this->_User.clear();


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

    this->_ClientsInfo[clientSocket] = ClientInfo();
    this->_PendingData[clientSocket] = "";

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
        if (bytesRecv == 0)
            std::cerr << "Client disconnected" << std::endl;
        else
            std::cerr << "recv failed" << std::endl;

        close(clientSocket);
        this->_ClientsInfo.erase(clientSocket);
        this->_PendingData.erase(clientSocket);
        return;
    }


    this->_PendingData[clientSocket] += std::string(buf, bytesRecv);
    std::string &data = this->_PendingData[clientSocket];
    ClientInfo &info = this->_ClientsInfo[clientSocket];

    size_t pos;
    while ((pos = data.find("\r\n")) != std::string::npos)
    {
        std::string line = data.substr(0, pos);
        data.erase(0, pos + 2);

        if (line.find("NICK ") == 0)
        {
            info.nick = GetNick(line);
            info.gotNick = true;
            std::cout << "[DEBUG] NICK: " << info.nick << std::endl;
        }
        else if (line.find("USER ") == 0)
        {
            info.user = GetName(line);
            info.gotUser = true;
            std::cout << "[DEBUG] USER: " << info.user << std::endl;
        }
        else if (line.find("PASS ") == 0)
        {
            info.pass = GetPwd(line);
            info.gotPass = true;
            std::cout << "[DEBUG] PASS: " << info.pass << std::endl;
        }
    }

    if (info.gotNick && info.gotUser && info.gotPass)
    {
        if (!PassCont(info.pass))
        {
            send(clientSocket, "Invalid password\n", 17, 0);
            close(clientSocket);
        }
        else
        {
            this->_User.addUser(info.user, info.nick, info.pass, 5, clientSocket);
        }

        _ClientsInfo.erase(clientSocket);
        _PendingData.erase(clientSocket);
    }
}

//Version tmp
void Server::Run()
{
    fd_set readfds;
    int maxFd;

    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(this->_Listening, &readfds);
        maxFd = this->_Listening;

        // Ajouter les clients en handshake
        for (std::map<int, ClientInfo>::iterator it = _ClientsInfo.begin();
             it != _ClientsInfo.end(); ++it)
        {
            FD_SET(it->first, &readfds);
            if (it->first > maxFd)
                maxFd = it->first;
        }

        /*
        // Ajouter les clients authentifiés (_User)
        const std::map<int, UserInfo>& activeUsers = this->_User.getUsers();
        for (std::map<int, UserInfo>::const_iterator it = activeUsers.begin();
             it != activeUsers.end(); ++it)
        {
            FD_SET(it->first, &readfds);
            if (it->first > maxFd)
                maxFd = it->first;
        }
        */
       
        // Attente d'activité (lecture uniquement)
        if (select(maxFd + 1, &readfds, NULL, NULL, NULL) < 0)
        {
            Error::ErrorServ(9, "select failed");
            continue;
        }

        // Nouveau client en attente ?
        if (FD_ISSET(this->_Listening, &readfds))
        {
            AcceptClient();
        }

        // Données sur un client en handshake ?
        for (std::map<int, ClientInfo>::iterator it = _ClientsInfo.begin();
             it != _ClientsInfo.end(); )
        {
            int clientSock = it->first;
            ++it; // avancer avant potentielle suppression

            if (FD_ISSET(clientSock, &readfds))
            {
                HandleClientData(clientSock);
            }
        }
        /*
        // Données sur un client authentifié ?
        for (std::map<int, UserInfo>::const_iterator it = activeUsers.begin();
             it != activeUsers.end(); ++it)
        {
            int clientSock = it->first;
            if (FD_ISSET(clientSock, &readfds))
            {
                this->_User.HandleUserData(clientSock);
            }
        }
            */
    }
}


/*
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

        
        //fonction controle du password
        if (PassCont() == false)
        {
			std::cerr << "Wrong Password " << std::endl;
			break ;
        }


		std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl; 
		
		//resend message
		//send(this->clientSocket, buf, bytesRecv , 0);
		send(this->_ClientSocket, "Hello, world!\n", 14, 0) ;

		if (send(this->_ClientSocket, buf, bytesRecv , 0) == -1)
		{
			std::cerr << "Send failed" << std::endl;
			break ;
		}
	
	}

    Shutdown();
}
*/

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