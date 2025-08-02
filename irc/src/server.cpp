/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/inc.hpp"

//=================
//Constructeur
//=================

Server::Server(std::string port, std::string pass)
{

    this->_Port = Utils::PortConvert(port);
    this->_Pass = pass;

    return ;
}


Server::~Server()
{
    
    return ;
}


//===============
//Fonctions public
//===============



//===============
//Fonctions private
//===============


