/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdList_new.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:13 by phkevin           #+#    #+#             */
/*   Updated: 2025/10/02 17:45:19 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"
# include "../inc/utils.hpp"

//retourne la liste des canaux - VERSION ULTRA-SIMPLES HEXCHAT
void Server::handleList(User& user)
{
    int clientSocket = user.getSocket();
    std::string msg = "";

    // RPL_LISTSTART (321) - Formato minimalista
    msg = ":" + this->_ServName + " 321 " + user.getNick() + " Channel :Users  Name\r\n";
    ::send(clientSocket, msg.c_str(), msg.size(), 0);

    // RPL_LIST (322) - Para cada canal
    for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
    {
        std::string channelName = "#" + it->second.GetName();
        int userCount = it->second.GetPop();
        std::string topic = it->second.getTopic();
        
        // Topic ultra-simples - só letras e números
        if (topic.empty()) {
            topic = "No topic";
        }
        
        // Limpar TUDO que não seja ASCII básico
        std::string safeTopic = "";
        for (size_t i = 0; i < topic.size(); i++) {
            char c = topic[i];
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || 
                (c >= '0' && c <= '9') || c == ' ' || c == '-' || c == '_') {
                safeTopic += c;
            }
        }
        if (safeTopic.empty()) {
            safeTopic = "Welcome";
        }
        
        // Formato RFC2812 exato
        msg = ":" + this->_ServName + " 322 " + user.getNick() + " " + channelName + " " + Utils::IntToString(userCount) + " :" + safeTopic + "\r\n";
        ::send(clientSocket, msg.c_str(), msg.size(), 0);
        
        std::cout << "[LIST] " << channelName << " (" << userCount << ") - " << safeTopic << std::endl;
    }

    // RPL_LISTEND (323)
    msg = ":" + this->_ServName + " 323 " + user.getNick() + " :End of /LIST\r\n";
    ::send(clientSocket, msg.c_str(), msg.size(), 0);
    
    std::cout << "[LIST] Sent " << this->_Chan.size() << " channels to " << user.getNick() << std::endl;
}