/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
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
 * @brief Affiche un message d'erreur serveur formaté et termine le programme.
 * 
 * Cette fonction prend un code d'erreur et une chaîne descriptive,
 * affiche le message correspondant avec un style coloré, puis quitte le programme.
 * 
 * @param err Code d'erreur indiquant le type d'erreur survenue.
 * @param str Chaîne descriptive supplémentaire pour le message d'erreur.
 */
void Error::ErrorServ(int err, std::string str)
{
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;

    if (err == 1)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": port conversion fail." << str << RESET << std::endl; 
    else if (err == 2)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": Port out of range." << str << RESET << std::endl; 
    else if (err == 3)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": Getaddrinfo error: " << str << RESET << std::endl;
    else if (err == 4)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": Can't create a socket!" << str << RESET << std::endl; 
    else if (err == 5)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": Setsockopt failed." << str << RESET << std::endl;   
    else if (err == 6)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": Can't bind IP/port." << str << RESET << std::endl; 
    else if (err == 7)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": Can't Listen." << str << RESET << std::endl;
    else if (err == 8)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": Problem with client connecting." << str << RESET << std::endl;  
    else if (err == 9)
        std::cerr << RED << "Error " << Error::GetValErr(err) << ": Data NULL: " << str << RESET << std::endl;
    else
        std::cerr << RED << "Error " << Error::GetValErr(-1) << ": A mystical error has occurred, we must dig or pray to the god of code for the answer." << RESET << std::endl;
   
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;

    std::exit(EXIT_FAILURE);
}


//===============
//Fonctions private
//===============

/**
 * @brief Retourne une chaîne formatée contenant le code d'erreur en couleur.
 *
 * Concatène le code d'erreur converti en chaîne avec les codes ANSI pour
 * colorer le texte en vert (pour le code) puis en rouge (pour la suite).
 *
 * @param err Code d'erreur à formater.
 * @return std::string Chaîne formatée avec coloration ANSI.
 */
std::string Error::GetValErr(int err)
{
    return std::string(GREEN) + Utils::IntToString(err) + RED;
}