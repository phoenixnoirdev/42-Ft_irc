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

void Server::handleNames(User& user, const std::string& line)
{
    std::string cmd, chanName;
    std::istringstream iss(line);
    iss >> cmd >> chanName;

    if (!chanName.empty() && chanName[0] == '#')
        chanName.erase(0, 1);

    if (chanName.empty())
    {
        // Aucun canal précisé -> afficher tous les canaux publics
        for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); ++it)
        {
            Channel &chan = it->second;

            // Si canal non privé ou utilisateur membre, afficher
            if (!chan.hasKey() || chan.isOpChannel(user))
                this->sendNamesReply(user, chan);
        }
    }
    else
    {
        // Canal spécifié
        Channel *chan = NULL;
        for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); ++it)
        {
            if (it->second.GetName() == chanName)
            {
                chan = &(it->second);
                break;
            }
        }

        if (!chan)
        {
            std::string msg = ":" + this->_ServName + " 366 " + user.getNick() + " " + chanName + " :End of /NAMES list.\r\n";
            ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

            return;
        }

        this->sendNamesReply(user, *chan);
    }
}

void Server::sendNamesReply(User &user, Channel &chan)
{
    std::string names;
    std::string chanName = chan.GetName();

    // Grade 0 (admin).
    const std::map<int, User>& grade0 = chan.GetLstGrade(0);
    for (std::map<int, User>::const_iterator it = grade0.begin(); it != grade0.end(); ++it)
    {
        if (!names.empty()) 
            names += " ";

        names += "@";
        names += it->second.getNick();
    }

    // Grade 1 (opérateur)
    const std::map<int, User>& grade1 = chan.GetLstGrade(1);
    for (std::map<int, User>::const_iterator it = grade1.begin(); it != grade1.end(); ++it)
    {
        if (!names.empty()) 
            names += " ";

        names += "@";
        names += it->second.getNick();
    }

    // Grade 2 (voice)
    const std::map<int, User>& grade2 = chan.GetLstGrade(2);
    for (std::map<int, User>::const_iterator it = grade2.begin(); it != grade2.end(); ++it)
    {
        if (!names.empty()) 
            names += " ";

        names += "@";
        names += it->second.getNick();
    }

    // Grade 3 (utilisateur normal)
    const std::map<int, User>& grade3 = chan.GetLstGrade(3);
    for (std::map<int, User>::const_iterator it = grade3.begin(); it != grade3.end(); ++it)
    {
        if (!names.empty()) 
            names += " ";

        names += "@";
        names += it->second.getNick();
    }

    // Envoi du code 353 (liste)
    std::string msg = ":" + this->_ServName + " 353 " + user.getNick() + " = " + chanName + " :" + names + "\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

    // Envoi du code 366 (fin de liste)
    std::string end = ":" + this->_ServName + " 366 " + user.getNick() + " " + chanName + " :End of /NAMES list.\r\n";
    ::send(user.getSocket(), end.c_str(), end.size(), 0);
}