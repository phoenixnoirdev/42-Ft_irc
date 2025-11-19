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

# include "../inc/inc.hpp"

Server* ServPtr = NULL;

/**
 * @brief Affiche un nombre spécifié de sauts de ligne dans la console.
 *
 * @param nb Nombre de lignes vides à afficher.
 *
 * @details
 * Cette fonction insère `nb` sauts de ligne successifs dans la sortie
 * standard (std::cout). Elle est principalement utilisée pour aérer
 * l'affichage dans la console, par exemple entre deux sections de logs
 * ou d'informations serveur.
 *
 * @note
 * Si `nb` est égal à zéro, aucune ligne vide n'est affichée.
 */
void lBreak(int nb)
{
	for (int i = 0; i < nb; ++i)
		std::cout << std::endl;
}

/**
 * @brief Affiche une chaîne de texte encadrée par des séparateurs visuels.
 *
 * @param str La chaîne de texte à afficher entre les lignes de séparation.
 *
 * @details
 * Cette fonction affiche deux sauts de ligne, une ligne de tirets, 
 * la chaîne `str`, une nouvelle ligne de tirets, puis deux sauts de 
 * ligne supplémentaires. Utile pour organiser visuellement les 
 * sections de la sortie console.
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
 * @brief Gestionnaire de signal pour la fermeture propre du serveur.
 *
 * @param signum Le numéro du signal reçu.
 *
 * @details
 * Cette fonction intercepte les signaux (ex. SIGINT, SIGTERM) et 
 * s'assure que le serveur effectue une fermeture propre, en évitant
 * les appels récursifs grâce à une variable statique `handling`.
 * Elle affiche un message d'information avant d'appeler `ShutSign()`
 * sur l'objet serveur pointé par `ServPtr`, si celui-ci est défini.
 */
void SigHandler(int signum)
{
    static bool handling = false;
    
	if (handling)
		return;

	handling = true;

	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
	std::cout << YELLOW << "\n[Signal " << GREEN << signum << YELLOW << "] Closing server properly..." << RESET << std::endl;
	std::cout << YELLOW << "------------------------------------" << RESET << std::endl;

	
    if (ServPtr != NULL)
        ServPtr->ShutSign();
}

/**
 * @brief Point d'entrée principal du serveur IRC.
 *
 * @param argc Nombre d'arguments en ligne de commande.
 * @param argv Tableau contenant les arguments : [port] [mot de passe].
 *
 * @return int Retourne 0 si succès, 1 si erreur.
 *
 * @details
 * Vérifie que le serveur reçoit exactement deux arguments. Sinon, 
 * affiche l'utilisation en français ou anglais selon la macro LANG.
 * Configure les gestionnaires de signaux : SIGPIPE est ignoré, 
 * SIGINT appelle SigHandler pour une fermeture propre.
 * Instancie ensuite l'objet Server avec le port et le mot de passe 
 * fournis, et met à jour le pointeur global ServPtr pour la gestion 
 * des signaux. Les exceptions sont capturées et affichées avant de 
 * retourner une erreur.
 */
int main(int argc, char** argv)
{ 
	if (argc != 3)
	{
		std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
		if (std::string(LANG) == "FR")
			std::cout << CYAN << "Usage: ./ircserv [port] [password]" << RESET << std::endl;
		else
			std::cout << CYAN << "Usage: ./ircserv [port] [password]" << RESET << std::endl;
		std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
		return 1;
	}

	signal(SIGPIPE, SIG_IGN);
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

