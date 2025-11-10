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
 * @brief Affiche plusieurs sauts de ligne dans la console.
 * 
 * Cette fonction affiche un nombre spécifié de lignes vides pour aérer
 * ou formater la sortie dans le terminal.
 * 
 * @param nb Le nombre de sauts de ligne à afficher.
 */
void lBreak(int nb)
{
	for (int i = 0; i < nb; ++i)
		std::cout << std::endl;
}

/**
 * @brief Affiche un séparateur visuel avec un texte au centre.
 * 
 * Cette fonction affiche deux lignes vides, un encadré de tirets avec
 * le texte fourni au centre, puis de nouveau deux lignes vides. 
 * Utile pour organiser visuellement la sortie dans le terminal.
 * 
 * @param str Le texte à afficher au centre du séparateur.
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
 * @brief Gestionnaire de signal pour une fermeture propre du bot.
 * 
 * Cette fonction intercepte les signaux (comme SIGINT ou SIGTERM) et
 * effectue une fermeture ordonnée du bot en appelant la méthode `shutSign()`
 * sur l'objet global `BotPtr`. Elle empêche également la récursivité
 * si le signal est reçu plusieurs fois rapidement.
 * 
 * @param signum Numéro du signal reçu.
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

/**
 * @brief Point d'entrée du programme du bot IRC.
 * 
 * Cette fonction principale vérifie les arguments de la ligne de commande,
 * configure les gestionnaires de signaux pour la fermeture propre et
 * crée une instance de la classe `Bot` avec les paramètres fournis.
 * 
 * Arguments attendus :
 * - argv[1] : Adresse IP du serveur IRC.
 * - argv[2] : Port du serveur IRC.
 * - argv[3] : Mot de passe du serveur IRC.
 * - argv[4] : Canal IRC sur lequel se connecter.
 * 
 * @param argc Nombre d'arguments.
 * @param argv Tableau des arguments.
 * @return int Code de retour : 0 si succès, 1 sinon.
 */
int main(int argc, char** argv)
{
	if (argc != 5)
	{
		std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
		if (std::string(LANG) == "FR")
			std::cout << CYAN << "Utilisation: ./botirc [ip] [port] [password] [channel]" << RESET << std::endl;
		else
			std::cout << CYAN << "Usage: ./botirc [ip] [port] [password] [channel]" << RESET << std::endl;
		std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
		return 1;
	}
	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, SigHandler);
    
    try
	{
		Bot Bot(argv[1], argv[2], argv[3], argv[4]);
		BotPtr = &Bot;
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << "Fatal error: " << e.what() << RESET << std::endl;
		return 1;
	}

    return 0;
}