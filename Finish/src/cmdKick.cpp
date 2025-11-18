/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdKick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:09 by phkevin           #+#    #+#             */
/*   Updated: 2025/11/18 17:18:11 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"
# include <sstream>

/**
 * @brief Gère la commande IRC KICK pour expulser un utilisateur d'un channel.
 *
 * Cette fonction vérifie que le client est présent et est autorisé à kicker
 * dans le channel. Elle extrait le nom du channel, le pseudo de l'utilisateur
 * cible et la raison du kick depuis la ligne de commande.
 *
 * Si l'utilisateur est autorisé, le message de kick est diffusé à tous les
 * membres du channel et l'utilisateur cible est retiré du channel. Sinon,
 * un message d'erreur est envoyé au kicker.
 *
 * @param clientSocket Socket du client qui envoie la commande KICK.
 * @param line Ligne contenant la commande KICK complète.
 */
void Server::handleKickCommand(int clientSocket, const std::string& line)
{
    std::map<int, User>::iterator itK = _User.find(clientSocket);

    if (itK == _User.end())
        return;

    User &kicker = itK->second;

    std::string chanName, targetNick, reason;

    std::string tmp = line.substr(5); // aprés "KICK "
    std::string::size_type sp1 = tmp.find(' ');
    if (sp1 == std::string::npos)
    {
        std::string err = ":" + this->_ServName + " 461 " + kicker.getNick() + " KICK :Not enough parameters\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, err.c_str(), err.size(), 0);
        return;
    }
    chanName = tmp.substr(0, sp1);

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
        if (Utils::IsSocketWritable(clientSocket))    
            ::send(clientSocket, err.c_str(), err.size(), 0);
    }


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

    if (chan.GetGradeUser(kicker) == 0 || chan.GetGradeUser(kicker) == 1 || chan.GetOpChannel() == kicker.getSocket())
    {
        if (kicker.getName() == targetNick)
        {
            std::cout << RED << "[KICK]: " << chanName << " " << kicker.getName() << " a tenter d'utiliser la commande KICK sur lui meme " << targetNick << " = " << reason << std::endl;

            std::string note = ":" + this->_ServName + "Tu ne peu pas te kick.\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(clientSocket, note.c_str(), note.size(), 0);
            return;
        }

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
        const User &target = targetUs->second;


        if (targetFd == -1)
        {
            std::string note = ":" + this->_ServName + " NOTICE " + kicker.getNick() + " :User " + targetNick + " not online.\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(clientSocket, note.c_str(), note.size(), 0);
            return;
        }

        
        if (!(chan.GetGradeUser(kicker) <= chan.GetGradeUser(target)))
        {
            if (_AdminConfig.isOperator(target.getName()))
            {
                std::cout << RED << "[KICK]: " << chanName << " " << kicker.getName() << " a tenter d'utiliser la commande KICK sur l'user de operateur serveur " << targetNick << " = " << reason << std::endl;

                std::string note = ":" + this->_ServName + "Tu ne peu pas kick un operateur serveur.\r\n";
                if (Utils::IsSocketWritable(clientSocket))
                    ::send(clientSocket, note.c_str(), note.size(), 0);
                return;
            }
            else
            {   
                std::cout << RED << "[KICK]: " << chanName << " " << kicker.getName() << " a tenter d'utiliser la commande KICK sur l'user de grade egale ou superieur " << targetNick << " = " << reason << std::endl;
      
                std::string note = ":" + this->_ServName + "Tu ne peu pas kick un user de grade superieur ou egale.\r\n";
                if (Utils::IsSocketWritable(clientSocket))
                    ::send(clientSocket, note.c_str(), note.size(), 0);
                return;
            }
        } 



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




        std::cout << "[" << RED << "KICK" << RESET << "]: " << chanName << " " << targetNick << " " << reason << " / OP = " << kicker.getName() << std::endl;


        {
            std::string bye = ":" + this->_ServName + " NOTICE " + target.getNick() + " :You were KICK (" + (reason.empty() ? "no reason" : reason) + ")\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(targetFd, bye.c_str(), bye.size(), 0);

            std::string byeMp = "SERVER:" + target.getNick() + "!~" + target.getName() + "@localhost PRIVMSG " + target.getNick() + " :You were KICK (" + (reason.empty() ? "no reason" : reason) + ")\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(targetFd, byeMp.c_str(), byeMp.size(), 0);
        }


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
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[KICK]: " << chanName << " " << kicker.getName() << " a tenter d'utiliser la commande KICK sur l'user " << targetNick << " = " << reason << std::endl;
        
        return;
    }
}