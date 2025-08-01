/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.hpp"

/**
 * @brief Affiche plusieurs lignes vides dans la console.
 *
 * @param nb Nombre de lignes vides a afficher.
 */
void lBreak(int nb)
{
	for (int i = 0; i < nb; ++i)
		std::cout << std::endl;
}

/**
 * @brief Affiche un separateur formate avec un titre centre.
 *
 * @param str Texte a afficher au centre du separateur.
 */
void separator(std::string str)
{
	lBreak(2);
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
	std::cout << YELLOW << str << RESET << std::endl;
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
	lBreak(2);
}

int main(int argc, char** argv)
{
	(void) argv;
	
	if (argc != 3)
	{
		std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
		if (std::string(LANG) == "FR")
			std::cout << CYAN << "Utilisation: ./ircserv [port] [password]" << RESET << std::endl;
		else
			std::cout << CYAN << "Usage: ./ircserv [port] [password]" << RESET << std::endl;
		std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
		return 1;
	}


    std::cout << "Hello World!\n";

    return 0;
}

