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

# include "../inc/botinc.hpp"

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
        std::cerr << RED << "Error: port conversion fail." << RESET << std::endl; 

	if (port < 1 || port > PORT_MAX)
        std::cerr << RED << "Error: Port out of range." << RESET << std::endl; 
    
	return static_cast<uint16_t>(port);
}

/**
 * @brief Convertit une adresse IP en structure in_addr (IPv4).
 *
 * Utilise getaddrinfo pour résoudre une adresse IP passée en chaîne
 * de caractères (ipStr) vers une structure sockaddr, puis extrait
 * l'adresse IPv4 (in_addr) correspondante.
 *
 * En cas d'échec de résolution, une erreur est levée via
 * Error::ErrorServ(3, gai_strerror(status)).
 *
 * @param ipStr Adresse IP en format chaîne (ex: "127.0.0.1").
 * @return struct in_addr Adresse IP convertie au format binaire.
 */
struct in_addr Utils::IpConvert(const std::string& ipStr)
{
    struct addrinfo hints;
    struct addrinfo* res = NULL;
    struct in_addr outAddr;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(ipStr.c_str(), NULL, &hints, &res);
    if (status != 0 || !res)
    {
        std::cerr << RED << "Error: Getaddrinfo error: " << gai_strerror(status) << RESET << std::endl;
        outAddr.s_addr = INADDR_NONE;
        return outAddr;
    }

    struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
    outAddr = ipv4->sin_addr;

    freeaddrinfo(res);
    return outAddr;
}

/**
 * @brief Convertit un entier en chaîne de caractères.
 *
 * Utilise un std::stringstream pour convertir un entier en string.
 *
 * @param i L'entier à convertir.
 * @return std::string Représentation texte de l'entier.
 */
std::string Utils::IntToString(int i)
{
    std::stringstream ss;

    ss << i;

    return ss.str();
}


//===============
//Fonctions private
//===============


