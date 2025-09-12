/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "bot.hpp"

//=================
//Constructeur
//=================

Bot::Bot(std::string ip, std::string port, std::string pass)
{
    srand(time(NULL)); 

    this->_Port = Utils::PortConvert(port);
    this->_Ip = Utils::IpConvert(ip);
    this->_Pass = pass;
    this->_BotOn = true;


    initMsgFr();
    initMsgEng();


    connectToServer();
    loginBot();
    run();
}

Bot::~Bot()
{
    return;
}

//===============
//Fonctions public
//===============

/**
 * @brief Arrête le bot en mettant le drapeau _BotOn à false.
 */
void Bot::shutSign()
{
    if (!_BotOn)
        return;

    this->_BotOn = false;
}

/**
 * @brief Ferme proprement le serveur en nettoyant utilisateurs, canaux et sockets.
 */
void Bot::shutdown()
{
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;


    std::cout << YELLOW << "Close Socket" << RESET << std::endl;
    if (this->_Sock != -1)
        close(this->_Sock);
    std::cout << YELLOW << "Socket Closed" << RESET << std::endl;


	std::cout << YELLOW << "Fermeture propre du Bot..." << RESET << std::endl;
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
}


//===============
//Fonctions private
//===============

void Bot::connectToServer()
{
    this->_Sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_Sock == -1)
        std::cerr << RED << "Error: Can't create a socket!" << RESET << std::endl; 
    
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(this->_Port);
    serv_addr.sin_addr   = this->_Ip;

    if (connect(this->_Sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        std::cerr << RED << "Error: Can't bind IP/port." << RESET << std::endl; 

    
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
    std::cout << GREEN << "Bot connecté au serveur IRC" << RESET << std::endl;
    std::cout << YELLOW << "-- IP: " << CYAN << inet_ntoa(this->_Ip) << RESET << std::endl;
    std::cout << YELLOW << " Port: " << CYAN << this->_Port << RESET << std::endl;
    std::cout << YELLOW << "-- Pwd: " << CYAN << this->_Pass <<  RESET << std::endl;
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
}

void Bot::loginBot()
{
    if (!this->_Pass.empty())
        sendRaw("PASS " + this->_Pass + "\r\n");

    sendRaw(std::string("NICK ") + NICK + "\r\n");
    sendRaw(std::string("USER ") + NAME + " 0 * :" + NAME + "\r\n");
    sendRaw(std::string("JOIN ") + CHAN_NAME + "\r\n");
    sendRaw(std::string("NAMES ") + CHAN_NAME + "\r\n");
}

void Bot::run()
{
    char buffer[512];
    int compt = 0;
    int maxFd = this->_Sock;
    fd_set readfds;

    while (this->_BotOn)
    {
        FD_ZERO(&readfds);
        FD_SET(this->_Sock, &readfds);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(maxFd + 1, &readfds, NULL, NULL, &tv);
        if (ret < 0)
            break;
        if (ret == 0)
            continue;

        if (FD_ISSET(this->_Sock, &readfds))
        {
            std::memset(buffer, 0, sizeof(buffer));
            ssize_t n = read(this->_Sock, buffer, sizeof(buffer) - 1);
            if (n <= 0)
                break;

            std::string msg(buffer);
            
            if(DEBUG == true)
                std::cout << "[SERVER] " << msg;


            if (compt < NBMSG)
                compt++;
            else
            {
                sendMessage();
                compt = 0;
                std::cout << YELLOW << "[BOT] Annonce envoyer " << RESET << std::endl;
            }
            

            if(DEBUG == true)
                std::cout << YELLOW << "[BOT] Prochaine annonce dans " << CYAN << compt << "/" << NBMSG << RESET << std::endl;

            // PING/PONG
            if (msg.find("PING") == 0)
                sendRaw("PONG " + msg.substr(5));
        }
    }

    shutdown();
}

void Bot::sendMessage()
{
    int randomValue = 0;

    if (std::string(BOTLANG) == "FR")
    {
        randomValue = rand() % this->_MsgFR.size();
        sendRaw("PRIVMSG " + std::string(CHAN_NAME) + " :" + this->_MsgFR[randomValue] + "\r\n");
    }

    if (std::string(BOTLANG) == "ENG")
    {
        randomValue = rand() % this->_MsgENG.size();
        sendRaw("PRIVMSG " + std::string(CHAN_NAME) + " :" + this->_MsgENG[randomValue] + "\r\n");
    }
}


void Bot::sendRaw(const std::string& msg)
{
    if (write(this->_Sock, msg.c_str(), msg.size()) < 0)
        throw std::runtime_error("Error writing to socket");
}

void Bot::initMsgFr()
{
    this->_MsgFR.push_back("Hey ! Si tu parles tout seul, au moins que ça reste sur ce canal !");
    this->_MsgFR.push_back("Petit conseil : le café ne se partage pas par IRC, désolé !");
    this->_MsgFR.push_back("Si tu vois ce message, c’est que tu existes… bravo !");
    this->_MsgFR.push_back("Attention : tout message idiot sera immortalisé par le bot !");
    this->_MsgFR.push_back("Annonce automatique : même les bots ont besoin de blagues.");
}

void Bot::initMsgEng()
{
    this->_MsgENG.push_back("Hey! Talking to yourself is fine, but keep it in this channel!");
    this->_MsgENG.push_back("Pro tip: coffee doesn’t send over IRC, sorry!");
    this->_MsgENG.push_back("If you see this message, congrats! You exist!");
    this->_MsgENG.push_back("Warning: any silly message will be immortalized by the bot!");
    this->_MsgENG.push_back("Automated announcement: even bots enjoy a good joke."); 
}