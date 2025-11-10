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
 * Initialise tous les membres de l'utilisateur à des valeurs par défaut,
 * incluant UUID, nom, pseudonyme, mot de passe, buffer et grade.
 */
User::User(): _uuid(""), _name(""), _nick(""), _pass(""), _recvBuffer(""), _grade(0), _socket(0), _idChan() {}

/**
 * @brief Constructeur de la classe User avec initialisation du socket.
 *
 * Crée un nouvel utilisateur avec un descripteur de socket donné.
 * Initialise les autres membres à des valeurs par défaut.
 *
 * @param socket Descripteur du socket associé à l'utilisateur.
 */
User::User(int socket): _uuid(""), _name(""), _nick(""), _pass(""), _recvBuffer(""), _grade(0), _idChan()
{
	this->_socket = socket;
	this->_auth = false;
}

/**
 * @brief Constructeur de copie de la classe User.
 *
 * Initialise un nouvel objet User en copiant tous les membres
 * privés depuis l'objet fourni.
 *
 * @param other Objet User à copier.
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
 * @brief Surcharge de l'opérateur d'affectation pour User.
 *
 * Copie tous les membres privés de l'objet fourni dans l'objet
 * courant, en évitant l'auto-affectation.
 *
 * @param other Objet User à copier.
 * @return User& Référence à l'objet courant après copie.
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
 * Libère les ressources associées à l'utilisateur lors de sa
 * destruction. Actuellement, aucune opération spécifique n'est
 * effectuée dans ce destructeur.
 */
User::~User() {}



//===============
//Fonctions public
//===============

/**
 * @brief Définit l'identifiant unique (UUID) de l'utilisateur.
 *
 * Met à jour le membre privé _uuid avec la chaîne fournie,
 * garantissant une identification unique de l'utilisateur.
 *
 * @param str Nouvelle valeur de l'UUID à assigner.
 */
void User::setUuid(std::string str)
{
	this->_uuid = str;
}

/**
 * @brief Retourne l'identifiant unique (UUID) de l'utilisateur.
 *
 * Fournit la valeur du membre privé _uuid, servant à identifier
 * de manière unique l'utilisateur sur le serveur.
 *
 * @return std::string UUID de l'utilisateur.
 */
std::string User::getUuid() const
{
	return this->_uuid;
}

/**
 * @brief Définit le nom complet de l'utilisateur.
 *
 * Met à jour le membre privé _name avec la chaîne fournie,
 * représentant le nom officiel ou complet de l'utilisateur.
 *
 * @param str Nouveau nom complet à assigner.
 */
void User::setName(std::string str)
{
	this->_name = str;
}

/**
 * @brief Retourne le nom complet de l'utilisateur.
 *
 * Fournit la valeur du membre privé _name, représentant le nom
 * officiel ou complet de l'utilisateur.
 *
 * @return std::string Nom complet de l'utilisateur.
 */
std::string User::getName() const
{
	return this->_name;
}

/**
 * @brief Définit le pseudonyme (nickname) de l'utilisateur.
 *
 * Met à jour le membre privé _nick avec la chaîne fournie,
 * modifiant ainsi le nom affiché ou utilisé sur le serveur.
 *
 * @param str Nouvelle valeur du pseudonyme à assigner.
 */
void User::setNick(std::string str)
{
	this->_nick = str;
}

/**
 * @brief Retourne le pseudonyme (nickname) de l'utilisateur.
 *
 * Fournit le nom affiché ou utilisé pour identifier l'utilisateur
 * sur le serveur.
 *
 * @return std::string Pseudonyme actuel de l'utilisateur.
 */
std::string User::getNick() const
{
	return this->_nick;
}

/**
 * @brief Définit le mot de passe de l'utilisateur.
 *
 * Met à jour le membre privé _pass avec la chaîne fournie,
 * utilisée pour l'authentification sur le serveur.
 *
 * @param str Nouveau mot de passe de l'utilisateur.
 */
void User::setPass(std::string str)
{
	this->_pass = str;
}

/**
 * @brief Retourne le mot de passe de l'utilisateur.
 *
 * Fournit le mot de passe associé à l'utilisateur pour
 * l'authentification sur le serveur.
 *
 * @return std::string Mot de passe de l'utilisateur.
 */
std::string User::getPass() const
{
	return this->_pass;
}


/**
 * @brief Met à jour le buffer de réception de l'utilisateur.
 *
 * Remplace le contenu du buffer _recvBuffer par la chaîne fournie,
 * représentant les nouvelles données reçues.
 *
 * @param str Chaîne à stocker dans le buffer de réception.
 */
void User::setRcvBuff(std::string str)
{
	this->_recvBuffer = str;
}

