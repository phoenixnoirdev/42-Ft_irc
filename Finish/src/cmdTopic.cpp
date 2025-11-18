/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdTopic.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:00 by phkevin           #+#    #+#             */
/*   Updated: 2025/11/18 14:15:41 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"

/**
 * @brief Gère la commande TOPIC pour changer le sujet d'un canal.
 * 
 * Cette fonction permet à un utilisateur de modifier le topic d'un canal
 * s'il possède les droits nécessaires (Grade 0, Grade 1 ou opérateur du canal).
 * Si l'utilisateur n'a pas les permissions, un message d'erreur est renvoyé.
 * 
 * @param user Référence vers l'utilisateur qui envoie la commande.
 * @param line Ligne complète de la commande reçue.
 */
void Server::handleTopic(User& user, const std::string& line)
{
    std::string tmp = line.substr(6);

    size_t sp1 = tmp.find("#");
    size_t sp2 = tmp.find(" :");
    std::string chanName = "";

    if(sp2 > 5)
    {
        handleTopicAff(user, line);
        return;
    }
    
    for (size_t i = sp1; i < sp2; i++)
        chanName += tmp[i];

    int idChan;
    for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
    {
        if (chanName.compare("#" + it->second.GetName()) == 0)
        {
            idChan = it->second.GetId();
            break;
        }
    }
    Channel &chan = _Chan[  idChan];

    if (chan.GetGradeUser(user) == 0 || chan.GetGradeUser(user) == 1 || chan.GetOpChannel() == user.getSocket())
    {
        EdtiTopic(chan, chanName, sp2, tmp, user);
        return;
    }
    else if(chan.isTopicOpOnly() ==  0)
    {
        EdtiTopic(chan, chanName, sp2, tmp, user);
        return;
    }
        
    std::string err = ":" + this->_ServName + " 482 " + user.getNick() + " " + chanName + " :You're not channel operator\r\n";
    if (Utils::IsSocketWritable(user.getSocket()))
        ::send(user.getSocket(), err.c_str(), err.size(), 0);

    std::cout << RED << "[TOPIC]: " << chanName << " " << user.getName() << " a tenter d'utiliser la commande TOPIC." << RESET << std::endl;
    
    return;
}

void Server::handleTopicAff(User& user, const std::string& line)
{
    std::string tmp = line.substr(6);
    std::string chanName = "";

    for (size_t i = 0; i < tmp.size(); i++)
        chanName += tmp[i];

    int idChan;
    for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
    {
        if (chanName.compare("#" + it->second.GetName()) == 0)
        {
            idChan = it->second.GetId();
            break;
        }
    }
    Channel &chan = _Chan[  idChan];

    std::cout << CYAN << "[TOPIC]: " << chanName << " " << user.getName() << " a demander le sujet." << RESET << std::endl;
    
    std::string top = ":" + this->_ServName + user.getNick() + " " + chanName + " : " + chan.GetTopic() + "\r\n";
    if (Utils::IsSocketWritable(user.getSocket()))
        ::send(user.getSocket(), top.c_str(), top.size(), 0);
}

void Server::EdtiTopic(Channel& chan, std::string chanName, size_t sp2, std::string tmp, User& user)
{
    std::string topic = "";
    for (size_t i = sp2 + 2; i < tmp.size(); i++)
        topic += tmp[i];

    std::cout << CYAN << "[TOPIC]: " << chanName << " " << user.getName() << " a change le TOPIC en : " << topic << RESET << std::endl;
    
    chan.SetTopic(topic);
}