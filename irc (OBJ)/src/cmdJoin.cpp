/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:00 by phkevin           #+#    #+#             */
/*   Updated: 2025/10/24 16:38:46 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"

void Server::handleJoin(int clientSocket, User& user, const std::string& line)
{
    size_t pos0 = line.find(" ");

    char c = line[pos0 + 1];



    bool multi = false;

    std::string tmp = line.substr(5); // aprés "JOIN "
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

        
        // Verificar se usuário está banido
        if (chanIt->second.GetUserBan(user))
        {
            std::string err = ":" + this->_ServName + " 474 " + user.getNick() + " " + chanName + " :Cannot join channel (+b)\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }
        
        // Verificar modo invite-only (+i)
        if (chanIt->second.isInviteOnly() && !chanIt->second.isInvited(user.getNick()))
        {
            std::string err = ":" + this->_ServName + " 473 " + user.getNick() + " " + chanName + " :Cannot join channel (+i)\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }

        // Verificar limite de usuários (+l)
        if (!chanIt->second.canJoin())
        {
            std::string err = ":" + this->_ServName + " 471 " + user.getNick() + " " + chanName + " :Cannot join channel (+l)\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }

        
        // Verificar senha do canal (+k)
        if (chanIt->second.hasKey())
        {
            std::cout << "[DEBUG KEY] Canal tem senha. Provided: '" << key << "', Expected: '" << chanIt->second.getKey() << "'" << std::endl;
        }
        
        if (!chanIt->second.checkKey(key))
        {
            std::cout << "[DEBUG KEY] BLOCKING JOIN: Wrong key for " << user.getNick() << std::endl;
            std::string err = ":" + this->_ServName + " 475 " + user.getNick() + " " + chanName + " :Cannot join channel (+k)\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }

        // Remover convite se estava na lista (usado uma vez)
        if (chanIt->second.isInvited(user.getNick()))
        {
            chanIt->second.removeInvite(user.getNick());
        }

        chanIt->second.AddUser(user);

        user.addIdChan(idChan);

        std::cout << "[INFO] User " << user.getNick() << " ajouté au channel " << chanIt->second.GetName() << std::endl;
        
        
        // Dar privilégios automáticos se configurado
        giveAutoOpPrivileges(clientSocket, "#" + chanName);


        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";
        chanIt->second.BroadcastAll(joinMsg);
    }
}