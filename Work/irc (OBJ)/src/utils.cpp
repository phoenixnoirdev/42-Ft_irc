/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/10/31 13:51:37 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"

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
        Error::ErrorServ(1, "");

	if (port < 1 || port > PORT_MAX) 
        Error::ErrorServ(2, "");
    
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

 /**
 * @brief Convertit une adresse IP sous forme de chaîne en structure in_addr.
 *
 * Cette fonction résout une adresse IPv4 donnée sous forme de texte et
 * retourne la structure correspondante utilisable par les fonctions réseau.
 *
 * @param ipStr Chaîne contenant l'adresse IP à convertir (ex: "127.0.0.1").
 * @return struct in_addr Structure contenant l'adresse IPv4 convertie.
 *
 * @throws Appelle Error::ErrorServ() si la conversion échoue ou si
 *         getaddrinfo ne parvient pas à résoudre l'adresse IP.
 *
 * @note Utilise getaddrinfo() pour la résolution et libère les ressources
 *       allouées avec freeaddrinfo() avant de retourner le résultat.
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
        Error::ErrorServ(3, gai_strerror(status));	

    struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
    outAddr = ipv4->sin_addr;

    freeaddrinfo(res);
    return outAddr;
}

/**
 * @brief Convertit un entier en chaîne de caractères.
 *
 * Cette fonction insère la valeur entière dans un flux de type stringstream
 * puis retourne la chaîne résultante.
 *
 * @param i Entier à convertir en chaîne.
 * @return std::string Représentation textuelle de l'entier fourni.
 *
 * @note Utilise std::stringstream pour la conversion, garantissant un
 *       comportement cohérent avec les flux standards C++.
 */
std::string Utils::IntToString(int i)
{
    std::stringstream ss;

    ss << i;

    return ss.str();
}

/**
 * @brief Vérifie si un socket est prêt à l'écriture sans blocage.
 *
 * Cette fonction utilise `select()` avec un timeout nul pour déterminer si le
 * descripteur de socket spécifié peut être écrit immédiatement, c’est-à-dire
 * sans bloquer le programme.
 *
 * @param sock Descripteur du socket à vérifier.
 * @return `true` si le socket est prêt à l’écriture, sinon `false`.
 *
 * @note Cette fonction est utile pour éviter les blocages lors de l’envoi de
 *       données sur des sockets non-bloquants.
 */
bool Utils::IsSocketWritable(int sock)
{
    fd_set writefds;
    struct timeval tv = {0, 0};
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    int ret = select(sock + 1, NULL, &writefds, NULL, &tv);
    return (ret > 0 && FD_ISSET(sock, &writefds));
}