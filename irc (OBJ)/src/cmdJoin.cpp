/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:00 by phkevin           #+#    #+#             */
/*   Updated: 2025/10/31 14:10:52 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"

/**
 * @brief Gère la commande IRC JOIN pour permettre à un utilisateur de rejoindre un channel.
 *
 * Cette fonction analyse la ligne de commande pour extraire le nom du channel
 * et éventuellement la clé (mot de passe). Elle vérifie la validité du nom de
 * channel, crée un nouveau channel si nécessaire, ou ajoute l'utilisateur à
 * un channel existant en respectant les restrictions (ban, mode +i, +l, +k).
 *
 * Des messages d'erreur IRC standards sont envoyés si l'utilisateur ne peut
 * pas rejoindre le channel pour une quelconque raison.
 *
 * @param clientSocket Socket du client qui envoie la commande JOIN.
 * @param user Référence à l'objet User correspondant au client.
 * @param line Ligne contenant la commande JOIN complète.
 */
void Server::handleJoin(int clientSocket, User& user, const std::string& line)
{
    size_t pos0 = line.find(" ");

    char c = line[pos0 + 1];

    bool multi = false;

    std::string tmp = line.substr(5);
    std::string chanName = "";
    std::string key = "";
    
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (!(c != ' '))
        {
            multi = true;
            break;
        }
    }

    if (multi == true)
    {
        size_t sp1 = tmp.find(" ");
        std::string chanName = "";
  
        for (size_t i = 1; i < sp1; i++)
            chanName += tmp[i];

        key = tmp.substr(sp1 + 1);

        while (!key.empty() && key[0] == ' ')
            key = key.substr(1);
        while (!key.empty() && key[key.size() - 1] == ' ')
            key = key.substr(0, key.size() - 1);
        }
    else
    {
        for (size_t i = 1; i < tmp.size(); i++)
            chanName += tmp[i];
    }

    
    if ((c != '#' && c != '&' && c != '+' && c != '!') || chanName.empty())
    {
        std::string err = ":" + this->_ServName + " 479 " + user.getNick() + " " + chanName + " :Illegal channel name\r\n";
        if (Utils::IsSocketWritable(clientSocket))
          ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[JOIN]: " << user.getNick() << " a tenter d'utiliser la commande JOIN pour cree le chan : " << chanName << RESET << std::endl;
        
        return;
    }


    bool existe = false;

    int idChan;
    for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
    {
        if (chanName.compare(it->second.GetName()) == 0)
        {
            idChan = it->second.GetId();
            existe = true;
            break;
        }
    }

    if (existe == false)
    {
        int idNeChan = this->_Chan.size();

        this->_Chan.insert(std::make_pair(idNeChan, Channel(idNeChan, chanName)));
        user.addIdChan(idNeChan);
        std::map<int, Channel>::iterator chanIt = this->_Chan.find(idNeChan);
        chanIt->second.AddUserCreatChan(user);
        chanIt->second.SetOpChannel(user);
        std::cout << "[INFO] User " << user.getNick() << " ajouté au channel " << chanIt->second.GetName() << " qu'il a cree" << std::endl;
        
        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";
        chanIt->second.BroadcastAll(joinMsg);
    }
    else
    {
        std::map<int, Channel>::iterator chanIt = this->_Chan.find(idChan);

        if (chanIt->second.GetUserBan(user))
        {
            std::string err = ":" + this->_ServName + " 474 " + user.getNick() + " " + chanName + " :Cannot join channel (+b)\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }
        
        if (chanIt->second.isInviteOnly() && !chanIt->second.isInvited(user.getNick()))
        {
            std::string err = ":" + this->_ServName + " 473 " + user.getNick() + " " + chanName + " :Cannot join channel (+i)\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }

        if (!chanIt->second.canJoin())
        {
            std::string err = ":" + this->_ServName + " 471 " + user.getNick() + " " + chanName + " :Cannot join channel (+l)\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }

        if (chanIt->second.hasKey())
        {
            std::cout << "[DEBUG KEY] Canal tem senha. Provided: '" << key << "', Expected: '" << chanIt->second.getKey() << "'" << std::endl;
        }
        
        if (!chanIt->second.checkKey(key))
        {
            std::cout << "[DEBUG KEY] BLOCKING JOIN: Wrong key for " << user.getNick() << std::endl;
            std::string err = ":" + this->_ServName + " 475 " + user.getNick() + " " + chanName + " :Cannot join channel (+k)\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }

        if (chanIt->second.isInvited(user.getNick()))
        {
            chanIt->second.removeInvite(user.getNick());
        }

        chanIt->second.AddUser(user);

        user.addIdChan(idChan);

        std::cout << "[INFO] User " << user.getNick() << " ajouté au channel " << chanIt->second.GetName() << std::endl;
        
        giveAutoOpPrivileges(clientSocket, "#" + chanName);

        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";
        chanIt->second.BroadcastAll(joinMsg);
    }
}