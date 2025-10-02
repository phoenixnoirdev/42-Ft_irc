/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdNames.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:14 by phkevin           #+#    #+#             */
/*   Updated: 2025/09/29 12:08:30 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"

//Commande qui permet de connecter un user
void Server::handleLogin(int clientSocket, User& user)
{
    if (!PassCont(user.getPass()))
    {
        send(clientSocket, "Invalid password\n", 17, 0);
        close(clientSocket);
        this->_User.erase(clientSocket);
        return;
    }

    if (NickIsList(user.getNick()) == true)
    {
        std::cout << "[INFO] User " << user.getNick() << "@" << user.getName() << " deconnecter nick doublon !" << std::endl;
        send(clientSocket, "Nickname use\n", 13, 0);
        close(clientSocket);
        this->_User.erase(clientSocket);
        return;
    }

    user.setGrade(0);
    user.setAuth(true);

    std::cout << "[INFO] User " << user.getNick() << "@" << user.getName() << " authentifié !" << std::endl;


    std::map<int, Channel>::iterator chanIt = this->_Chan.find(0);
    if (chanIt != this->_Chan.end())
    {
        //Ajout le chan a l'user
        user.addIdChan(chanIt->second.GetId());

        // Gestion du ban a la tentative de join le chan par default
        if (chanIt->second.GetUserBan(user) == true)
        {
            send(clientSocket, "You are banned from this channel.\n", 30, 0);
            
            std::cout << RED << "[INFO] Banned user " << user.getNick() << " tried to connect to chan: " << chanIt->second.GetName() << std::endl;
            
            return;
        }
        handleWelcome(user);

        chanIt->second.AddUser(user);
        std::cout << "[INFO] User " << user.getNick() << " ajouté au channel " << chanIt->second.GetName() << std::endl;
        
        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";

        chanIt->second.BroadcastAll(joinMsg);
    }
}

void Server::handleWelcome(const User& user)
{
    std::string msg;

    // 001 - Welcome
    msg = ":" + _ServName + " 001 " + user.getNick() + " :Welcome to the IRC server " + user.getNick() + "!" + user.getName() + "@localhost\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

    // 002 - Host info
    msg = ":" + _ServName + " 002 " + user.getNick() + " :Your host is " + _ServName + ", running version 1.0\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

    // 003 - Server creation time (fake for now)
    msg = ":" + _ServName + " 003 " + user.getNick() + " :This server was created today\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

    // 004 - Server info (capabilities)
    msg = ":" + _ServName + " 004 " + user.getNick() + " " + _ServName + " 1.0 ao mtov\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);
}