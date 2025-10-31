/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 15:30:00 by luis-fif         #+#    #+#             */
/*   Updated: 2025/10/02 15:30:00 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/inc.hpp"

/**
 * @brief Sépare une chaîne en tokens à partir des espaces.
 *
 * Cette fonction parcourt la chaîne fournie et découpe chaque mot séparé par 
 * un espace en un élément d'un vecteur de chaînes.
 *
 * @param str La chaîne à découper.
 * @return std::vector<std::string> Un vecteur contenant tous les mots extraits.
 */
std::vector<std::string> splitString(const std::string& str)
{
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (iss >> token)
        tokens.push_back(token);
        
    return tokens;
}

/**
 * @brief Recherche un canal par son nom dans une map de canaux.
 *
 * Cette fonction retire le préfixe '#' si présent dans le nom du canal
 * et parcourt la map fournie pour trouver un canal correspondant.
 *
 * @param channelName Le nom du canal à rechercher (peut commencer par '#').
 * @param channels La map des canaux, indexée par leur identifiant.
 * @return Channel* Pointeur vers le canal trouvé, ou NULL si aucun canal ne correspond.
 */
Channel* findChannelByName(const std::string& channelName, std::map<int, Channel>& channels)
{
    std::string searchName = channelName;
    if (!searchName.empty() && searchName[0] == '#')
        searchName = searchName.substr(1);
    
    for (std::map<int, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->second.GetName() == searchName)
            return &(it->second);
    }
    
    return NULL;
}

/**
 * @brief Recherche un utilisateur par son pseudo dans un canal donné.
 *
 * Cette fonction parcourt la map des utilisateurs et vérifie si
 * chaque utilisateur appartient au canal (grade >= 0) et si son
 * pseudo correspond à celui recherché.
 *
 * @param nick Le pseudo de l'utilisateur à rechercher.
 * @param channel Référence au canal dans lequel rechercher l'utilisateur.
 * @param users La map de tous les utilisateurs, indexée par socket.
 * @return User* Pointeur vers l'utilisateur trouvé, ou NULL si aucun utilisateur ne correspond.
 */
User* findUserInChannelByNick(const std::string& nick, Channel& channel, std::map<int, User>& users)
{
    for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (it->second.getNick() == nick && channel.GetGradeUser(it->second) >= 0)
            return &(it->second);
    }
    return NULL;
}

/**
 * @brief Envoie un message d'erreur IRC à un client spécifique.
 *
 * Cette fonction construit un message d'erreur au format IRC et l'envoie
 * directement au socket du client.
 *
 * @param clientSocket Le socket du client à qui envoyer l'erreur.
 * @param errorCode Le code d'erreur IRC à transmettre.
 * @param message Le message d'erreur détaillant la cause.
 */
void sendIRCError(int clientSocket, int errorCode, const std::string& message)
{
    std::ostringstream oss;
    oss << ":localhost " << errorCode << " * " << message << "\r\n";

    std::string response = oss.str();
    send(clientSocket, response.c_str(), response.size(), 0);
}

/**
 * @brief Envoie un message MODE à tous les utilisateurs d'un canal.
 *
 * Cette fonction construit un message de type MODE selon le format IRC et le
 * diffuse à tous les utilisateurs présents dans le canal.
 *
 * @param channel Référence au canal sur lequel appliquer le MODE.
 * @param user Référence à l'utilisateur qui déclenche le MODE.
 * @param modeString Chaîne représentant le mode à appliquer (ex: "+o", "-v").
 * @param param Paramètre optionnel associé au mode (ex: pseudo d'un utilisateur).
 */
void sendModeResponse(Channel& channel, const User& user, const std::string& modeString, const std::string& param = "")
{
    std::string response = ":" + user.getNick() + "!~" + user.getName() + "@localhost MODE " + channel.GetName() + " " + modeString;
    
    if (!param.empty())
        response += " " + param;
        
    response += "\r\n";
    
    channel.BroadcastAll(response);
}

