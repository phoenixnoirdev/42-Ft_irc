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

/**
 * @brief Gère la commande NAMES pour lister les utilisateurs d'un ou plusieurs canaux.
 *
 * Cette fonction analyse la commande NAMES reçue d'un utilisateur et renvoie 
 * la liste des utilisateurs présents dans le canal spécifié. Si aucun canal 
 * n'est précisé, elle liste les utilisateurs de tous les canaux visibles pour 
 * l'utilisateur (non protégés par mot de passe ou si l'utilisateur est opérateur).
 *
 * @param user Référence vers l'objet User envoyant la commande.
 * @param line Ligne complète de la commande NAMES reçue.
 */
void Server::handleNames(User& user, const std::string& line)
{
    std::string cmd, chanName;
    std::istringstream iss(line);
    iss >> cmd >> chanName;

    if (!chanName.empty() && chanName[0] == '#')
        chanName.erase(0, 1);

    if (chanName.empty())
    {
        for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); ++it)
        {
            Channel &chan = it->second;

            if (!chan.hasKey() || chan.isOpChannel(user))
                this->sendNamesReply(user, chan);
        }
    }
    else
    {
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
            if (Utils::IsSocketWritable(user.getSocket()))
                ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

            return;
        }

        this->sendNamesReply(user, *chan);
    }
}

/**
 * @brief Envoie la liste des utilisateurs présents dans un canal à un utilisateur.
 *
 * Cette fonction construit et envoie un message IRC contenant tous les utilisateurs 
 * d'un canal donné, classés par grade (0 à 3). Les utilisateurs avec des grades 
 * supérieurs sont préfixés par '@'. Après la liste, un message de fin de liste est envoyé.
 *
 * @param user Référence vers l'utilisateur destinataire de la liste.
 * @param chan Référence vers le canal dont on souhaite envoyer la liste des utilisateurs.
 */
void Server::sendNamesReply(User &user, Channel &chan)
{
    std::string names;
    std::string chanName = chan.GetName();

    const std::map<int, User>& grade0 = chan.GetLstGrade(0);
    for (std::map<int, User>::const_iterator it = grade0.begin(); it != grade0.end(); ++it)
    {
        if (!names.empty()) 
            names += " ";

        names += "@";
        names += it->second.getNick();
    }

    const std::map<int, User>& grade1 = chan.GetLstGrade(1);
    for (std::map<int, User>::const_iterator it = grade1.begin(); it != grade1.end(); ++it)
    {
        if (!names.empty()) 
            names += " ";

        names += "@";
        names += it->second.getNick();
    }

    const std::map<int, User>& grade2 = chan.GetLstGrade(2);
    for (std::map<int, User>::const_iterator it = grade2.begin(); it != grade2.end(); ++it)
    {
        if (!names.empty()) 
            names += " ";

        names += "@";
        names += it->second.getNick();
    }

    const std::map<int, User>& grade3 = chan.GetLstGrade(3);
    for (std::map<int, User>::const_iterator it = grade3.begin(); it != grade3.end(); ++it)
    {
        if (!names.empty()) 
            names += " ";

        names += "@";
        names += it->second.getNick();
    }

    std::string msg = ":" + this->_ServName + " 353 " + user.getNick() + " = " + chanName + " :" + names + "\r\n";
    if (Utils::IsSocketWritable(user.getSocket()))
        ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

    std::string end = ":" + this->_ServName + " 366 " + user.getNick() + " " + chanName + " :End of /NAMES list.\r\n";
    if (Utils::IsSocketWritable(user.getSocket()))
        ::send(user.getSocket(), end.c_str(), end.size(), 0);
}