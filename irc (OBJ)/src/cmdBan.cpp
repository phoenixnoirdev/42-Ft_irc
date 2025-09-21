
//#include "Commands.hpp"
# include "../inc/inc.hpp"


// cmdBan.cpp
#include "../inc/server.hpp"   // garante a declaração do Server
#include "../inc/utils.hpp"
#include <sstream>

void Server::handleBanCommand(int clientSocket, const std::string& chanName, const std::string maskTarg)
{
    // 0) Permet retourner l'user qui lance la commande
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


    if (op.getNick().compare(target.getNick()) == 0)
    {
        std::string note = "Vous ne pouvez pas vous ban vous même.\r\n";
        ::send(clientSocket, note.c_str(), note.size(), 0);
        return;
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


    if (chan.GetGradeUser(op) == 0 || chan.GetGradeUser(op) == 1)
    { 
        // 2) Cherche l'utilisateur dans la liste
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

        // 3) Retour d’information dans le channel
        {
            std::string reply = ":" + op.getNick() + "!~" + op.getName() + "@localhost BAN " + chanName + " " + target.getName() + "\r\n";

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
            std::string note = ":server NOTICE " + op.getNick() + " :User " + target.getName() + " not online.\r\n";
            ::send(clientSocket, note.c_str(), note.size(), 0);
            return;
        }
        

        
        // 5) Log serveur
        std::cout << "[" << RED << "BAN" << RESET << "]: " << chanName << " " << target.getName() <<  " / OP = " << op.getName() << std::endl;
        

        
        // 6) Informe la cible de sont ban
        {
            const User &target = targetUs->second;

            std::string bye = ":server NOTICE " + target.getNick() + " :You were BAN \r\n";
            ::send(targetFd, bye.c_str(), bye.size(), 0);

            std::string byeMp = "SERVER:" + target.getNick() + "!~" + target.getName() + "@localhost PRIVMSG " + target.getNick() + " :You were BAN \r\n";
            ::send(targetFd, byeMp.c_str(), byeMp.size(), 0);
        }

        
        // 8) Deconnecte la cible du channel et set a -1 pour signifier qu'il est hors channel
        for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
        {
            if (chanName.compare("#" + it->second.GetName()) == 0)
            {
                User &target = targetUs->second;
                target.setIdChan(-1);

                it->second.RemoveUser(targetFd);
                it->second.AddUserBan(target);

                return;
            }
        }
    }
    else
    {
        std::string err = ":server 482 " + op.getNick() + " " + chanName + " :You're not channel operator\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " a tenter d'utiliser la commande BAN sur l'user " << target.getName() << std::endl;
        
        return;
    }
}


void Server::handleBanlistCommand(int clientSocket, const std::string& chanName)
{
    // 0) Permet retourner l'user qui lance la commande
    std::map<int, User>::iterator itK = _User.find(clientSocket);

    if (itK == _User.end())
        return;

    User &op = itK->second;


    std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " a demander un BANLIST." << RESET << std::endl;
        
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



    if (chan.GetGradeUser(op) == 0 || chan.GetGradeUser(op) == 1)
    { 

        std::string note = "";

        note = "----------- BANLIST -----------\r\n";
        ::send(clientSocket, note.c_str(), note.size(), 0);

        std::map<int, User> banl = chan.GetBanMap();
        int i = 0;
        for (std::map<int, User>::iterator it = banl.begin(); it != banl.end(); it++)
        {
            note = "- " + Utils::IntToString(i) + " | " + it->second.getNick() + "\r\n";
            ::send(clientSocket, note.c_str(), note.size(), 0);
        }

        note = "-------------------------------\r\n";
        ::send(clientSocket, note.c_str(), note.size(), 0);
    }
    else
    {
        std::string err = ":server 482 " + op.getNick() + " " + chanName + " :You're not channel operator\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " a tenter d'utiliser la commande BANLIST." << std::endl;
        
        return;
    }
}


void Server::handleUnbanCommand(int clientSocket, const std::string& chanName, const std::string target)
{
        // 0) Permet retourner l'user qui lance la commande
        std::map<int, User>::iterator itK = _User.find(clientSocket);

        if (itK == _User.end())
            return;
    
        User &op = itK->second;

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
    
        if (chan.GetGradeUser(op) == 0 || chan.GetGradeUser(op) == 1)
        { 
            std::map<int, User> banl = chan.GetBanMap();
            for (std::map<int, User>::iterator it = banl.begin(); it != banl.end(); it++)
            {
                if (target.compare(it->second.getNick()) == 0 && !op.getNick().compare(it->second.getNick()) == 0)
                {
                    chan.RemoveUserBan(it->second.getSocket());

                    std::string note = "L'utilisateur " + target + " a été unban. \r\n";
                    ::send(clientSocket, note.c_str(), note.size(), 0);

                    return;
                }
            }

            std::string note = "L'utilisateur " + target + " n'a pas pus être unban. \r\n";
            ::send(clientSocket, note.c_str(), note.size(), 0);
        }
        else
        {
            std::string err = ":server 482 " + op.getNick() + " " + chanName + " :You're not channel operator\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
    

            std::cout << RED << "[BAN]: " << chanName << " " << op.getName() << " a tenter d'utiliser la commande UNBAN sur l'user " << target << std::endl;
            
            return;
        }
}
