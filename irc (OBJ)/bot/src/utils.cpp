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
 * @brief Convertit une chaîne de caractères en numéro de port valide.
 * 
 * Cette fonction tente de convertir la chaîne fournie en entier, 
 * vérifie que la conversion est correcte et que le port est dans 
 * la plage autorisée (1 à PORT_MAX).
 * 
 * @param arg Chaîne représentant le numéro de port.
 * @return uint16_t Le numéro de port converti.
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
 * @brief Convertit une chaîne représentant une adresse IPv4 en structure in_addr.
 * 
 * Cette fonction utilise getaddrinfo pour résoudre la chaîne IP fournie et 
 * renvoie l'adresse IPv4 correspondante sous forme de struct in_addr. 
 * En cas d'erreur, la fonction renvoie INADDR_NONE.
 * 
 * @param ipStr Chaîne contenant l'adresse IP à convertir.
 * @return struct in_addr Structure contenant l'adresse IPv4 convertie.
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
 * Cette fonction utilise un stringstream pour transformer un entier en 
 * représentation textuelle.
 * 
 * @param i L'entier à convertir.
 * @return std::string Chaîne correspondant à l'entier fourni.
 */
std::string Utils::IntToString(int i)
{
    std::stringstream ss;

    ss << i;

    return ss.str();
}