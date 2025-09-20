
//#include "Commands.hpp"
# include "../inc/inc.hpp"


// cmdBan.cpp
#include "../inc/server.hpp"   // garante a declaração do Server
#include <sstream>

/*
void Server::handleKickCommand(int clientSocket, const std::string& line)
{
    // 0) Permet controler si l'user existe
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
        std::string err = ":server 461 " + kicker.getNick() + " KICK :Not enough parameters\r\n";
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
        std::string err = ":server 461 " + kicker.getNick() + " KICK :Not enough parameters\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
    }


    // -) Ajout l'user au BAN Global
    //getBanList().addBan(targetNick);


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
        std::string note = ":server NOTICE " + kicker.getNick() + " :User " + targetNick + " not online.\r\n";
        ::send(clientSocket, note.c_str(), note.size(), 0);
        return;
    }

    // 5) Log serveur
    std::cout << "[" << RED << "KICK" << RESET << "]: " << chanName << " " << targetNick << " " << reason << " / kicker = " << kicker.getName() << std::endl;


    // 6) Informe la cible de sont kick
    {
        const User &target = targetUs->second;
        std::string bye = ":server NOTICE " + target.getNick() + " :You were KICK (" + (reason.empty() ? "no reason" : reason) + ")\r\n";
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
            target.setIdChan(-1);

            return;
        }
    }
}
*/


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
        if (maskTarg.compare(it->second.getName()) == 0)
        {
            socketUser = it->second.getSocket();
            break;
        }
    }
    std::map<int, User>::iterator tar = _User.find(socketUser);
    
    if (tar == _User.end())
        return;

    User &target = tar->second;


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

/*
void Server::handleUnbanCommand(int clientSocket, const std::string& line)
{
    std::map<int, User>::iterator itK = _User.find(clientSocket);
    if (itK == _User.end())
        return;
    User &who = itK->second;

    // UNBAN <nick>
    if (line.size() <= 6) {
        std::string err = ":server 461 " + who.getNick() + " UNBAN :Not enough parameters\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
        return;
    }
    std::string nick = line.substr(6);
    // tira espaços no fim/início (simples)
    while (!nick.empty() && (nick[0]==' '||nick[0]=='\t')) nick.erase(0,1);
    while (!nick.empty() && (nick[nick.size()-1]==' '||nick[nick.size()-1]=='\t')) nick.erase(nick.size()-1);

    bool ok = getBanList().removeBan(nick);
    std::string ans = ":server NOTICE " + who.getNick() + " :UNBAN " + nick + (ok ? " ok" : " (not banned)") + "\r\n";
    ::send(clientSocket, ans.c_str(), ans.size(), 0);
}
*/