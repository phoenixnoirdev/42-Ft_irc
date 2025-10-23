/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"

//=================
//Canonical Form
//=================

/**
 * @brief Constructeur par défaut de la classe User.
 *
 * Initialise tous les attributs avec des valeurs neutres par défaut :
 * - `_uuid`       : chaîne vide
 * - `_name`       : chaîne vide
 * - `_nick`       : chaîne vide
 * - `_pass`       : chaîne vide
 * - `_recvBuffer` : chaîne vide
 * - `_grade`      : 0
 * - `_socket`     : 0
 * - `_idChan`     : 0
 */
User::User(): _uuid(""), _name(""), _nick(""), _pass(""), _recvBuffer(""), _grade(0), _socket(0), _idChan() {}

/**
 * @brief Constructeur de la classe User avec un socket.
 *
 * Initialise un utilisateur avec un descripteur de socket spécifique.
 * Les autres champs sont initialisés avec des valeurs neutres :
 * - `_uuid`       : chaîne vide
 * - `_name`       : chaîne vide
 * - `_nick`       : chaîne vide
 * - `_pass`       : chaîne vide
 * - `_recvBuffer` : chaîne vide
 * - `_grade`      : 0
 * - `_idChan`     : 0
 * - `_auth`       : false
 *
 * @param socket Descripteur de socket associé à l'utilisateur.
 */
User::User(int socket): _uuid(""), _name(""), _nick(""), _pass(""), _recvBuffer(""), _grade(0), _idChan()
{
	this->_socket = socket;
	this->_auth = false;
}

/**
 * @brief Constructeur de copie de la classe User.
 *
 * Initialise un nouvel utilisateur en copiant toutes les
 * informations d'un autre utilisateur existant.
 *
 * @param other Référence constante vers l'utilisateur à copier.
 */
User::User(const User& other)
{
	this->_uuid = other._uuid;
	this->_name = other._name;
	this->_nick = other._nick;
	this->_pass = other._pass;
	this->_recvBuffer = other._recvBuffer;
	this->_grade = other._grade;
	this->_socket = other._socket;
	this->_auth = other._auth;
}

/**
 * @brief Opérateur d'affectation de la classe User.
 *
 * Copie toutes les informations d'un autre utilisateur dans
 * l'utilisateur courant, sauf en cas d'auto-affectation.
 *
 * @param other Référence constante vers l'utilisateur à copier.
 * @return Référence vers l'utilisateur courant (*this).
 */
User&	User::operator=(const User& other)
{
	if (this != &other)
	{
		this->_uuid = other._uuid;
		this->_name = other._name;
		this->_nick = other._nick;
		this->_pass = other._pass;
		this->_recvBuffer = other._recvBuffer;
		this->_grade = other._grade;
		this->_socket = other._socket;
		this->_auth = other._auth;
	}
	return (*this);
}

/**
 * @brief Destructeur de la classe User.
 *
 * Comme la classe ne gère pas de ressources dynamiques,
 * le destructeur est trivial.
 */
User::~User() {}



//===============
//Fonctions public
//===============

/**
 * @brief Définit l'UUID de l'utilisateur avec la valeur donnée.
 *
 * @param str Nouvelle valeur de l'UUID.
 */
void User::setUuid(std::string str)
{
	this->_uuid = str;
}

/**
 * @brief Retourne l'UUID actuel de l'utilisateur.
 *
 * @return UUID de l'utilisateur.
 */
std::string User::getUuid() const
{
	return this->_uuid;
}

/**
 * @brief Définit le nom de l'utilisateur avec la valeur donnée.
 *
 * @param str Nouveau nom de l'utilisateur.
 */
void User::setName(std::string str)
{
	this->_name = str;
}

/**
 * @brief Retourne le nom actuel de l'utilisateur.
 *
 * @return Nom de l'utilisateur.
 */
std::string User::getName() const
{
	return this->_name;
}

/**
 * @brief Définit le pseudonyme de l'utilisateur avec la valeur donnée.
 *
 * @param str Nouveau pseudonyme de l'utilisateur.
 */
void User::setNick(std::string str)
{
	this->_nick = str;
}

/**
 * @brief Retourne le pseudonyme actuel de l'utilisateur.
 *
 * @return Pseudonyme de l'utilisateur.
 */
std::string User::getNick() const
{
	return this->_nick;
}

/**
 * @brief Définit le mot de passe de l'utilisateur.
 *
 * @param str Nouveau mot de passe.
 */
void User::setPass(std::string str)
{
	this->_pass = str;
}

/**
 * @brief Retourne le mot de passe actuel de l'utilisateur.
 *
 * @return Mot de passe de l'utilisateur.
 */
std::string User::getPass() const
{
	return this->_pass;
}


/**
 * @brief Définit le contenu du buffer de réception de l'utilisateur.
 *
 * @param str Nouveau contenu du buffer.
 */
void User::setRcvBuff(std::string str)
{
	this->_recvBuffer = str;
}

/**
 * @brief Retourne le contenu actuel du buffer de réception.
 *
 * @return Contenu du buffer de réception.
 */
const std::string User::getRcvBuff() const
{
	return this->_recvBuffer;
}

/**
 * @brief Retourne une référence au buffer de réception de l'utilisateur.
 *
 * @return Référence du buffer de réception.
 */
std::string& User::getRcvBuff()
{
	return this->_recvBuffer;
}      

/**
 * @brief Définit le grade (niveau) de l'utilisateur.
 *
 * @param i Nouveau grade de l'utilisateur.
 */
void User::setGrade(int i)
{
	this->_grade = i;
}

/**
 * @brief Retourne le grade (niveau) actuel de l'utilisateur.
 *
 * @return Grade de l'utilisateur.
 */
int User::getGrade() const
{
	return this->_grade;
}


/**
 * @brief Définit le descripteur de socket de l'utilisateur.
 *
 * @param i Nouveau descripteur de socket.
 */
void User::setSocket(int i)
{
	this->_socket = i;
}

/**
 * @brief Retourne le descripteur de socket de l'utilisateur.
 *
 * @return Descripteur de socket.
 */
int User::getSocket() const
{
	return this->_socket;
}


// Ajoute un chan a la liste
void User::addIdChan(int id)
{
	this->_idChan.insert(id);
}

// Retire un chan de la liste
void User::removeIdChan(int id)
{
	this->_idChan.erase(id);
}

// Retourn la liste des id
std::set<int>  User::getLstIdChan()
{
	return _idChan;
}

// Retourn si le chan est dans la liste
bool User::getIdChan(int id)
{
	return _idChan.find(id) != _idChan.end();
}


/**
 * @brief Définit le statut d'authentification de l'utilisateur.
 *
 * @param e Nouveau statut (true si authentifié).
 */
void User::setAuth(bool e)
{
	this->_auth = e;
}

/**
 * @brief Retourne le statut d'authentification de l'utilisateur.
 *
 * @return true si l'utilisateur est authentifié, false sinon.
 */
bool User::getAuth() const
{
	return this->_auth;
}

/**
 * @brief Ferme le descripteur de socket de l'utilisateur.
 */
void User::closeSocket()
{
	close(this->_socket);
}