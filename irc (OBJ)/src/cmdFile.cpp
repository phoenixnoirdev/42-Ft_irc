/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdFile.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:29:50 by kelevequ          #+#    #+#             */
/*   Updated: 2025/10/31 12:32:06 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include "../inc/inc.hpp"

/*void	Server::handleFileList(int clientSocket, User& user, const std::string& line)
{
	std::map<std::string, File>::iterator	it;
	std::string								msg;
	File									tmp;

	msg = ":" + this->_ServName + " NOTICE " + user.getNick() + ;
	for (it = _Files.begin(); it != _Files.end(); it++)
	{
		tmp = it->second;
	}
}*/

/**
 * @brief Envoie un message à un utilisateur pour l'informer qu'un fichier lui est envoyé.
 *
 * Cette fonction construit un message IRC de type PRIVMSG indiquant que l'utilisateur
 * `user` tente d'envoyer un fichier nommé `filename` à l'utilisateur `target`.
 *
 * @param target Pointeur vers l'utilisateur destinataire du message.
 * @param user Référence vers l'utilisateur qui envoie le fichier.
 * @param filename Nom du fichier que l'utilisateur tente d'envoyer.
 */
void	Server::_msgUser(User *target, User& user, std::string filename)
{
	std::string	ircMsg;

	ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + target->getNick() + " : " + user.getNick() + " is trying to send you a file called \"" + filename + "\"\n";
	send(target->getSocket(), ircMsg.c_str(), ircMsg.size(), 0);
}

/**
 * @brief Gère la commande FSEND pour envoyer un fichier à un utilisateur, un canal ou globalement.
 *
 * La fonction analyse la ligne de commande pour extraire une option éventuelle, la cible
 * (utilisateur ou canal) et le chemin du fichier. Elle valide la cible, vérifie l'existence
 * du fichier et stocke les métadonnées du fichier dans la map des fichiers du serveur.
 * Enfin, elle notifie la ou les cibles de l'arrivée du fichier.
 *
 * Options supportées :
 *  - 'p' : envoyer à un utilisateur privé
 *  - 'c' : envoyer à un canal spécifique
 *  - 'g' : envoyer globalement à tous les utilisateurs
 *
 * @param clientSocket Le socket de l'utilisateur qui émet la commande FSEND.
 * @param line La ligne brute de la commande FSEND contenant l'option, la cible et le chemin du fichier.
 */
void	Server::handleFileSend(User& user, const std::string& line)
{
	std::string	tmp = line.substr(6);
	char 		opt = 'd';
	File		file;

	size_t	i = 0;

	while (i < tmp.size() && tmp[i] == ' ')
		i++;

	if (tmp[i] == '-')
	{
		i++;

		if ((i < tmp.size() && tmp[i] == ' ') || (i + 1 < tmp.size() && tmp[i + 1] == ' '))
		{
			std::cout << "FSEND: Missing or multiple options" << std::endl;
			return ;
		}
		else if (tmp[i] != 'p' || tmp[i] != 'c' || tmp[i] != 'g')
		{
			std::cout << "FSEND: Invalid option" << std::endl;
			return ;
		}
		else
			opt = tmp[i];


		while (i < tmp.size() && tmp[i] == ' ')
			i++;
	}


	User	*target = NULL;
	Channel	*chan = NULL;
	switch(opt)
	{
		case 'g':
			file.setGlobal(1);
			break ;
		case 'c':
		{
			size_t	k = 0;
			while (i + k < tmp.size() && tmp[i + k] != ' ')
				k++;
			if (k == 0)
			{
				std::cout << "FSEND: Missing target channel" << std::endl;
				return ;
			}
			std::string	targetChan = tmp.substr(i, k);

			for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
			{
				if (targetChan.compare(it->second.GetName()) == 0)
				{
					chan = &it->second;
					break ;
				}
			}
			if (chan == NULL)
			{
				std::cout << "FSEND: Target channel not found" << std::endl;
				return ;
			}

			file.setChannel(targetChan);

			i += k;
			while (i < tmp.size() && tmp[i] == ' ')
				i++;
			break ;
		}
		default:
		{
			size_t	k = 0;
			while (i + k < tmp.size() && tmp[i + k] != ' ')
				k++;
			if (k == 0)
			{
				std::cout << "FSEND: Missing target user" << std::endl;
				return ;
			}
			std::string	targetNick = tmp.substr(i, k);


			for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end(); it++)
			{
				if (targetNick.compare(it->second.getNick()) == 0)
				{
					target = &it->second;
					break ;
				}
			}

			if (target == NULL)
			{
				std::cout << "FSEND: Target user not found" << std::endl;
				return ;
			}

			
			file.setCaptor(targetNick);

		
			i += k;
			while (i < tmp.size() && tmp[i] == ' ')
				i++;
		}
	}
	tmp = tmp.substr(i);

	i = 0;
	while (i < tmp.size() && tmp[i] != ' ')
		i++;

	if (i == 0)
	{
		std::cout << "FSEND: Missing target file" << std::endl;
		return ;
	}

	std::string	targetFile = tmp.substr(0, i);

	while (i < tmp.size() && tmp[i] == ' ')
		i++;

	if (i != tmp.size())
	{
		std::cout << "FSEND: Too many arguments" << std::endl;
		return ;
	}


	std::fstream	ifs(targetFile.c_str(), std::fstream::in);
	if (ifs.fail())
	{
		std::cout << "FSEND: Invalid path" << std::endl;
		return ;
	}
	
	std::string filename = targetFile.substr(targetFile.find_last_of('/') + 1);


	file.setFileName(filename);
	file.setFileDir(targetFile);
	file.setSender(user.getNick());

	if (this->_Files.find(filename) != this->_Files.end())
	{
		std::cout << "FSEND: File already exists" << std::endl;
		return ;
	}

	this->_Files.insert(std::pair<std::string, File>(filename, file));

	_msgUser(target, user, filename);
}

