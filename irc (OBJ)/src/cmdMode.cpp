/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 15:30:00 by luis-fif         #+#    #+#             */
/*   Updated: 2025/10/02 15:30:00 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/inc.hpp"

/**
 * @brief Divide uma string em tokens usando espaço como delimitador
 * 
 * @param str String a ser dividida
 * @return std::vector<std::string> Vetor com os tokens
 */
std::vector<std::string> splitString(const std::string& str)
{
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (iss >> token)
        tokens.push_back(token);
        
    return tokens;
}

/**
 * @brief Procura um canal pelo nome no mapa de canais do servidor
 * 
 * @param channelName Nome do canal a procurar
 * @param channels Mapa de canais do servidor
 * @return Channel* Ponteiro para o canal encontrado ou NULL se não encontrado
 */
Channel* findChannelByName(const std::string& channelName, std::map<int, Channel>& channels)
{
    // Remover o # do nome se estiver presente para padronizar a busca
    std::string searchName = channelName;
    if (!searchName.empty() && searchName[0] == '#')
        searchName = searchName.substr(1);
    
    for (std::map<int, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->second.GetName() == searchName)
            return &(it->second);
    }
    
    return NULL;
}

/**
 * @brief Procura um usuário pelo nick no canal
 * 
 * @param nick Nick do usuário a procurar
 * @param channel Canal onde procurar
 * @return User* Ponteiro para o usuário encontrado ou NULL se não encontrado
 */
User* findUserInChannelByNick(const std::string& nick, Channel& channel, std::map<int, User>& users)
{
    // Procurar em todas as grades do canal
    for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (it->second.getNick() == nick && channel.GetGradeUser(it->second) >= 0)
            return &(it->second);
    }
    return NULL;
}

/**
 * @brief Envia uma mensagem de erro IRC para o cliente
 * 
 * @param clientSocket Socket do cliente
 * @param errorCode Código do erro IRC
 * @param message Mensagem do erro
 */
void sendIRCError(int clientSocket, int errorCode, const std::string& message)
{
    std::ostringstream oss;
    oss << ":localhost " << errorCode << " * " << message << "\r\n";
    std::string response = oss.str();
    send(clientSocket, response.c_str(), response.size(), 0);
}

/**
 * @brief Envia resposta de modo IRC para o canal
 * 
 * @param channel Canal onde enviar a resposta
 * @param user Usuário que executou o comando
 * @param modeString String do modo aplicado
 * @param param Parâmetro do modo (se houver)
 */
void sendModeResponse(Channel& channel, const User& user, const std::string& modeString, const std::string& param = "")
{
    std::string response = ":" + user.getNick() + "!~" + user.getName() + "@localhost MODE " + channel.GetName() + " " + modeString;
    
    if (!param.empty())
        response += " " + param;
        
    response += "\r\n";
    
    channel.BroadcastAll(response);
}

/**
 * @brief Processa comando MODE para definir/consultar modos do canal
 * 
 * Formato: MODE #canal [+/-modo] [parâmetro]
 * 
 * Modos suportados:
 * +i/-i : Invite-only channel
 * +t/-t : Topic restriction to ops only
 * +k/-k : Channel key (password)
 * +o/-o : Give/take operator privilege
 * +l/-l : Set/remove user limit
 * +m/-m : Moderated channel
 * 
 * @param clientSocket Socket do cliente que enviou o comando
 * @param line Linha completa do comando MODE
 * @param users Mapa de usuários do servidor
 * @param channels Mapa de canais do servidor
 */
void handleModeCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 2)
    {
        sendIRCError(clientSocket, 461, "MODE :Not enough parameters");
        return;
    }
    
    std::string channelName = tokens[1];
    
    // Verificar se o canal existe
    Channel* channel = findChannelByName(channelName, channels);
    if (!channel)
    {
        sendIRCError(clientSocket, 403, channelName + " :No such channel");
        return;
    }
    
    // Verificar se o usuário existe
    if (users.find(clientSocket) == users.end())
    {
        sendIRCError(clientSocket, 401, ":No such nick/channel");
        return;
    }
    
    User& user = users[clientSocket];
    
    // Se não há mais parâmetros, mostrar modos atuais
    if (tokens.size() == 2)
    {
        std::string modes = channel->getModeString();
        std::string response = ":localhost 324 " + user.getNick() + " " + channelName + " " + modes + "\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
        return;
    }
    
    std::string modeString = tokens[2];
    std::string param = (tokens.size() > 3) ? tokens[3] : "";
    
    // Verificar se o usuário é operador do canal
    if (channel->GetGradeUser(user) == 3 || channel->GetGradeUser(user) == 2)
    {
        sendIRCError(clientSocket, 482, channelName + " :You're not channel operator --- dd");
        return;
    }
    
    // Processar cada modo na string
    if (modeString.length() < 2)
    {
        sendIRCError(clientSocket, 472, modeString + " :is unknown mode char to me");
        return;
    }
    
    bool adding = (modeString[0] == '+');
    
    // Processar cada caracter de modo
    for (size_t i = 1; i < modeString.length(); ++i)
    {
        char mode = modeString[i];
        std::string singleMode = std::string(1, modeString[0]) + mode;
        
        switch(mode)
        {
            case 'i': // Invite-only
                channel->setInviteOnly(adding);
                sendModeResponse(*channel, user, singleMode);
                break;
                
            case 't': // Topic restriction
                channel->setTopicOpOnly(adding);
                sendModeResponse(*channel, user, singleMode);
                break;
                
            case 'k': // Channel key
                if (adding)
                {
                    if (param.empty())
                    {
                        sendIRCError(clientSocket, 461, "MODE +k :Not enough parameters");
                        continue;
                    }
                    channel->setKey(param);
                    sendModeResponse(*channel, user, singleMode, param);
                }
                else
                {
                    channel->removeKey();
                    sendModeResponse(*channel, user, singleMode);
                }
                break;
                
            case 'o': // Operator privilege
                if (param.empty())
                {
                    sendIRCError(clientSocket, 461, "MODE +o :Not enough parameters");
                    continue;
                }
                
                // Verificar se o usuário alvo existe no canal
                if (!findUserInChannelByNick(param, *channel, users))
                {
                    sendIRCError(clientSocket, 401, param + " :No such nick/channel");
                    continue;
                }
                
                if (adding)
                    channel->giveOp(param);
                else
                    channel->removeOp(param);
                    
                sendModeResponse(*channel, user, singleMode, param);
                break;
                
            case 'l': // User limit
                if (adding)
                {
                    if (param.empty())
                    {
                        sendIRCError(clientSocket, 461, "MODE +l :Not enough parameters");
                        continue;
                    }
                    
                    int limit = std::atoi(param.c_str());
                    if (limit <= 0)
                    {
                        sendIRCError(clientSocket, 696, param + " :Invalid limit");
                        continue;
                    }
                    
                    channel->setUserLimit(limit);
                    sendModeResponse(*channel, user, singleMode, param);
                }
                else
                {
                    channel->removeUserLimit();
                    sendModeResponse(*channel, user, singleMode);
                }
                break;
                
            case 'm': // Moderated
                channel->setModerated(adding);
                sendModeResponse(*channel, user, singleMode);
                break;
                
            default:
                sendIRCError(clientSocket, 472, std::string(1, mode) + " :is unknown mode char to me");
                break;
        }
    }
}

/**
 * @brief Processa comando INVITE para convidar usuário para canal invite-only
 * 
 * Formato: INVITE nick #canal
 * 
 * @param clientSocket Socket do cliente que enviou o comando
 * @param line Linha completa do comando INVITE
 * @param users Mapa de usuários do servidor
 * @param channels Mapa de canais do servidor
 */
void handleInviteCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 3)
    {
        sendIRCError(clientSocket, 461, "INVITE :Not enough parameters");
        return;
    }
    
    std::string targetNick = tokens[1];
    std::string channelName = tokens[2];
    
    // Verificar se o canal existe
    Channel* channel = findChannelByName(channelName, channels);
    if (!channel)
    {
        sendIRCError(clientSocket, 403, channelName + " :No such channel");
        return;
    }
    
    // Verificar se o usuário que convida existe
    if (users.find(clientSocket) == users.end())
        return;
        
    User& inviter = users[clientSocket];
    
    // Procurar usuário alvo
    User* target = NULL;
    for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (it->second.getNick() == targetNick)
        {
            target = &(it->second);
            break;
        }
    }
    
    if (!target)
    {
        sendIRCError(clientSocket, 401, targetNick + " :No such nick/channel");
        return;
    }
    
    // Verificar se o usuário já está no canal
    if (channel->GetGradeUser(*target) >= 0)
    {
        sendIRCError(clientSocket, 443, targetNick + " " + channelName + " :is already on channel");
        return;
    }
    
    // Adicionar convite
    channel->addInvite(targetNick);
    
    // Notificar o usuário convidado
    std::string inviteMsg = ":" + inviter.getNick() + "!~" + inviter.getName() + "@localhost INVITE " + targetNick + " " + channelName + "\r\n";
    send(target->getSocket(), inviteMsg.c_str(), inviteMsg.size(), 0);
    
    // Confirmar para quem convidou
    std::string confirmMsg = ":localhost 341 " + inviter.getNick() + " " + targetNick + " " + channelName + "\r\n";
    send(clientSocket, confirmMsg.c_str(), confirmMsg.size(), 0);
}

/**
 * @brief Processa comando TOPIC para definir/consultar tópico do canal
 * 
 * Formato: TOPIC #canal [novo_tópico]
 * 
 * @param clientSocket Socket do cliente que enviou o comando
 * @param line Linha completa do comando TOPIC
 * @param users Mapa de usuários do servidor
 * @param channels Mapa de canais do servidor
 */
void handleTopicCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels)
{
    std::vector<std::string> tokens = splitString(line);
    
    if (tokens.size() < 2)
    {
        sendIRCError(clientSocket, 461, "TOPIC :Not enough parameters");
        return;
    }
    
    std::string channelName = tokens[1];
    
    // Verificar se o canal existe
    Channel* channel = findChannelByName(channelName, channels);
    if (!channel)
    {
        sendIRCError(clientSocket, 403, channelName + " :No such channel");
        return;
    }
    
    // Verificar se o usuário existe
    if (users.find(clientSocket) == users.end())
        return;
        
    User& user = users[clientSocket];
    
    // Se não há mais parâmetros, mostrar tópico atual
    if (tokens.size() == 2)
    {
        std::string topic = channel->GetTopic();
        if (topic.empty())
        {
            std::string response = ":localhost 331 " + user.getNick() + " " + channelName + " :No topic is set\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        }
        else
        {
            std::string response = ":localhost 332 " + user.getNick() + " " + channelName + " :" + topic + "\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        }
        return;
    }
    
    // Verificar se pode alterar tópico
    if (!channel->canChangeTopic(user))
    {
        sendIRCError(clientSocket, 482, channelName + " :You're not channel operator");
        return;
    }
    
    // Extrair novo tópico (tudo após "TOPIC #canal ")
    size_t pos = line.find(" :");
    std::string newTopic;
    
    if (pos != std::string::npos)
        newTopic = line.substr(pos + 2);
    else
    {
        // Se não tem ":", pegar tudo após o segundo espaço
        size_t firstSpace = line.find(' ');
        if (firstSpace != std::string::npos)
        {
            size_t secondSpace = line.find(' ', firstSpace + 1);
            if (secondSpace != std::string::npos)
                newTopic = line.substr(secondSpace + 1);
        }
    }
    
    channel->SetTopic(newTopic);
    
    // Notificar mudança para todos no canal
    std::string response = ":" + user.getNick() + "!~" + user.getName() + "@localhost TOPIC " + channelName + " :" + newTopic + "\r\n";
    channel->BroadcastAll(response);
}