/**
 * @brief Gère la commande MODE d'un client sur un canal IRC.
 *
 * Cette fonction analyse la commande MODE reçue d'un client et effectue les
 * actions correspondantes sur le canal visé, comme définir des modes (+i, +t, +k, +o, +l, +m),
 * ajouter ou retirer des opérateurs, limiter le nombre d'utilisateurs ou définir une clé.
 * Elle envoie également les réponses appropriées au client et diffuse les changements aux autres utilisateurs du canal.
 *
 * @param clientSocket Le socket du client qui envoie la commande.
 * @param line La ligne complète de la commande MODE.
 * @param users Référence à la map des utilisateurs connectés.
 * @param channels Référence à la map des canaux existants.
 */
void handleModeCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 2)
    {
        sendIRCError(clientSocket, 461, "MODE :Not enough parameters");
        return;
    }
    
    std::string channelName = tokens[1];
    
    Channel* channel = findChannelByName(channelName, channels);
    if (!channel)
    {
        sendIRCError(clientSocket, 403, channelName + " :No such channel");
        return;
    }
    
    if (users.find(clientSocket) == users.end())
    {
        sendIRCError(clientSocket, 401, ":No such nick/channel");
        return;
    }
    
    User& user = users[clientSocket];
    
    if (tokens.size() == 2)
    {
        std::string modes = channel->getModeString();
        std::string response = ":localhost 324 " + user.getNick() + " " + channelName + " " + modes + "\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
        return;
    }
    
    std::string modeString = tokens[2];
    std::string param = (tokens.size() > 3) ? tokens[3] : "";
    

    if (channel->GetGradeUser(user) == 3 || channel->GetGradeUser(user) == 2)
    {
        sendIRCError(clientSocket, 482, channelName + " :You're not channel operator --- dd");
        return;
    }
    
    if (modeString.length() < 2)
    {
        sendIRCError(clientSocket, 472, modeString + " :is unknown mode char to me");
        return;
    }
    
    bool adding = (modeString[0] == '+');
    
    for (size_t i = 1; i < modeString.length(); ++i)
    {
        char mode = modeString[i];
        std::string singleMode = std::string(1, modeString[0]) + mode;
        
        switch(mode)
        {
            case 'i':
                channel->setInviteOnly(adding);
                sendModeResponse(*channel, user, singleMode);
                break;
                
            case 't':
                channel->setTopicOpOnly(adding);
                sendModeResponse(*channel, user, singleMode);
                break;
                
            case 'k':
                if (adding)
                {
                    if (param.empty())
                    {
                        sendIRCError(clientSocket, 461, "MODE +k :Not enough parameters");
                        continue;
                    }
                    channel->setKey(param);
                    sendModeResponse(*channel, user, singleMode, param);
                }
                else
                {
                    channel->removeKey();
                    sendModeResponse(*channel, user, singleMode);
                }
                break;
                
            case 'o':
                if (param.empty())
                {
                    sendIRCError(clientSocket, 461, "MODE +o :Not enough parameters");
                    continue;
                }
                
                if (!findUserInChannelByNick(param, *channel, users))
                {
                    sendIRCError(clientSocket, 401, param + " :No such nick/channel");
                    continue;
                }
                
                if (adding)
                    channel->giveOp(param);
                else
                    channel->removeOp(param);
                    
                sendModeResponse(*channel, user, singleMode, param);
                break;
                
            case 'l':
                if (adding)
                {
                    if (param.empty())
                    {
                        sendIRCError(clientSocket, 461, "MODE +l :Not enough parameters");
                        continue;
                    }
                    
                    int limit = std::atoi(param.c_str());
                    if (limit <= 0)
                    {
                        sendIRCError(clientSocket, 696, param + " :Invalid limit");
                        continue;
                    }
                    
                    channel->setUserLimit(limit);
                    sendModeResponse(*channel, user, singleMode, param);
                }
                else
                {
                    channel->removeUserLimit();
                    sendModeResponse(*channel, user, singleMode);
                }
                break;
                
            case 'm':
                channel->setModerated(adding);
                sendModeResponse(*channel, user, singleMode);
                break;
                
            default:
                sendIRCError(clientSocket, 472, std::string(1, mode) + " :is unknown mode char to me");
                break;
        }
    }
}

/**
 * @brief Gère la commande INVITE d'un utilisateur pour inviter un autre utilisateur
 *        à rejoindre un canal IRC.
 *
 * Cette fonction vérifie que tous les paramètres nécessaires sont fournis,
 * que le canal et le destinataire existent, et que l'utilisateur n'est pas déjà
 * présent sur le canal. Si toutes les conditions sont respectées, elle ajoute
 * l'utilisateur à la liste des invités et envoie les messages IRC appropriés
 * à l'invité et à l'expéditeur.
 *
 * @param clientSocket Le socket de l'utilisateur qui envoie l'invitation.
 * @param line La ligne complète de la commande INVITE.
 * @param users Référence à la map des utilisateurs connectés.
 * @param channels Référence à la map des canaux existants.
 */
void handleInviteCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 3)
    {
        sendIRCError(clientSocket, 461, "INVITE :Not enough parameters");
        return;
    }
    
    std::string targetNick = tokens[1];
    std::string channelName = tokens[2];
    
    Channel* channel = findChannelByName(channelName, channels);
    if (!channel)
    {
        sendIRCError(clientSocket, 403, channelName + " :No such channel");
        return;
    }
    
    if (users.find(clientSocket) == users.end())
        return;
        
    User& inviter = users[clientSocket];
    
    User* target = NULL;
    for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (it->second.getNick() == targetNick)
        {
            target = &(it->second);
            break;
        }
    }
    
    if (!target)
    {
        sendIRCError(clientSocket, 401, targetNick + " :No such nick/channel");
        return;
    }
    
    if (channel->GetGradeUser(*target) >= 0)
    {
        sendIRCError(clientSocket, 443, targetNick + " " + channelName + " :is already on channel");
        return;
    }
    
    channel->addInvite(targetNick);
    
    std::string inviteMsg = ":" + inviter.getNick() + "!~" + inviter.getName() + "@localhost INVITE " + targetNick + " " + channelName + "\r\n";
    send(target->getSocket(), inviteMsg.c_str(), inviteMsg.size(), 0);
    
    std::string confirmMsg = ":localhost 341 " + inviter.getNick() + " " + targetNick + " " + channelName + "\r\n";
    send(clientSocket, confirmMsg.c_str(), confirmMsg.size(), 0);
}

/**
 * @brief Gère la commande TOPIC pour afficher ou modifier le sujet d'un canal IRC.
 *
 * Si la commande ne fournit pas de nouveau sujet, elle renvoie le sujet actuel
 * du canal au client. Si un nouveau sujet est fourni, la fonction vérifie si
 * l'utilisateur a les droits pour modifier le sujet (opérateur du canal ou
 * permissions adéquates). Si autorisé, le sujet est mis à jour et diffusé
 * à tous les utilisateurs du canal.
 *
 * @param clientSocket Le socket de l'utilisateur qui envoie la commande.
 * @param line La ligne complète de la commande TOPIC.
 * @param users Référence à la map des utilisateurs connectés.
 * @param channels Référence à la map des canaux existants.
 */
void handleTopicCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 2)
    {
        sendIRCError(clientSocket, 461, "TOPIC :Not enough parameters");
        return;
    }
    
    std::string channelName = tokens[1];
    
    Channel* channel = findChannelByName(channelName, channels);
    if (!channel)
    {
        sendIRCError(clientSocket, 403, channelName + " :No such channel");
        return;
    }
    
    if (users.find(clientSocket) == users.end())
        return;
        
    User& user = users[clientSocket];
    
    if (tokens.size() == 2)
    {
        std::string topic = channel->GetTopic();
        if (topic.empty())
        {
            std::string response = ":localhost 331 " + user.getNick() + " " + channelName + " :No topic is set\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        }
        else
        {
            std::string response = ":localhost 332 " + user.getNick() + " " + channelName + " :" + topic + "\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        }
        return;
    }
    
    if (!channel->canChangeTopic(user))
    {
        sendIRCError(clientSocket, 482, channelName + " :You're not channel operator");
        return;
    }
    
    size_t pos = line.find(" :");
    std::string newTopic;
    
    if (pos != std::string::npos)
        newTopic = line.substr(pos + 2);
    else
    {
        size_t firstSpace = line.find(' ');
        if (firstSpace != std::string::npos)
        {
            size_t secondSpace = line.find(' ', firstSpace + 1);
            if (secondSpace != std::string::npos)
                newTopic = line.substr(secondSpace + 1);
        }
    }
    
    channel->SetTopic(newTopic);
    
    std::string response = ":" + user.getNick() + "!~" + user.getName() + "@localhost TOPIC " + channelName + " :" + newTopic + "\r\n";
    channel->BroadcastAll(response);
}