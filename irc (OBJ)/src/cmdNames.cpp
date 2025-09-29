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

//Retourn la liste des membres dans le channel
void Server::handleNames(User& user, const std::string& line)
{
    (void)line;
    (void)user;
    
    //int clientSocket = user.getSocket();
}