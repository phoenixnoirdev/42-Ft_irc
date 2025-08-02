/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
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


//===============
//Fonctions public
//===============

/**
 * @brief Convertit une chaîne en entier pour définir le port du serveur.
 *
 * Cette fonction tente de convertir l'argument fourni en un entier.
 * Si l'argument ne peut pas être converti ou s'il reste des caractères
 * après la conversion, une erreur est levée via Error::ErrorServ(1).
 * Si le port est en dehors des bornes valides (1 à PORT_MAX), une erreur
 * est levée via Error::ErrorServ(2). Si tout est valide, le port
 * converti est stocké dans l'attribut _Port du serveur.
 *
 * @param arg Chaîne représentant un port à convertir (doit être un entier).
 */
uint16_t Utils::PortConvert(const std::string& arg)
{
	int port;

	std::stringstream ss(arg);

	if (!(ss >> port) || !ss.eof())
        Error::ErrorServ(1);

	if (port < 1 || port > PORT_MAX)// 
        Error::ErrorServ(2);
    
	return static_cast<uint16_t>(port);
}


//===============
//Fonctions private
//===============


