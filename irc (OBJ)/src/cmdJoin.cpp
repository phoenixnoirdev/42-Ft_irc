/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:00 by phkevin           #+#    #+#             */
/*   Updated: 2025/10/02 17:07:41 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"

void Server::handleJoin(int clientSocket, User& user, const std::string& line)
{
    // Parsing melhorado: JOIN #canal [senha]
    std::string tmp = line.substr(5); // após "JOIN "
    std::string chanName = "";
    std::string key = "";
    
    // Procurar primeiro espaço para separar canal da senha
    size_t spacePos = tmp.find(' ');
    
    if (spacePos == std::string::npos)
    {
        // Não há espaço - apenas o nome do canal
        chanName = tmp;
        // Remover o # se estiver presente
        if (!chanName.empty() && chanName[0] == '#')
            chanName = chanName.substr(1);
    }
    else
    {
        // Há espaço - canal e senha
        std::string fullChan = tmp.substr(0, spacePos);
        key = tmp.substr(spacePos + 1);
        
        // Remover o # do canal
        if (!fullChan.empty() && fullChan[0] == '#')
            chanName = fullChan.substr(1);
        else
            chanName = fullChan;
        
        // Limpar espaços extras da senha
        while (!key.empty() && key[0] == ' ')
            key = key.substr(1);
        while (!key.empty() && key[key.size() - 1] == ' ')
            key = key.substr(0, key.size() - 1);
    }

    // Verificar se o nome do canal é válido (não está vazio)
    if (chanName.empty())
    {
        std::string err = ":" + this->_ServName + " 479 " + user.getNick() + " " + chanName + " :Illegal channel name\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
        std::cout << RED << "[JOIN]: " << user.getNick() << " tentou usar comando JOIN com nome inválido" << RESET << std::endl;
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

    std::cout << "[DEBUG JOIN] User: " << user.getNick() << " trying to join: '" << chanName << "' with key: '" << key << "'" << std::endl;
    


    if (existe == false)
    {
        // Vérification des privilèges pour la création de canaux
        int userGrade = user.getGrade();
        if (userGrade >= 3) // Seuls les grades 0, 1, 2 peuvent créer des canaux
        {
            // Envoyer message informatif à l'utilisateur
            std::string msg = ":" + this->_ServName + " 403 " + user.getNick() + 
                             " #" + chanName + " :Privilèges insuffisants pour créer un canal\r\n";
            ::send(clientSocket, msg.c_str(), msg.size(), 0);
            
            std::cout << "[INFO] Création de canal refusée pour " << user.getNick() 
                     << " (Grade " << userGrade << ") - Canal: " << chanName << std::endl;
            return;
        }
        
        // Criar novo canal - usuário criador é automaticamente operador DO CANAL (não do sistema)
        int idNeChan = this->_Chan.size();

        this->_Chan.insert(std::make_pair(idNeChan, Channel(idNeChan, chanName)));
        user.addIdChan(idNeChan);
        // NÃO alterar o grade do sistema - preservar grade administrativo
        std::map<int, Channel>::iterator chanIt = this->_Chan.find(idNeChan);
        chanIt->second.AddUser(user);
        
        // Dar privilégios de operador DO CANAL (não do sistema) - isso deve ser feito no Channel
        std::cout << "[INFO] User " << user.getNick() << " criou e entrou no channel " << chanIt->second.GetName() << " como operador" << std::endl;
        
        // Dar privilégios automáticos se configurado
        giveAutoOpPrivileges(clientSocket, "#" + chanName);
        
        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";
        chanIt->second.BroadcastAll(joinMsg);
    }
    else
    {
        // Canal já existe - verificar modos
        std::map<int, Channel>::iterator chanIt = this->_Chan.find(idChan);
        
        // Verificar se usuário está banido
        if (chanIt->second.GetUserBan(user))
        {
            std::string err = ":" + this->_ServName + " 474 " + user.getNick() + " " + chanName + " :Cannot join channel (+b)\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }
        
        // Verificar modo invite-only (+i)
        if (chanIt->second.isInviteOnly() && !chanIt->second.isInvited(user.getNick()))
        {
            std::string err = ":" + this->_ServName + " 473 " + user.getNick() + " " + chanName + " :Cannot join channel (+i)\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }
        
        // Verificar limite de usuários (+l)
        if (!chanIt->second.canJoin())
        {
            std::string err = ":" + this->_ServName + " 471 " + user.getNick() + " " + chanName + " :Cannot join channel (+l)\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }
        
        // Verificar senha do canal (+k)
        if (chanIt->second.hasKey())
        {
            std::cout << "[DEBUG KEY] Canal tem senha. Provided: '" << key << "', Expected: '" << chanIt->second.getKey() << "'" << std::endl;
        }
        
        if (!chanIt->second.checkKey(key))
        {
            std::cout << "[DEBUG KEY] BLOCKING JOIN: Wrong key for " << user.getNick() << std::endl;
            std::string err = ":" + this->_ServName + " 475 " + user.getNick() + " " + chanName + " :Cannot join channel (+k)\r\n";
            ::send(clientSocket, err.c_str(), err.size(), 0);
            return;
        }
        
        // Remover convite se estava na lista (usado uma vez)
        if (chanIt->second.isInvited(user.getNick()))
        {
            chanIt->second.removeInvite(user.getNick());
        }
        
        user.addIdChan(idChan);
        // NÃO alterar o grade do sistema - preservar grade administrativo original
        chanIt->second.AddUser(user);

        std::cout << "[INFO] User " << user.getNick() << " entrou no channel " << chanIt->second.GetName() << std::endl;
        
        // Dar privilégios automáticos se configurado
        giveAutoOpPrivileges(clientSocket, "#" + chanName);
        
        std::string joinMsg = ":" + user.getNick() + "!" + user.getName() + " JOIN #" + chanIt->second.GetName() + "\r\n";
        chanIt->second.BroadcastAll(joinMsg);
    }
}