/**
 * @brief Permet à un utilisateur de récupérer un fichier depuis le serveur.
 *
 * Cette fonction traite la commande de téléchargement d'un fichier (`FGET`) envoyé
 * par un autre utilisateur ou mis à disposition globalement. Elle vérifie :
 * - La présence du nom de fichier dans la commande.
 * - L'existence du fichier sur le serveur.
 * - Les droits d'accès en fonction de l'utilisateur, du canal ou du caractère global du fichier.
 *
 * Si toutes les conditions sont remplies, le fichier est copié dans le répertoire
 * cible de l'utilisateur. Si le fichier n'est pas global et destiné à un canal,
 * l'entrée correspondante dans `_Files` est supprimée après le téléchargement.
 *
 * @param user Référence vers l'utilisateur qui souhaite récupérer le fichier.
 * @param line La ligne de commande complète reçue contenant le nom du fichier et éventuellement le répertoire cible.
 */
void	Server::handleFileGet(User& user, const std::string& line)
{
	std::string	tmp = line.substr(5);
	
	size_t	i = 0;
	while (i < tmp.size() && tmp[i] != ' ')
		i++;
	
	size_t	k = 0;
	while (i + k < tmp.size() && tmp[i + k] == ' ')
		k++;
	
	if (i == 0)
	{
		std::cout << "FGET: Missing target file" << std::endl;
		return ;
	}

	std::string	targetFile = tmp.substr(0, i);


	i += k;
	size_t	j = 0;
	while (i + j < tmp.size() && tmp[i + j] != ' ')
		j++;
	k = i;
	while (j + k < tmp.size() && tmp[j + k] == ' ')
		k++;

	std::string	targetDir = (k == 0) ? "./" : tmp.substr(i, k) + "/";


	if (this->_Files.find(targetFile) == this->_Files.end())
	{
		std::cout << "FGET: Couldn't find target file" << std::endl;
		return ;
	}

	File	file(this->_Files.find(targetFile)->second);

	if (file.getGlobal())
		;
	else if (file.getChannel().empty())
	{
		Channel* channel = findChannelByName(file.getChannel(), this->_Chan);
		if (channel == NULL)
		{
			std::cout << "FGET: Channel no longer exists" << std::endl;
			this->_Files.erase(file.getFileName());
			return ;
		}
		if (channel->GetGradeUser(user) == -1)
		{
			std::cout << "FGET: User is not in the target channel" << std::endl;
			return ;
		}
	}
	else if (file.getCaptor() != user.getNick())
	{
		std::cout << "FGET: File is not meant for " << user.getNick() << std::endl;
		return ;
	}

	std::string		full_path(targetDir + targetFile);
	std::fstream	ofs(full_path.c_str(), std::fstream::out);
	std::fstream	ifs(file.getFileDir().c_str(), std::fstream::in);

	if (ofs.is_open())
		ofs << ifs.rdbuf();

	if (ofs && !file.getGlobal() && file.getChannel().empty())
	{
		this->_Files.erase(file.getFileName());
		std::cout << "FGET: Succesfull, deleting _Files entry" << std::endl;
	}
	else if (ofs)
	{
		std::cout << "FGET: Succesfull" << std::endl;
	}
	else
	{
		std::cout << "FGET: Error while outputting file" << std::endl;
		return ;
	}
}

/**
 * @brief Affiche la liste des fichiers actuellement enregistrés sur le serveur.
 *
 * Cette fonction parcourt la map `_Files` contenant tous les fichiers gérés
 * par le serveur et affiche le nom de chaque fichier dans la console.
 */
void	Server::handleFileList()
{
	for (std::map<std::string, File>::iterator it = this->_Files.begin(); it != this->_Files.end(); it++)
	{
		std::cout << it->second.getFileName() << std::endl;
	}
}