/**
 * @brief Retourne une copie du buffer de réception de l'utilisateur.
 *
 * Fournit le contenu actuel du buffer de réception (_recvBuffer)
 * sans permettre sa modification.
 *
 * @return const std::string Copie du buffer de réception.
 */
const std::string User::getRcvBuff() const
{
	return this->_recvBuffer;
}

/**
 * @brief Fournit l'accès au buffer de réception de l'utilisateur.
 *
 * Retourne une référence vers la chaîne stockant les données reçues
 * depuis le socket de l'utilisateur.
 *
 * @return std::string& Référence au buffer de réception (_recvBuffer).
 */
std::string& User::getRcvBuff()
{
	return this->_recvBuffer;
}      

/**
 * @brief Définit le grade de l'utilisateur.
 *
 * Met à jour le membre privé _grade pour refléter le niveau ou
 * rôle attribué à l'utilisateur sur le serveur.
 *
 * @param i Valeur entière représentant le grade à assigner.
 */
void User::setGrade(int i)
{
	this->_grade = i;
}

/**
 * @brief Retourne le grade de l'utilisateur.
 *
 * Fournit la valeur entière correspondant au niveau ou rôle
 * de l'utilisateur dans le serveur.
 *
 * @return int Grade actuel de l'utilisateur.
 */
int User::getGrade() const
{
	return this->_grade;
}


/**
 * @brief Définit le descripteur du socket de l'utilisateur.
 *
 * Met à jour le membre privé _socket avec le descripteur fourni,
 * utilisé pour la communication réseau de l'utilisateur.
 *
 * @param i Descripteur de socket à assigner à l'utilisateur.
 */
void User::setSocket(int i)
{
	this->_socket = i;
}

/**
 * @brief Retourne le descripteur du socket de l'utilisateur.
 *
 * Fournit l'identifiant du socket utilisé par l'utilisateur pour
 * les communications réseau.
 *
 * @return int Descripteur du socket de l'utilisateur.
 */
int User::getSocket() const
{
	return this->_socket;
}


/**
 * @brief Ajoute un canal à la liste de l'utilisateur.
 *
 * Insère l'identifiant du canal spécifié dans l'ensemble _idChan,
 * indiquant que l'utilisateur fait désormais partie de ce canal.
 *
 * @param id Identifiant du canal à ajouter.
 */
void User::addIdChan(int id)
{
	this->_idChan.insert(id);
}

/**
 * @brief Supprime un canal de la liste de l'utilisateur.
 *
 * Retire l'identifiant du canal spécifié de l'ensemble _idChan,
 * indiquant que l'utilisateur n'en fait plus partie.
 *
 * @param id Identifiant du canal à supprimer.
 */
void User::removeIdChan(int id)
{
	this->_idChan.erase(id);
}

/**
 * @brief Retourne la liste des identifiants des canaux de l'utilisateur.
 *
 * Fournit une copie de l'ensemble contenant tous les IDs de canaux
 * auxquels l'utilisateur est actuellement abonné.
 *
 * @return std::set<int> Ensemble des identifiants de canaux.
 */
std::set<int>  User::getLstIdChan()
{
	return _idChan;
}

/**
 * @brief Vérifie si l'utilisateur appartient à un canal donné.
 *
 * Recherche l'identifiant du canal dans la collection interne _idChan
 * pour déterminer si l'utilisateur en fait partie.
 *
 * @param id Identifiant numérique du canal à vérifier.
 * @return true si l'utilisateur est présent dans le canal, false sinon.
 */
bool User::getIdChan(int id)
{
	return _idChan.find(id) != _idChan.end();
}


/**
 * @brief Définit l'état d'authentification de l'utilisateur.
 *
 * Met à jour la valeur du membre privé _auth selon le paramètre fourni,
 * indiquant si l'utilisateur est authentifié ou non.
 *
 * @param e Valeur booléenne représentant l'état d'authentification.
 *        true pour authentifié, false sinon.
 */
void User::setAuth(bool e)
{
	this->_auth = e;
}

/**
 * @brief Indique si l'utilisateur est authentifié.
 *
 * Retourne l'état d'authentification de l'utilisateur, déterminé
 * par la valeur du membre privé _auth.
 *
 * @return true si l'utilisateur est authentifié, false sinon.
 */
bool User::getAuth() const
{
	return this->_auth;
}

/**
 * @brief Ferme le socket associé à l'utilisateur.
 *
 * Cette fonction ferme proprement la connexion réseau de l'utilisateur
 * en appelant la fonction système close() sur le descripteur stocké.
 *
 * @note Après cet appel, le descripteur _socket ne doit plus être utilisé.
 */
void User::closeSocket()
{
	close(this->_socket);
}