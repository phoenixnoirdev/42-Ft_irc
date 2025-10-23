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

/**
 * @brief Constructeur de la classe Bot.
 *
 * Initialise un bot avec une IP, un port et un mot de passe donnés.
 * Configure les messages en français et anglais, puis se connecte
 * au serveur, s'authentifie et démarre son exécution.
 *
 * @param ip   Adresse IP du serveur sous forme de chaîne.
 * @param port Numéro de port du serveur sous forme de chaîne.
 * @param pass Mot de passe utilisé pour l'authentification.
 */
Bot::Bot(std::string ip, std::string port, std::string pass, std::string chan)
{
    srand(time(NULL)); 

    this->_Port = Utils::PortConvert(port);
    this->_Ip = Utils::IpConvert(ip);
    this->_Pass = pass;
    this->_Chan = chan;
    this->_BotOn = true;


    initMsgFr();
    initMsgEng();


    connectToServer();
    loginBot();
    run();
}

/**
 * @brief Destructeur de la classe Bot.
 *
 * Libère les ressources utilisées par le bot avant sa destruction.
 */
Bot::~Bot()
{
    return;
}

//===============
//Fonctions public
//===============

/**
 * @brief Désactive le bot en cours d'exécution.
 *
 * Met l'état interne du bot à inactif si celui-ci est actif.
 */
void Bot::shutSign()
{
    if (!_BotOn)
        return;

    this->_BotOn = false;
}

/**
 * @brief Ferme proprement le bot et sa connexion.
 *
 * Affiche les étapes de fermeture et ferme le socket si ouvert.
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

/**
 * @brief Connecte le bot au serveur IRC spécifié.
 *
 * Crée un socket TCP IPv4 et initialise la structure d'adresse du
 * serveur avec l'IP et le port configurés dans le bot. Tente ensuite
 * d'établir une connexion au serveur IRC. Affiche des messages
 * d'erreur en cas d'échec de création ou de connexion du socket.
 *
 * Si la connexion réussit, affiche dans la console les informations
 * de connexion (adresse IP, port et mot de passe utilisé) pour suivi.
 *
 * @note Cette méthode doit être appelée avant toute tentative de
 * communication avec le serveur.
 */
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
    std::cout << YELLOW << "-- Port: " << CYAN << this->_Port << RESET << std::endl;
    std::cout << YELLOW << "-- Pwd: " << CYAN << this->_Pass <<  RESET << std::endl;
    std::cout << YELLOW << "-- Chan: " << CYAN << this->_Chan <<  RESET << std::endl;
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
}

/**
 * @brief Authentifie et connecte le bot au serveur IRC.
 *
 * Envoie au serveur IRC les commandes nécessaires à la connexion :
 * - PASS : envoie le mot de passe si défini.
 * - NICK : définit le pseudonyme du bot.
 * - USER : envoie les informations utilisateur du bot.
 * - JOIN : rejoint le canal spécifié.
 * - NAMES : demande la liste des utilisateurs présents.
 *
 * @note Cette méthode doit être appelée après l'établissement de la
 * connexion avec le serveur via connectToServer().
 */
void Bot::loginBot()
{
    if (!this->_Pass.empty())
        sendRaw("PASS " + this->_Pass + "\r\n");

    sendRaw(std::string("NICK ") + NICK + "\r\n");
    sendRaw(std::string("USER ") + NAME + " 0 * :" + NAME + "\r\n");
    sendRaw(std::string("JOIN ") + "#" + this->_Chan + "\r\n");
    sendRaw(std::string("NAMES ") + "#" + this->_Chan + "\r\n");
}

/**
 * @brief Boucle principale d'exécution du bot IRC.
 *
 * Écoute les messages entrants du serveur en utilisant `select` pour
 * surveiller le socket du bot. Lit les données reçues et les affiche
 * si le mode DEBUG est activé. Gère les commandes PING/PONG pour
 * maintenir la connexion active. Compte les messages reçus et envoie
 * automatiquement une annonce après un nombre prédéfini de messages.
 *
 * La boucle continue tant que le bot est actif (_BotOn == true).
 * À la fin ou en cas d'erreur, appelle shutdown() pour fermer
 * proprement le bot et son socket.
 */
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


            if (msg.find("PING") == 0)
                sendRaw("PONG " + msg.substr(5));
        }
    }

    shutdown();
}

/**
 * @brief Envoie un message aléatoire du bot dans le canal IRC.
 *
 * Sélectionne un message aléatoire dans la liste correspondant à
 * la langue du bot (FR ou ENG) et l'envoie sur le canal défini
 * par CHAN_NAME en utilisant la commande PRIVMSG.
 *
 * @note Utilise la fonction sendRaw pour l'envoi direct au serveur.
 */
void Bot::sendMessage()
{
    int randomValue = 0;

    if (std::string(BOTLANG) == "FR")
    {
        randomValue = rand() % this->_MsgFR.size();
        sendRaw("PRIVMSG #" + this->_Chan + " :" + this->_MsgFR[randomValue] + "\r\n");
    }

    if (std::string(BOTLANG) == "ENG")
    {
        randomValue = rand() % this->_MsgENG.size();
        sendRaw("PRIVMSG #" + this->_Chan + " :" + this->_MsgENG[randomValue] + "\r\n");
    }
}

/**
 * @brief Envoie une chaîne de caractères brute au serveur IRC.
 *
 * Écrit directement la chaîne `msg` sur le socket du bot. Si
 * l'écriture échoue, lance une exception std::runtime_error.
 *
 * @param msg Chaîne de caractères à envoyer au serveur.
 */
void Bot::sendRaw(const std::string& msg)
{
    if (write(this->_Sock, msg.c_str(), msg.size()) < 0)
        throw std::runtime_error("Error writing to socket");
}

/**
 * @brief Initialise la liste des messages en français du bot.
 *
 * Remplit le vecteur _MsgFR avec des messages prédéfinis
 * humoristiques ou informatifs que le bot pourra envoyer
 * automatiquement sur le canal IRC.
 */
void Bot::initMsgFr()
{
    this->_MsgFR.push_back("Hey ! Si tu parles tout seul, au moins que ça reste sur ce canal !");
    this->_MsgFR.push_back("Petit conseil : le café ne se partage pas par IRC, désolé !");
    this->_MsgFR.push_back("Si tu vois ce message, c’est que tu existes… bravo !");
    this->_MsgFR.push_back("Attention : tout message idiot sera immortalisé par le bot !");
    this->_MsgFR.push_back("Annonce automatique : même les bots ont besoin de blagues.");
}

/**
 * @brief Initialise la liste des messages en anglais du bot.
 *
 * Remplit le vecteur _MsgENG avec des messages prédéfinis
 * humoristiques ou informatifs que le bot pourra envoyer
 * automatiquement sur le canal IRC.
 */
void Bot::initMsgEng()
{
    this->_MsgENG.push_back("Hey! Talking to yourself is fine, but keep it in this channel!");
    this->_MsgENG.push_back("Pro tip: coffee doesn’t send over IRC, sorry!");
    this->_MsgENG.push_back("If you see this message, congrats! You exist!");
    this->_MsgENG.push_back("Warning: any silly message will be immortalized by the bot!");
    this->_MsgENG.push_back("Automated announcement: even bots enjoy a good joke."); 
}