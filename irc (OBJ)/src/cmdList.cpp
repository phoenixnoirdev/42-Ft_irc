/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdList.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:13 by phkevin           #+#    #+#             */
/*   Updated: 2025/10/02 17:45:19 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"
# include "../inc/utils.hpp"

//retourne la liste des canaux
void Server::handleList(User& user)
{
    std::cout << "[LIST] Processando LIST para: " << user.getNick() << std::endl;
    int clientSocket = user.getSocket();
    std::string msg = "";

    // RPL_LISTSTART (321)
    msg = ":" + this->_ServName + " 321 " + user.getNick() + " Channel :Users  Name\r\n";
    ::send(clientSocket, msg.c_str(), msg.size(), 0);
    std::cout << "[LIST] Start enviado" << std::endl;

    // RPL_LIST (322) - FORMATO HEXCHAT ULTRA-COMPATÍVEL - ORDENADO
    for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
    {
        std::string channelName = "#" + it->second.GetName();
        int userCount = it->second.GetPop();
        std::string topic = it->second.getTopic();
        
        // FORÇAR topic ASCII puro para HexChat
        if (topic.empty()) {
            topic = "Canal disponivel";
        }
        
        // Limpar caracteres não-ASCII do topic
        std::string cleanTopic = "";
        for (size_t i = 0; i < topic.size(); i++) {
            char c = topic[i];
            if (c >= 32 && c <= 126) { // ASCII imprimível
                cleanTopic += c;
            } else if (c == -61 || c == -87) { // UTF-8 para acentos - ignorar
                continue;
            } else {
                cleanTopic += '?'; // Substituir por ?
            }
        }
        topic = cleanTopic;
        
        // Garantir que o canal tenha pelo menos 1 usuário
        if (userCount <= 0) {
            userCount = 1; // HexChat pode filtrar canais com 0 usuários
        }
        
        // Formato RFC2812 RIGOROSO - exatamente como outros servidores IRC
        msg = ":" + this->_ServName + " 322 " + user.getNick() + " " + channelName + " " + 
              Utils::IntToString(userCount) + " :" + topic + "\r\n";
        
        // Enviar com flush explícito
        ssize_t bytes_sent = ::send(clientSocket, msg.c_str(), msg.size(), 0);
        
        // Debug SUPER detalhado
        std::cout << "[LIST DEBUG] Enviado RPL_LIST: " << msg;
        std::cout << "[LIST DEBUG] Bytes enviados: " << bytes_sent << "/" << msg.size() << std::endl;
        std::cout << "[LIST DEBUG] Canal: '" << channelName << "' Pop: " << userCount 
                  << " Topic: '" << topic << "' Socket: " << clientSocket << std::endl;
        
        // Debug hexadecimal da mensagem
        std::cout << "[LIST HEX] Mensagem em hex: ";
        for (size_t i = 0; i < msg.size(); i++) {
            printf("%02x ", (unsigned char)msg[i]);
        }
        std::cout << std::endl;
        
        // Pausa muito pequena entre canais
        usleep(5000); // 5ms para dar tempo ao cliente processar
        std::cout << "[LIST DEBUG] Canal: '" << channelName << "' Usuários: " << userCount << " Topic: '" << topic << "'" << std::endl;
        std::cout << "[LIST DEBUG] Tamanho da mensagem: " << msg.size() << " bytes" << std::endl;
    }

    // RPL_LISTEND (323) - Fim obrigatório da lista  
    msg = ":" + this->_ServName + " 323 " + user.getNick() + " :End of /LIST\r\n";
    ::send(clientSocket, msg.c_str(), msg.size(), 0);
    std::cout << "[LIST DEBUG] Enviado RPL_LISTEND: " << msg << std::endl;
    
    std::cout << "[LIST] Lista completa enviada: " << this->_Chan.size() << " canais para " << user.getNick() << std::endl;
}