/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42luxembourg>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-09-29 16:03:37 by kelevequ          #+#    #+#             */
/*   Updated: 2025-09-29 16:03:37 by kelevequ         ###   ########.lu       */
/*                                                                            */
/* ************************************************************************** */

#include "file.hpp"

/**
 * @brief Constructeur par défaut de la classe File.
 *
 * Initialise tous les pointeurs membres à NULL et la variable entière
 * _global à 0.
 *
 * Membres initialisés :
 * - _fileName : nom du fichier
 * - _fileDir  : répertoire du fichier
 * - _channel  : canal associé au fichier
 * - _sender   : utilisateur ayant envoyé le fichier
 * - _captor   : utilisateur captant le fichier
 * - _global   : indicateur global (initialisé à 0)
 */
File::File(): _fileName(NULL), _fileDir(NULL), _channel(NULL), _sender(NULL), _captor(NULL), _global(0) {}

/**
 * @brief Constructeur de copie de la classe File.
 *
 * Crée un nouvel objet File en copiant les membres d'un autre objet File.
 *
 * @param other Référence constante vers l'objet File à copier.
 *
 * Membres copiés :
 * - _fileName : nom du fichier
 * - _fileDir  : répertoire du fichier
 * - _channel  : canal associé au fichier
 * - _sender   : utilisateur ayant envoyé le fichier
 * - _captor   : utilisateur captant le fichier
 * - _global   : indicateur global
 */
File::File(const File& other): _fileName(other._fileName), _fileDir(other._fileDir), _channel(other._channel), _sender(other._sender), _captor(other._captor), _global(other._global) {}

File&	File::operator=(const File& other)
{
	if (this != &other)
	{
		_fileName = other.getFileName();
		_fileDir = other.getFileDir();
		_channel = other.getChannel();
		_sender = other.getSender();
		_captor = other.getCaptor();
		_global = other.getGlobal();
	}
	return (*this);
}

/**
 * @brief Destructeur de la classe File.
 *
 * Libère les ressources éventuellement allouées.
 * Actuellement, la classe ne gère pas de mémoire dynamique,
 * donc le destructeur est vide.
 */
File::~File() {}

/**
 * @brief Constructeur paramétré de la classe File.
 *
 * Initialise un fichier avec son nom, son répertoire, l'expéditeur et le capteur.
 * Le canal est initialisé à NULL et le flag global à 0 par défaut.
 *
 * @param fileName Nom du fichier.
 * @param fileDir Répertoire du fichier.
 * @param sender Nom de l'expéditeur.
 * @param captor Nom du capteur.
 */
File::File(	std::string fileName,
			std::string fileDir,
			std::string sender,
			std::string captor):
			_fileName(fileName),
			_fileDir(fileDir),
			_channel(NULL),
			_sender(sender),
			_captor(captor),
			_global(0) {}

/**
 * @brief Récupère le nom du fichier.
 *
 * @return std::string Le nom du fichier (_fileName).
 */
std::string	File::getFileName() const
{
	return (_fileName);
}

/**
 * @brief Récupère le répertoire du fichier.
 *
 * @return std::string Le chemin du répertoire du fichier (_fileDir).
 */
std::string	File::getFileDir() const
{
	return (_fileDir);
}

/**
 * @brief Récupère le nom du canal associé au fichier.
 *
 * @return std::string Le nom du canal (_channel).
 */
std::string	File::getChannel() const
{
	return (_channel);
}

/**
 * @brief Récupère le nom de l'expéditeur du fichier.
 *
 * @return std::string Le nom de l'expéditeur (_sender).
 */
std::string	File::getSender() const
{
	return (_sender);
}

/**
 * @brief Récupère le nom du capteur ou destinataire associé au fichier.
 *
 * @return std::string Le nom du capteur (_captor).
 */
std::string	File::getCaptor() const
{
	return (_captor);
}

/**
 * @brief Indique si le fichier est global ou non.
 *
 * @return true si le fichier est global, false sinon.
 */
bool	File::getGlobal() const
{
	return (_global);
}

/**
 * @brief Définit le nom du fichier.
 *
 * @param fileName Le nouveau nom du fichier.
 */
void		File::setFileName(std::string fileName)
{
	_fileName = fileName;
}

/**
 * @brief Définit le chemin ou le répertoire du fichier.
 *
 * @param fileDir Le nouveau chemin ou répertoire du fichier.
 */
void		File::setFileDir(std::string fileDir)
{
	_fileDir = fileDir;
}

/**
 * @brief Définit le canal associé au fichier.
 * 
 * @param channel Le nom du canal à associer au fichier.
 */
void		File::setChannel(std::string channel)
{
	_channel = channel;
}

/**
 * @brief Définit l'expéditeur du fichier.
 * 
 * @param sender Le nom de l'expéditeur.
 */
void		File::setSender(std::string sender)
{
	_sender = sender;
}

/**
 * @brief Définit le capteur associé au fichier.
 * 
 * @param captor Le nom du capteur.
 */
void		File::setCaptor(std::string captor)
{
	_captor = captor;
}

/**
 * @brief Définit le statut global du fichier.
 * 
 * @param global Booléen indiquant si le fichier est global (true) ou non (false).
 */
void		File::setGlobal(bool global)
{
	_global = global;
}
