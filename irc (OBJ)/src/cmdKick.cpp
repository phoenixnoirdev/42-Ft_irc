/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdKick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:09 by phkevin           #+#    #+#             */
/*   Updated: 2025/09/29 12:46:08 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"
# include <sstream>

void Server::handleKickCommand(int clientSocket, const std::string& line)
{

    // 0) Permet retourner l'user qui lance la commande
    std::map<int, User>::iterator itK = _User.find(clientSocket);

    if (itK == _User.end())
        return;

    User &kicker = itK->second;


    // 1.0) parse: KICK = #canal / nick / reason
    std::string chanName, targetNick, reason;

    // 1.1) parse: KICK = #canal / nick / reason (etrait le canal)
    std::string tmp = line.substr(5); // aprés "KICK "
    std::string::size_type sp1 = tmp.find(' ');
    if (sp1 == std::string::npos)
    {
        std::string err = ":" + this->_ServName + " 461 " + kicker.getNick() + " KICK :Not enough parameters\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
        return;
    }
    chanName = tmp.substr(0, sp1);

    // 1.2) parse: KICK = #canal / nick / reason (extrait la target et la reason si il y a)
    std::string rest = tmp.substr(sp1 + 1);
    std::string::size_type sp2 = rest.find(' ');
    if (sp2 == std::string::npos)
    {
        targetNick = rest;
    } 
    else
    {
        targetNick = rest.substr(0, sp2);
        std::string::size_type colon = line.find(" :");
        if (colon != std::string::npos)
            reason = line.substr(colon + 2);
    }

    if (targetNick.empty())
    {
        std::string err = ":" + this->_ServName + " 461 " + kicker.getNick() + " KICK :Not enough parameters\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
    }


    //Controle le grade
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

    if (chan.GetGradeUser(kicker) == 0 || chan.GetGradeUser(kicker) == 1)
    {
        // 2) Cherche l'utilisateur dans la liste
        int targetFd = -1;
        std::map<int, User>::iterator targetUs = _User.end();
        for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it)
        {
            if (it->second.getNick() == targetNick) 
            {
                targetFd = it->first;
                targetUs = it;
                break; 
            }
        }


        // 3) Retour d’information dans le channel
        {
            std::string reply = ":" + kicker.getNick() + "!~" + kicker.getName() + "@localhost KICK " + chanName + " " + targetNick + " :" + (reason.empty() ? "" : reason) + "\r\n";

            for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
            {
                if (chanName.compare("#" + it->second.GetName()) == 0)
                {
                    it->second.BroadcastAll(reply);
                    break;
                }
            }
        }

        // 6) Si l'utilisateur cible n'es pas en ligne informe l'utilisateur et sort de la commande
        if (targetFd == -1)
        {
            std::string note = ":" + this->_ServName + " NOTICE " + kicker.getNick() + " :User " + targetNick + " not online.\r\n";
            ::send(clientSocket, note.c_str(), note.size(), 0);
            return;
        }

        // 5) Log serveur
        std::cout << "[" << RED << "KICK" << RESET << "]: " << chanName << " " << targetNick << " " << reason << " / OP = " << kicker.getName() << std::endl;


        // 6) Informe la cible de sont kick
        {
            const User &target = targetUs->second;

            std::string bye = ":" + this->_ServName + " NOTICE " + target.getNick() + " :You were KICK (" + (reason.empty() ? "no reason" : reason) + ")\r\n";
            ::send(targetFd, bye.c_str(), bye.size(), 0);

            std::string byeMp = "SERVER:" + target.getNick() + "!~" + target.getName() + "@localhost PRIVMSG " + target.getNick() + " :You were KICK (" + (reason.empty() ? "no reason" : reason) + ")\r\n";
            ::send(targetFd, byeMp.c_str(), byeMp.size(), 0);
        }

        // 8) Deconnecte la cible du channel et set a -1 pour signifier qu'il est hors channel
        for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
        {
            if (chanName.compare("#" + it->second.GetName()) == 0)
            {
                it->second.RemoveUser(targetFd);

                User &target = targetUs->second;
                target.removeIdChan(it->second.GetId());

                return;
            }
        }
    }
    else
    {
        std::string err = ":server 482 " + kicker.getNick() + " " + chanName + " :You're not channel operator\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[KICK]: " << chanName << " " << kicker.getName() << " a tenter d'utiliser la commande KICK sur l'user " << targetNick << " = " << reason << std::endl;
        
        return;
    }
}