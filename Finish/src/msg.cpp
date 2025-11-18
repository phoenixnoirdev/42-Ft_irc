/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msg.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:19 by phkevin           #+#    #+#             */
/*   Updated: 2025/11/18 12:09:54 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"

/**
 * @brief Diffuse un message d'un utilisateur à tous les membres d'un canal IRC.
 *
 * @param clientSocket Le descripteur de socket du client émetteur.
 * @param user         Référence vers l'utilisateur envoyant le message.
 * @param line         Ligne complète contenant la commande PRIVMSG et le message.
 * @param idchan       Identifiant interne du canal concerné.
 *
 * @details
 * Cette fonction :
 * - Vérifie si l'utilisateur a le droit de parler sur le canal (ex : mode +m).
 * - Extrait le message du format IRC (après " :").
 * - Formate le message complet selon la syntaxe IRC :
 *   @code
 *   :<nick>!~<user>@localhost PRIVMSG #<channel> :<message>\r\n
 *   @endcode
 * - Diffuse le message à tous les membres du canal via Broadcast().
 * - Affiche également le message sur la sortie standard pour le suivi serveur.
 *
 * En cas de restriction (canal modéré, utilisateur sans voix/opérateur),
 * un message d'erreur "404 Cannot send to channel (moderated)" est renvoyé
 * à l'expéditeur.
 *
 * @note
 * Le message est ignoré si le texte après " :" est vide.
 *
 * @see Channel::canSpeak()
 * @see Channel::Broadcast()
 */
void Server::handleBrodcastMsgChann(int clientSocket, User& user, std::string line, int idchan)
{
    std::map<int, Channel>::iterator chanIt = this->_Chan.find(idchan);
    
    if (!chanIt->second.canSpeak(user))
    {
        std::string err = ":localhost 404 " + user.getNick() + " " + chanIt->second.GetName() + " :Cannot send to channel (moderated)\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, err.c_str(), err.size(), 0);
        return;
    }
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

/**
 * @brief Envoie un message privé entre deux utilisateurs connectés.
 *
 * @param user Référence vers l'utilisateur émetteur du message.
 * @param line Ligne brute contenant la commande PRIVMSG et le contenu du message.
 *
 * @details
 * Cette fonction gère la commande IRC :
 *   @code
 *   PRIVMSG <nickname> :<message>
 *   @endcode
 *
 * Étapes du traitement :
 * - Extrait le pseudo du destinataire à partir de la ligne reçue.
 * - Extrait le message après la séquence " :".
 * - Vérifie si le destinataire est connecté via IsNickIsList().
 * - Si le pseudo est valide :
 *   - Construit un message IRC conforme :
 *     @code
 *     :<nick>!~<user>@localhost PRIVMSG <target> :<message>\r\n
 *     @endcode
 *   - Envoie le message directement au socket du destinataire.
 *   - Affiche la transaction en console (suivi serveur).
 * - Sinon :
 *   - Avertit l’expéditeur que le destinataire n’est pas connecté.
 *   - Affiche une erreur en console.
 *
 * @note
 * Si le message ne contient pas " :", un texte vide est envoyé.
 *
 * @see Server::IsNickIsList()
 * @see User::getNick()
 * @see User::getSocket()
 */
void Server::handleBrodcastPrivateMsg(User& user, std::string line)
{
    size_t pos0 = line.find(" ");
    size_t pos1 = line.find(" :");

    std::string userTarg = "";

    for (size_t i = pos0 + 1; i < pos1; i++)
        userTarg += line[i];

    
    size_t pos = line.find(" :");
    std::string msg;

    if (pos != std::string::npos)
        msg = line.substr(pos + 2);
    else
        msg = "";


    if (IsNickIsList(userTarg) == true)
    {
        int clientSock = 0;

        for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end();)
        {
            if (userTarg.compare(it->second.getNick()) == 0)
            {
                clientSock = it->second.getSocket();
                break;
            }
            ++it;
        }

        std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + userTarg + " :" + msg + "\r\n";
        if (Utils::IsSocketWritable(clientSock))
            send(clientSock, ircMsg.c_str(), ircMsg.size(), 0);

        std::cout << CYAN << "MP " << " / " << YELLOW << user.getName() << RESET << " to " << YELLOW << userTarg << RESET <<": " << msg << std::endl;
    }
    else
    {
        std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + user.getNick() + " :Le destinataire de votre message priver n'est pas connecter (" + msg + ")\r\n";
        if (Utils::IsSocketWritable(user.getSocket()))
            send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);

        std::cout << RED << "MP ERROR (TARGET NOT LOG)" << " / " << YELLOW << user.getName() << RESET << ": " << msg << std::endl;
    }
}

/**
 * @brief Informe un utilisateur qu'il ne peut pas envoyer de message
 *        car il a été kické ou banni.
 *
 * @param user Référence vers l'utilisateur tentant d'envoyer le message.
 * @param line Ligne brute contenant la commande PRIVMSG et le contenu
 *             original du message.
 *
 * @details
 * Cette fonction est appelée lorsqu'un utilisateur tente d'envoyer un
 * message alors qu'il a été exclu (kick) ou banni (ban) d'un canal.
 *
 * Étapes :
 * - Extrait le message à partir de la séquence " :" dans la ligne reçue.
 * - Si aucun message n'est trouvé, une chaîne vide est utilisée.
 * - Construit un message IRC d'erreur au format :
 *   @code
 *   :<nick>!~<user>@localhost PRIVMSG <nick> :
 *   Vous avez été kick ou ban. Le message n'a pas été envoyer (<msg>)
 *   @endcode
 * - Envoie cette notification au socket de l'utilisateur concerné.
 * - Affiche un message d'information côté serveur dans la console.
 *
 * @note
 * Cette fonction ne relaye pas le message d'origine à d'autres clients.
 *
 * @see Server::handleBrodcastMsgChann()
 * @see Server::handleBrodcastPrivateMsg()
 */
void Server::handleBrodcastMsgKB(User& user, std::string line)
{
    size_t pos = line.find(" :");
    std::string msg;

    if (pos != std::string::npos)
        msg = line.substr(pos + 2);
    else
        msg = "";

    std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + user.getNick() + " : Vous avez été kick ou ban. Le message n'a pas été envoyer (" + msg + ")\r\n";
    if (Utils::IsSocketWritable(user.getSocket()))
        send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);

    std::cout << CYAN << "USER KICKED/BAN MSG RECEP" << " / " << YELLOW << user.getName() << RESET << ": " << msg << std::endl;
}