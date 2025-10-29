/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdLogin.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:14 by phkevin           #+#    #+#             */
/*   Updated: 2025/10/17 14:34:45 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"

/**
 * @brief Authentifie un utilisateur après la connexion.
 *
 * Vérifie le mot de passe fourni par l'utilisateur et s'assure que le
 * pseudo n'est pas déjà utilisé par un autre utilisateur connecté. Si
 * l'authentification échoue, le socket de l'utilisateur est fermé et
 * il est supprimé de la liste des utilisateurs. Si elle réussit, le grade
 * de l'utilisateur est défini à 3 (utilisateur normal) et son état
 * d'authentification est activé.
 *
 * @param clientSocket Le socket de l'utilisateur qui tente de se connecter.
 * @param user Référence à l'objet User représentant l'utilisateur.
 */
void Server::handleLogin(int clientSocket, User& user)
{
    if (!PassCont(user.getPass()))
    {
        send(clientSocket, "Invalid password\n", 17, 0);
        close(clientSocket);
        this->_User.erase(clientSocket);

        return;
    }

    if (NickIsList(user.getNick()) == true)
    {
        std::cout << "[INFO] User " << user.getNick() << "@" << user.getName() << " deconnecter nick doublon !" << std::endl;
        send(clientSocket, "Nickname use\n", 13, 0);
        close(clientSocket);
        this->_User.erase(clientSocket);

        return;
    }

    user.setGrade(3);
    user.setAuth(true);

    std::cout << "[INFO] User " << user.getNick() << "@" << user.getName() << " authentifié !" << std::endl;
}

/**
 * @brief Envoie les messages de bienvenue IRC à un utilisateur nouvellement connecté.
 *
 * Cette fonction envoie les messages standards 001 à 004 conformément au protocole IRC,
 * incluant le message de bienvenue, les informations sur le serveur, sa version et les modes disponibles.
 *
 * @param user Référence constante vers l'objet User représentant l'utilisateur à accueillir.
 */
void Server::handleWelcome(const User& user)
{
    std::string msg;

    msg = ":" + _ServName + " 001 " + user.getNick() + " :Welcome to the IRC server " + user.getNick() + "!" + user.getName() + "@localhost\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

    msg = ":" + _ServName + " 002 " + user.getNick() + " :Your host is " + _ServName + ", running version 1.0\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

    msg = ":" + _ServName + " 003 " + user.getNick() + " :This server was created today\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);

    msg = ":" + _ServName + " 004 " + user.getNick() + " " + _ServName + " 1.0 ao mtov\r\n";
    ::send(user.getSocket(), msg.c_str(), msg.size(), 0);
}