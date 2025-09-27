//#include "Commands.hpp"
# include "../inc/inc.hpp"


// cmdKick.cpp
#include "../inc/server.hpp"   // garante a declaração do Server


void Server::handleBrodcastMsgChann(int clientSocket, User& user, std::string line, int idchan)
{
    //(void) clientSocket;
    //(void) user;
    //(void) line;
    
    std::map<int, Channel>::iterator chanIt = this->_Chan.find(idchan);

    size_t pos = line.find(" :");
    std::string msg;

    if (pos != std::string::npos)
        msg = line.substr(pos + 2);
    else
        msg = "";

    std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG #" + chanIt->second.GetName() + " :" + msg + "\r\n";

    if (DEBUG == true)
        std::cout << "[DEBUG] ircMsg: " << ircMsg << std::endl;

    chanIt->second.Broadcast(ircMsg, clientSocket);

    std::cout << CYAN << chanIt->second.GetName() << " / " << YELLOW << user.getName() << RESET << ": " << msg << std::endl;
    
}

void Server::handleBrodcastPrivateMsg(User& user, std::string line)
{
    size_t pos0 = line.find(" ");
    size_t pos1 = line.find(" :");

    std::string userTarg = "";

    for (size_t i = pos0 + 1; i < pos1; i++)
        userTarg += line[i];

    
    size_t pos = line.find(" :");
    std::string msg;

    if (pos != std::string::npos)
        msg = line.substr(pos + 2);
    else
        msg = "";


    if (IsNickIsList(userTarg) == true)
    {
        int clientSock = 0;

        for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end();)
        {
            if (userTarg.compare(it->second.getNick()) == 0)
            {
                clientSock = it->second.getSocket();
                break;
            }
            ++it;
        }

        std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + userTarg + " :" + msg + "\r\n";
        send(clientSock, ircMsg.c_str(), ircMsg.size(), 0);

        std::cout << CYAN << "MP " << " / " << YELLOW << user.getName() << RESET << " to " << YELLOW << userTarg << RESET <<": " << msg << std::endl;
    }
    else
    {
        std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + user.getNick() + " :Le destinataire de votre message priver n'est pas connecter (" + msg + ")\r\n";
        send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);

        std::cout << RED << "MP ERROR (TARGET NOT LOG)" << " / " << YELLOW << user.getName() << RESET << ": " << msg << std::endl;
    }
}

void Server::handleBrodcastMsgKB(User& user, std::string line)
{
    size_t pos = line.find(" :");
    std::string msg;

    if (pos != std::string::npos)
        msg = line.substr(pos + 2);
    else
        msg = "";

    std::string ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + user.getNick() + " : Vous avez été kick ou ban. Le message n'a pas été envoyer (" + msg + ")\r\n";
    send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);

    std::cout << CYAN << "USER KICKED/BAN MSG RECEP" << " / " << YELLOW << user.getName() << RESET << ": " << msg << std::endl;
}