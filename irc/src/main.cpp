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

#include "../inc/inc.hpp"

Server* ServPtr = NULL;


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
 * la méthode Shutdown() via le pointeur global ServPtr si celui-ci est
 * non nul. Termine ensuite le programme avec le code du signal reçu.
 *
 * @param signum Numéro du signal reçu (ex: SIGINT, SIGTERM).
 */
void SigHandler(int signum)
{
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
	std::cout << YELLOW << "\n[Signal " << GREEN << signum << YELLOW << "] Fermeture propre du serveur..." << RESET << std::endl;
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;

	if (ServPtr != NULL)
		ServPtr->Shutdown();

	std::exit(signum);
}

int main(int argc, char** argv)
{
	//Teste bdd user
	if (argc == 1)
	{
		User	userDB = User();

		std::cout << (userDB.empty() ? "Is empty" : "Is not empty") << std::endl;
		std::cout << userDB.hasPass("Skombadi") << std::endl;
		userDB.addUser("Name1", "Nickname1", "123456", 5);
		userDB.addUser("Name2", "Nickname2", "Pipipoupou", 127);
		userDB.addUser("Name3", "Nickname34", "Skibidi", 2);
		userDB.addUser("Name4", "Nickname567", "Skombadi", 29);
		std::cout << (userDB.empty() ? "Is empty" : "Is not empty") << std::endl;
		std::cout << "Size: " << userDB.getSize() << std::endl;
		std::cout << userDB.hasUUID("Hello") << std::endl;
		std::cout << userDB.getUserByPass("Pipipoupou")->grade << std::endl;
		std::cout << userDB.getUserByIndex(2)->name << std::endl;
		userDB.removeUserByUUID(userDB.getUserByIndex(2)->uuid);
		std::cout << userDB.getUserByIndex(2)->name << std::endl;
		std::cout << userDB.hasPass("Skombadi") << std::endl;
		std::cout << userDB.hasPass("Skombady") << std::endl;
		std::cout << userDB.getUserByNick("Nickname567")->uuid << std::endl;
		userDB.removeUserByUUID(userDB.getTop()->uuid);
		userDB.removeUserByUUID(userDB.getTop()->uuid);
		userDB.removeUserByUUID(userDB.getTop()->uuid);
		std::cout << (userDB.empty() ? "Is empty" : "Is not empty") << std::endl;

		return (0);
	}

	//Retourn l'usage 
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

	
	// Ignore SIGPIPE pour éviter crash sur socket fermé
	signal(SIGPIPE, SIG_IGN);

	// Capture SIGINT (CTRL+C)
	signal(SIGINT, SigHandler);

	try
	{
		Server server(argv[1], argv[2]);
		ServPtr = &server;
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << "Fatal error: " << e.what() << RESET << std::endl;
		return 1;
	}
	

    return 0;
}

