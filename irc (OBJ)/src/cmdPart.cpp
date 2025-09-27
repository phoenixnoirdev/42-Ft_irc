# include "../inc/inc.hpp"
# include "../inc/server.hpp"

void Server::handleQuit(int clientSocket, User& user, const std::string& line)
{
    size_t pos0 = line.find(" ");

    char c = line[pos0 + 1];



    bool multi = false;

    std::string tmp = line.substr(5); // aprés "PART "
    std::string chanName = "";
    
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (!(c != ' '))
        {
            multi = true;
            break;
        }
        i++;
    }


    size_t sp1 = tmp.find(":");

    if (multi == true)
    {
        std::string chanName = "";
  
        for (size_t i = 1; i < (sp1 - 1); i++)
            chanName += tmp[i];
    }
    else
    {
        for (size_t i = 1; i < (sp1 - 1); i++)
            chanName += tmp[i];
    }

    
    if (c != '#' && c != '&' && c != '+' && c != '!')
    {
        std::string err = ":server 479 " + user.getNick() + " " + chanName + " :Illegal channel name\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[PART]: " << user.getNick() << " a tenter d'utiliser la commande PART pour quitter le chan : " << chanName << RESET << std::endl;
        
        return;
    }


    bool existe = false;

    int idChan;
    std::cout << chanName << std::endl;
    for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
    {
        if (chanName.compare(it->second.GetName()) == 0)
        {
            idChan = it->second.GetId();
            existe = true;
            break;
        }
    }

    if (existe == false)
    {
        std::cout << "[INFO] User " << user.getNick() << " a essayer de quitter le channel " << chanName << " mais le channel n'existe pas." << std::endl;
        
        std::string err = ":server 403 " + user.getNick() + " " + chanName + " :You're not on that channel\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
    }
    else
    {
        user.removeIdChan(idChan);
        std::map<int, Channel>::iterator chanIt = this->_Chan.find(idChan);
        chanIt->second.RemoveUser(user.getSocket());

        std::cout << "[INFO] User " << user.getNick() << " a quitter le channel " << chanIt->second.GetName() << std::endl;
        
        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " QUIT #" + chanIt->second.GetName() + "\r\n";
        chanIt->second.BroadcastAll(joinMsg);

        if (chanIt->second.GetPop() == 0)
        {
            std::cout << "[INFO] Suppression du channel " << chanIt->second.GetName() << " car le channel est vide " << std::endl;

            this->_Chan.erase(idChan);
        }
    }
}