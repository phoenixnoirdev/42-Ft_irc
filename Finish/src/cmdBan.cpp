/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdBan.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:01 by phkevin           #+#    #+#             */
/*   Updated: 2025/11/14 16:00:18 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"
# include "../inc/utils.hpp"
# include <sstream>

/**
 * @brief Gère la commande BAN pour exclure un utilisateur d'un canal.
 *
 * Cette fonction permet à un opérateur de canal de bannir un utilisateur ciblé
 * d'un canal spécifique. Elle vérifie :
 * - Que l'expéditeur est bien un opérateur du canal.
 * - Que l'utilisateur ciblé existe et n'est pas l'expéditeur lui-même.
 * 
 * Si le ban est valide, l'utilisateur ciblé est retiré du canal, ajouté à la
 * liste des utilisateurs bannis du canal, et reçoit une notification. Tous les
 * membres du canal reçoivent également une notification du ban.
 *
 * @param clientSocket Le socket de l'utilisateur qui envoie la commande.
 * @param chanName Le nom du canal où le ban doit être appliqué.
 * @param maskTarg Le pseudonyme de l'utilisateur ciblé pour le ban.
 */
void Server::handleBanCommand(int clientSocket, const std::string& chanName, const std::string maskTarg)
{
    std::map<int, User>::iterator itK = _User.find(clientSocket);

    if (itK == _User.end())
        return;

    User &op = itK->second;

    int socketUser;
    for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end(); it++)
    {
        if (maskTarg.compare(it->second.getNick()) == 0)
        {
            socketUser = it->second.getSocket();
            break;
        }
    }
    std::map<int, User>::iterator tar = _User.find(socketUser);
    
    if (tar == _User.end())
        return;

    User &target = tar->second;


    if (op.getNick().compare(target.getNick()) == 0 && Utils::IsSocketWritable(clientSocket))
    {
        std::string note = "You cannot ban yourself.\r\n";
        ::send(clientSocket, note.c_str(), note.size(), 0);
        return;
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


    if (chan.GetGradeUser(op) == 0 || chan.GetGradeUser(op) == 1 || chan.GetOpChannel() == op.getSocket())
    { 
        if (op.getName() == target.getName())
        {
            std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " tried to use the BAN command on himself " << target.getNick() << std::endl;

            std::string note = ":" + this->_ServName + "You cannot ban yourself.\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(clientSocket, note.c_str(), note.size(), 0);
            return;
        }

        int targetFd = -1;
        std::map<int, User>::iterator targetUs = _User.end();
        for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it)
        {
            if (it->second.getNick() == target.getName()) 
            {
                targetFd = it->first;
                targetUs = it;
                break; 
            }
        }
        const User &target = targetUs->second;


        if (!(chan.GetGradeUser(op) <= chan.GetGradeUser(target)))
        {
            if (_AdminConfig.isOperator(target.getName()))
            {
                std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " tried to use the BAN command on the server operator user " << target.getNick() << std::endl;

                std::string note = ":" + this->_ServName + "You cannot ban a server operator.\r\n";
                if (Utils::IsSocketWritable(clientSocket))
                    ::send(clientSocket, note.c_str(), note.size(), 0);
                return;
            }
            else
            {   
                std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " tried to use the BAN command on the user with equal or higher grade " << target.getNick() << std::endl;
      
                std::string note = ":" + this->_ServName + "You cannot ban a user with equal or higher grade.\r\n";
                if (Utils::IsSocketWritable(clientSocket))
                    ::send(clientSocket, note.c_str(), note.size(), 0);
                return;
            }
        } 


        {
            std::string reply = ":" + op.getNick() + "!~" + op.getName() + "@localhost BAN " + chanName + " " + target.getName() + "\r\n";
            chan.BroadcastAll(reply);
        }


        if (targetFd == -1 && Utils::IsSocketWritable(clientSocket))
        {
            std::string note = ":" + this->_ServName + " NOTICE " + op.getNick() + " :User " + target.getName() + " not online.\r\n";
            ::send(clientSocket, note.c_str(), note.size(), 0);
            return;
        }
        

        

        std::cout << "[" << RED << "BAN" << RESET << "]: " << chanName << " " << target.getName() <<  " / OP = " << op.getName() << std::endl;
        

        

        {
            std::string bye = ":" + this->_ServName + " NOTICE " + target.getNick() + " :You were BAN \r\n";
            if (Utils::IsSocketWritable(targetFd))
                ::send(targetFd, bye.c_str(), bye.size(), 0);

            std::string byeMp = "SERVER:" + target.getNick() + "!~" + target.getName() + "@localhost PRIVMSG " + target.getNick() + " :You were BAN \r\n";
            if (Utils::IsSocketWritable(targetFd))
                ::send(targetFd, byeMp.c_str(), byeMp.size(), 0);
        }

        
        for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
        {
            if (chanName.compare("#" + it->second.GetName()) == 0)
            {
                User &target = targetUs->second;
                target.removeIdChan(it->second.GetId());

                it->second.RemoveUser(targetFd);
                it->second.AddUserBan(target);

                return;
            }
        }
    }
    else
    {
        std::string err = ":" + this->_ServName + " 482 " + op.getNick() + " " + chanName + " :You're not channel operator\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " tried to use the BAN command on the user " << target.getName() << RESET << std::endl;
        
        return;
    }
}

