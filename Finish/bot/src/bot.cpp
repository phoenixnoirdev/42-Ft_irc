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
 * Initialise le bot IRC avec les informations de connexion et
 * démarre sa boucle principale.
 * 
 * Étapes effectuées :
 * 1. Conversion du port et de l'adresse IP.
 * 2. Stockage du mot de passe et du canal cible.
 * 3. Initialisation des messages en français et en anglais.
 * 4. Connexion au serveur IRC.
 * 5. Authentification du bot.
 * 6. Démarrage de la boucle principale du bot.
 * 
 * @param ip Adresse IP du serveur IRC.
 * @param port Port du serveur IRC sous forme de chaîne de caractères.
 * @param pass Mot de passe pour la connexion au serveur IRC.
 * @param chan Canal IRC sur lequel le bot doit se connecter.
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
 * Nettoie les ressources allouées par le bot.
 * Actuellement, aucune opération spécifique n'est nécessaire.
 */
Bot::~Bot()
{
    return;
}

//===============
//Fonctions public
//===============

/**
 * @brief Gère la fermeture propre du bot suite à un signal.
 * 
 * Cette fonction désactive le bot en mettant le flag interne `_BotOn`
 * à `false`, empêchant ainsi la boucle principale de continuer à s’exécuter.
 * 
 * @note Si le bot est déjà arrêté, la fonction ne fait rien.
 */
void Bot::shutSign()
{
    if (!_BotOn)
        return;

    this->_BotOn = false;
}

/**
 * @brief Ferme proprement le bot et libère les ressources réseau.
 * 
 * Cette fonction affiche des messages d’état indiquant la fermeture du socket
 * et la terminaison propre du bot.  
 * Si le socket est valide (`_Sock != -1`), il est fermé avant la fin.
 * 
 * @note Cette méthode est appelée lors de la terminaison du programme ou
 *       lorsqu’un signal d’interruption (SIGINT) est reçu.
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
 * @brief Établit la connexion entre le bot et le serveur IRC.
 *
 * Cette fonction crée un socket TCP et tente de se connecter au serveur
 * en utilisant l’adresse IP et le port spécifiés.  
 * En cas d’échec lors de la création ou de la connexion du socket, 
 * un message d’erreur est affiché sur la sortie d’erreur standard.
 *
 * Une fois la connexion établie, les informations de connexion 
 * (IP, port, mot de passe et canal) sont affichées dans la console.
 *
 * @warning Aucun arrêt immédiat n’est effectué en cas d’échec de connexion.
 *          Il serait préférable de lever une exception ou de retourner un
 *          code d’erreur pour éviter un comportement indéfini.
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
 * @brief Authentifie le bot et rejoint le canal IRC.
 *
 * Cette fonction envoie au serveur IRC les commandes nécessaires pour :
 * - fournir le mot de passe (si défini) ;
 * - définir le pseudo (NICK) ;
 * - enregistrer le bot comme utilisateur (USER) ;
 * - rejoindre le canal spécifié ;
 * - récupérer la liste des utilisateurs présents (NAMES).
 *
 * @note Les commandes sont envoyées en texte brut via la méthode sendRaw().
 * @warning Cette fonction suppose que la connexion au serveur est déjà établie.
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
 * @brief Boucle principale du bot IRC.
 *
 * Cette fonction maintient la connexion au serveur et gère :
 * - la réception des messages du serveur ;
 * - la réponse automatique aux requêtes PING (pour éviter la déconnexion) ;
 * - l’envoi périodique de messages d’annonce ;
 * - la détection des erreurs de lecture ou de fermeture du socket.
 *
 * @details
 * La fonction utilise `select()` pour surveiller le socket et gérer les entrées
 * sans blocage.  
 * Un compteur `compt` est utilisé pour déterminer quand envoyer une annonce
 * automatisée.  
 * Si la constante `DEBUG` est activée, les messages reçus et l’état interne
 * sont affichés dans la console.
 *
 * @note La boucle principale s’arrête lorsque `_BotOn` devient `false`
 * (par signal ou erreur de connexion).
 *
 * @see Bot::shutdown(), Bot::sendMessage(), Bot::sendRaw()
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
 * @brief Envoie un message aléatoire sur le canal IRC.
 *
 * Cette fonction choisit aléatoirement un message prédéfini dans le tableau
 * correspondant à la langue du bot (`_MsgFR` ou `_MsgENG`), puis l’envoie
 * au canal défini par `_Chan`.
 *
 * @details
 * - La langue utilisée dépend de la constante `BOTLANG`.
 * - Le message est envoyé via la commande IRC `PRIVMSG`.
 * - L’ajout de "\r\n" en fin de message est nécessaire pour respecter
 *   le protocole IRC.
 *
 * @note Si `BOTLANG` ne correspond à aucune des langues connues (FR/ENG),
 *       aucun message n’est envoyé.
 *
 * @see Bot::sendRaw()
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
 * @brief Envoie une commande brute au serveur IRC.
 *
 * Cette fonction écrit directement le contenu de `msg` sur le socket
 * connecté au serveur IRC. Elle est utilisée pour transmettre toutes
 * les commandes et messages IRC tels que `PASS`, `NICK`, `JOIN`,
 * `PRIVMSG`, etc.
 *
 * @param msg Chaîne contenant la commande IRC complète à envoyer.
 *
 * @throw std::runtime_error Si l’écriture sur le socket échoue.
 *
 * @note Aucun contrôle de format n’est effectué sur `msg`. Il doit donc
 *       inclure les caractères de fin de ligne requis par le protocole
 *       IRC (`\r\n`).
 *
 * @see Bot::connectToServer(), Bot::loginBot(), Bot::sendMessage()
 */
void Bot::sendRaw(const std::string& msg)
{
    if (write(this->_Sock, msg.c_str(), msg.size()) < 0)
        throw std::runtime_error("Error writing to socket");
}

/**
 * @brief Initialise la liste des messages automatiques en français.
 *
 * Cette fonction remplit le vecteur `_MsgFR` avec plusieurs messages
 * humoristiques ou informatifs que le bot enverra de manière aléatoire
 * sur le canal IRC.
 *
 * @note Les messages sont codés en dur dans la fonction. Ils peuvent être
 *       modifiés ou complétés selon le ton souhaité pour le bot.
 *
 * @see Bot::sendMessage()
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
 * @brief Initializes the list of automatic English messages.
 *
 * This function fills the `_MsgENG` vector with several humorous or
 * informational messages that the bot will send randomly in the IRC channel.
 *
 * @note The messages are hardcoded and can be modified or extended to suit
 *       the bot’s personality or purpose.
 *
 * @see Bot::sendMessage()
 */
void Bot::initMsgEng()
{
    this->_MsgENG.push_back("Hey! Talking to yourself is fine, but keep it in this channel!");
    this->_MsgENG.push_back("Pro tip: coffee doesn’t send over IRC, sorry!");
    this->_MsgENG.push_back("If you see this message, congrats! You exist!");
    this->_MsgENG.push_back("Warning: any silly message will be immortalized by the bot!");
    this->_MsgENG.push_back("Automated announcement: even bots enjoy a good joke."); 
}