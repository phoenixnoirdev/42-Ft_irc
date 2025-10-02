/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:00 by phkevin           #+#    #+#             */
/*   Updated: 2025/09/29 12:47:16 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"

void Server::handleTopic(User& user, const std::string& line)
{
    std::string tmp = line.substr(6); // aprés "TOPIC "

    size_t sp1 = tmp.find("#");
    size_t sp2 = tmp.find(" :");
    std::string chanName = "";

    for (size_t i = sp1; i < sp2; i++)
        chanName += tmp[i];

    
    //Recupere le channel
    int idChan;
    for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
    {
        if (chanName.compare("#" + it->second.GetName()) == 0)
        {
            idChan = it->second.GetId();
            break;
        }
    }
    Channel &chan = _Chan[idChan];

    //Controle les autorisation
    if (chan.GetGradeUser(user) == 0 || chan.GetGradeUser(user) == 1 || chan.GetOpChannel() == user.getSocket())
    {
        std::string topic = "";
        for (size_t i = sp2 + 2; i < tmp.size(); i++)
            topic += tmp[i];

        std::cout << CYAN << "[TOPIC]: " << chanName << " " << user.getName() << " a change le TOPIC en : " << topic << RESET << std::endl;
        
        chan.SetTopic(topic);
        
        return;
    }
        
    std::string err = ":" + this->_ServName + " 482 " + user.getNick() + " " + chanName + " :You're not channel operator\r\n";
    ::send(user.getSocket(), err.c_str(), err.size(), 0);

    std::cout << RED << "[TOPIC]: " << chanName << " " << user.getName() << " a tenter d'utiliser la commande TOPIC." << RESET << std::endl;
    
    return;
}