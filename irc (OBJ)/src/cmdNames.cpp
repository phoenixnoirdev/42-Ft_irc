/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdNames.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:47:14 by phkevin           #+#    #+#             */
/*   Updated: 2025/09/29 12:08:30 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"
# include "../inc/server.hpp"
/*
Retourn la liste des membres dans le channel

Le protocole permet de saisire les commmande suivante /NAME ou /NAME #default ou /NAME #default #test


/NAMES : Retourn les liste d'user de tout les channel ou l'user est connecter

/NAMES #default : Retourn a liste des user du channel default

/NAME #default #test : Retourn les liste des channel #defaul et test si l'user est connecter dedans si il n'es pas
connecter dans le channel, alors on doit controler si le channel existe et si oui si il est priver/secret ou non


Si le channel n’existe pas, le serveur peut simplement renvoyer un 353 vide + 366 ou un message d’erreur

Si le channel est priver ou secret alors l'utilisateur ne reçois pas la liste des user si il n'est pas connecter 

*/


void Server::handleNames(User& user, const std::string& line)
{
    (void)line;
    (void)user;
    
    //int clientSocket = user.getSocket();
}