/**
 * @brief Gère la commande BANLIST pour afficher la liste des utilisateurs bannis d'un canal.
 *
 * Cette fonction permet à un opérateur de canal de consulter la liste des utilisateurs
 * actuellement bannis sur un canal spécifique. Elle vérifie :
 * - Que l'expéditeur est bien un opérateur du canal.
 * 
 * Si l'utilisateur a les droits, la fonction envoie via le socket du client une
 * liste numérotée des utilisateurs bannis. Sinon, un message d'erreur IRC est envoyé.
 *
 * @param clientSocket Le socket de l'utilisateur qui demande la liste.
 * @param chanName Le nom du canal dont la liste des bannis doit être affichée.
 */
void Server::handleBanlistCommand(int clientSocket, const std::string& chanName)
{
    std::map<int, User>::iterator itK = _User.find(clientSocket);

    if (itK == _User.end())
        return;

    User &op = itK->second;


    std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " requested a BANLIST." << RESET << std::endl;
        
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



    if (chan.GetGradeUser(op) == 0 || chan.GetGradeUser(op) == 1 || chan.GetOpChannel() == op.getSocket())
    { 

        std::string note = "";

        note = "----------- BANLIST -----------\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, note.c_str(), note.size(), 0);

        std::map<int, User> banl = chan.GetBanMap();
        int i = 0;
        for (std::map<int, User>::iterator it = banl.begin(); it != banl.end(); it++)
        {
            note = "- " + Utils::IntToString(i) + " | " + it->second.getNick() + "\r\n";
            if (Utils::IsSocketWritable(clientSocket))
                ::send(clientSocket, note.c_str(), note.size(), 0);
        }

        note = "-------------------------------\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, note.c_str(), note.size(), 0);
    }
    else
    {
        std::string err = ":" + this->_ServName + " 482 " + op.getNick() + " " + chanName + " :You're not channel operator\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " tried to use the BANLIST command." << std::endl;
        
        return;
    }
}

/**
 * @brief Gère la commande UNBAN pour retirer un utilisateur de la liste des bannis d'un canal.
 *
 * Cette fonction permet à un opérateur de canal de débannir un utilisateur spécifique
 * précédemment banni. Elle vérifie :
 * - Que l'expéditeur est bien un opérateur du canal.
 * - Que l'utilisateur ciblé est bien dans la liste des bannis.
 * 
 * Si l'utilisateur est correctement débanni, un message de confirmation est envoyé
 * au client. Sinon, un message d'erreur est transmis.
 *
 * @param clientSocket Le socket de l'utilisateur qui effectue le débannissement.
 * @param chanName Le nom du canal où l'utilisateur doit être débanni.
 * @param target Le pseudo de l'utilisateur à débannir.
 */
void Server::handleUnbanCommand(int clientSocket, const std::string& chanName, const std::string target)
{
    std::map<int, User>::iterator itK = _User.find(clientSocket);

    if (itK == _User.end())
        return;

    User &op = itK->second;

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

    if (chan.GetGradeUser(op) == 0 || chan.GetGradeUser(op) == 1 || chan.GetOpChannel() == op.getSocket())
    { 
        std::map<int, User> banl = chan.GetBanMap();
        for (std::map<int, User>::iterator it = banl.begin(); it != banl.end(); it++)
        {
            if (target.compare(it->second.getNick()) == 0 && op.getNick().compare(it->second.getNick()) == 1)
            {
                chan.RemoveUserBan(it->second.getSocket());

                std::string note = "User " + target + " has been unbanned. \r\n";
                if (Utils::IsSocketWritable(clientSocket))
                    ::send(clientSocket, note.c_str(), note.size(), 0);

                return;
            }
        }

        std::string note = "User " + target + " could not be unbanned. \r\n";
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, note.c_str(), note.size(), 0);
    }
    else
    {
        std::string err = ":" + this->_ServName + " 482 " + op.getNick() + " " + chanName + " :You're not channel operator\r\n";
        if (Utils::IsSocketWritable(clientSocket))
            ::send(clientSocket, err.c_str(), err.size(), 0);


        std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " tried to use the UNBAN command on the user " << target << std::endl;
        
        return;
    }
}
