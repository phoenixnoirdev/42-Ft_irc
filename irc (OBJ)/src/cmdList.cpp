/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdList.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:13 by phkevin           #+#    #+#             */
/*   Updated: 2025/09/29 12:45:06 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"
# include "../inc/utils.hpp"

//retourn la liste des channel
void Server::handleList(User& user)
{
    int clientSocket = user.getSocket();
    std::string msg = "";

    msg = ":" + this->_ServName + " 321 " + user.getNick() + " Channel :Users Name\r\n";
    ::send(clientSocket, msg.c_str(), msg.size(), 0);

    for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
    {
        msg = ":" + this->_ServName + " 322 " + user.getNick() + " #" + it->second.GetName() + " " + Utils::IntToString(it->second.GetNbUser()) + " :" + it->second.GetTopic() + "\r\n";
        ::send(clientSocket, msg.c_str(), msg.size(), 0);
    }

    msg = ":" + this->_ServName + " 323 " + user.getNick() +  " :End of /LIST\r\n";
    ::send(clientSocket, msg.c_str(), msg.size(), 0);
}