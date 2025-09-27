# include "../inc/inc.hpp"
# include "../inc/server.hpp"

void Server::handleJoin(int clientSocket, User& user, const std::string& line)
{
    size_t pos0 = line.find(" ");

    char c = line[pos0 + 1];



    bool multi = false;

    std::string tmp = line.substr(5); // aprés "JOIN "
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

    if (multi == true)
    {
        size_t sp1 = tmp.find(" ");
        std::string chanName = "";
  
        for (size_t i = 1; i < sp1; i++)
            chanName += tmp[i];
    }
    else
    {
        for (size_t i = 1; i < tmp.size(); i++)
            chanName += tmp[i];
    }

    
    if (c != '#' && c != '&' && c != '+' && c != '!')
    {
        std::string err = ":server 479 " + user.getNick() + " " + chanName + " :Illegal channel name\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);

        std::cout << RED << "[JOIN]: " << user.getNick() << " a tenter d'utiliser la commande JOIN pour cree le chan : " << chanName << RESET << std::endl;
        
        return;
    }


    bool existe = false;

    int idChan;
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
        int idNeChan = this->_Chan.size();

        this->_Chan.insert(std::make_pair(idNeChan, Channel(idNeChan, chanName)));
        user.addIdChan(idNeChan);
        std::map<int, Channel>::iterator chanIt = this->_Chan.find(idNeChan);
        chanIt->second.AddUser(user);
        std::cout << "[INFO] User " << user.getNick() << " ajouté au channel " << chanIt->second.GetName() << " qu'il a cree" << std::endl;
        
        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";
        chanIt->second.BroadcastAll(joinMsg);
    }
    else
    {
        user.addIdChan(idChan);
        std::map<int, Channel>::iterator chanIt = this->_Chan.find(idChan);
        chanIt->second.AddUser(user);

        std::cout << "[INFO] User " << user.getNick() << " ajouté au channel " << chanIt->second.GetName() << std::endl;
        
        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";
        chanIt->second.BroadcastAll(joinMsg);
    }
}