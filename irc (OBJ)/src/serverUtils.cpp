
//#include "Commands.hpp"
# include "../inc/inc.hpp"

void Server::kickUser(const std::string& nickToKick, int clientSocket, const std::string& kicker)
{
    for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end(); )
    {
        if (it->second.getNick() == nickToKick)
        {
            send(it->second.getSocket(), "You have been kicked from the server.\n", 36, 0);
            it->second.closeSocket();
            FD_CLR(it->second.getSocket(), &_Readfds);

            std::cout << "[INFO] User " << nickToKick << " was kicked by " << kicker << std::endl;

            // usa a classe Kick em vez de vector
            //this->banList.addBan(nickToKick);
            this->getBanList().addBan(nickToKick);

            int kickedSocket = it->second.getSocket();
            this->_User.erase(it++);

            if (clientSocket == kickedSocket)
                return; // evitar acessar user inválido

            return; // já encontrou e kickou
        }
        else
        {
            ++it;
        }
    }

    std::cout << "[INFO] User " << nickToKick << " not found for kick." << std::endl;
}
