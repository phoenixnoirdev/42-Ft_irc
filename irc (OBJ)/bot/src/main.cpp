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

# include "../inc/botinc.hpp"

Bot* BotPtr = NULL;

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

/**
 * @brief Gère les signaux pour effectuer un arrêt propre du serveur.
 *
 * Affiche un message indiquant qu'un signal a été reçu, puis appelle
 * la méthode Shutdown() via le pointeur global BotPtr si celui-ci est
 * non nul. Termine ensuite le programme avec le code du signal reçu.
 *
 * @param signum Numéro du signal reçu (ex: SIGINT, SIGTERM).
 */
void SigHandler(int signum)
{
    static bool handling = false;
    
	if (handling)
		return;

    handling = true;


	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
	std::cout << YELLOW << "\n[Signal " << GREEN << signum << YELLOW << "] Fermeture propre du Bot..." << RESET << std::endl;
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;

	if (BotPtr != NULL)
		BotPtr->shutSign();
}

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
		if (std::string(LANG) == "FR")
			std::cout << CYAN << "Utilisation: ./botirc [ip] [port] [password]" << RESET << std::endl;
		else
			std::cout << CYAN << "Usage: ./botirc [ip] [port] [password]" << RESET << std::endl;
		std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
		return 1;
	}

	
	signal(SIGPIPE, SIG_IGN);

	signal(SIGINT, SigHandler);
    
	
    try
	{
		Bot Bot(argv[1], argv[2], argv[3]);
		BotPtr = &Bot;
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << "Fatal error: " << e.what() << RESET << std::endl;
		return 1;
	}

    return 0;
}

