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

#include "../inc/inc.hpp"

//=================
//Canonical Form
//=================


User::User(): _uuid(""), _name(""), _nick(""), _pass(""), _recvBuffer(""), _grade(0), _socket(0), _idChan(0) {}

User::User(int socket): _uuid(""), _name(""), _nick(""), _pass(""), _recvBuffer(""), _grade(0), _idChan(0)
{

	this->_socket = socket;
	this->_auth = false;
}

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

User::~User() {}



//===============
//Fonctions public
//===============

void User::setUuid(std::string str)
{
	this->_uuid = str;
}

std::string User::getUuid() const
{
	return this->_uuid;
}


void User::setName(std::string str)
{
	this->_name = str;
}

std::string User::getName() const
{
	return this->_name;
}


void User::setNick(std::string str)
{
	this->_nick = str;
}

std::string User::getNick() const
{
	return this->_nick;
}


void User::setPass(std::string str)
{
	this->_pass = str;
}

std::string User::getPass() const
{
	return this->_pass;
}


void User::setRcvBuff(std::string str)
{
	this->_recvBuffer = str;
}

const std::string User::getRcvBuff() const
{
	return this->_recvBuffer;
}

std::string& User::getRcvBuff()
{
	return this->_recvBuffer;
}      


void User::setGrade(int i)
{
	this->_grade = i;
}

int User::getGrade() const
{
	return this->_grade;
}


void User::setSocket(int i)
{
	this->_socket = i;
}

int User::getSocket() const
{
	return this->_socket;
}


void User::setIdChan(int i)
{
	this->_idChan = i;
}

int User::getIdChan() const
{
	return this->_idChan;
}


void User::setAuth(bool e)
{
	this->_auth = e;
}

bool User::getAuth() const
{
	return this->_auth;
}


void User::closeSocket()
{
	close(this->_socket);
}
















/**
 * @brief Crée et ajoute un nouvel utilisateur à la liste.
 *
 * Génère un UUID pour l'utilisateur à partir de son nom et de son pseudo,
 * puis crée dynamiquement un objet t_user avec les informations fournies.
 * L'utilisateur est ajouté à la fin de la liste chaînée. Les pointeurs
 * _top et _bot ainsi que le compteur _size sont mis à jour.
 *
 * @param name_  Nom de l'utilisateur.
 * @param nick_  Pseudo (nick) de l'utilisateur.
 * @param pass_  Mot de passe de l'utilisateur.
 * @param grade_ Niveau ou grade de l'utilisateur.
 */
/*
void	User::addUser(
	const std::string& name_,
	const std::string& nick_,
	const std::string& pass_,
	int grade_,
	int socket_)
{
	t_user	*tmp = new t_user(generateUUID(name_, nick_), name_, nick_, pass_, grade_, socket_);

	if (empty())
		_top = tmp;
	else
		_bot->next = tmp;
	_bot = tmp;
	_size++;
}
9*/


/**
 * @brief Ajoute un nouvel utilisateur à la liste.
 *
 * Crée une copie dynamique de l'utilisateur fourni et l'ajoute
 * à la fin de la liste chaînée. Met à jour les pointeurs _top et _bot
 * ainsi que le compteur _size.
 *
 * @param other L'utilisateur à ajouter à la liste.
 */
/*
void	User::addUser(const t_user& other)
{
	t_user	*tmp = new t_user(other);

	if (empty())
		_top = tmp;
	else
		_bot->next = tmp;
	_bot = tmp;
	_size++;
}
*/


/**
 * @brief Supprime tous les utilisateurs de la liste.
 *
 * Cette fonction parcourt la liste chaînée des utilisateurs et
 * libère la mémoire de chaque élément. Après l'exécution, la
 * liste est vide, _top et _bot sont mis à NULL, et _size est 0.
 */
/*
void	User::clear()
{
	t_user	*tmp;

	if (empty())
		return ;
	while (_size > 0)
	{
		tmp = _top;
		_top = _top->next;
		delete tmp;
		_size--;
	}
	_bot = NULL;
}
*/

/**
 * @brief Supprime un utilisateur de la liste en fonction de son UUID.
 *
 * Cette fonction recherche l'utilisateur correspondant à l'UUID fourni
 * dans la liste chaînée. Si l'utilisateur est trouvé, il est retiré
 * de la liste et sa mémoire est libérée. La taille de la liste (_size)
 * est décrémentée.
 *
 * Cas particuliers gérés :
 * - L'utilisateur est en tête (_top) de la liste.
 * - L'utilisateur est en queue (_bot) de la liste.
 * - L'utilisateur est au milieu de la liste.
 *
 * @param uuid L'UUID de l'utilisateur à supprimer.
 * @return true si un utilisateur a été supprimé, false sinon.
 */
/*
bool	User::removeUserByUUID(const std::string& uuid)
{
	t_user	*tmp;
	t_user	*prev;
	size_t	i = 0;

	tmp = getUserByUUID(uuid, &i);
	if (!tmp)
		return (false);
	if (tmp == _top)
	{
		_top = tmp->next;
	}
	else if (tmp == _bot)
	{
		prev = getUserByIndex(i - 1);
		prev->next = NULL;
		_bot = prev;
	}
	else
	{
		prev = getUserByIndex(i - 1);
		prev->next = tmp->next;
	}
	delete tmp;
	_size--;
	return (true);
}
*/

/**
 * @brief Retourne le nombre total d'utilisateurs.
 *
 * Cette fonction renvoie la taille actuelle de la liste d'utilisateurs
 * contenue dans l'objet User.
 *
 * @return Nombre d'utilisateurs (_size).
 */
/*
size_t	User::getSize() const
{
	return (_size);
}
*/

/**
 * @brief Retourne le premier utilisateur de la liste.
 *
 * Cette fonction renvoie un pointeur vers la structure t_user
 * correspondant au premier utilisateur stocké dans l'objet User.
 *
 * @return Pointeur vers le premier utilisateur, ou NULL si la liste est vide.
 */
/*
t_user	*User::getTop() const
{
	return (_top);
}
*/

/**
 * @brief Retourne le bot associé à l'utilisateur.
 *
 * Cette fonction renvoie un pointeur vers la structure t_user
 * correspondant au bot géré par cette instance.
 *
 * @return Pointeur vers le bot, ou NULL si aucun bot n'est défini.
 */
/*
t_user	*User::getBot() const
{
	return (_bot);
}
*/

/**
 * @brief Récupère un utilisateur par son index dans la liste.
 *
 * Parcourt la liste chaînée des utilisateurs et retourne
 * le pointeur vers l'utilisateur situé à l'index spécifié.
 *
 * @param index Position de l'utilisateur dans la liste (0-based).
 * @return Pointeur vers l'utilisateur si l'index est valide,  
 *         `NULL` sinon.
 */
/*
t_user	*User::getUserByIndex(size_t index) const
{
	t_user	*tmp;
	size_t	i;

	if (index >= _size)
		return (NULL);
	tmp = getTop();
	if (index == 0)
		return (tmp);
	i = 1;
	while (i <= index)
	{
		tmp = tmp->next;
		i++;
	}
	return (tmp);
}
*/

/**
 * @brief Recherche un utilisateur par son UUID.
 *
 * Parcourt la liste chaînée des utilisateurs afin de trouver
 * le premier utilisateur dont le champ `uuid` correspond à celui fourni.
 *
 * @param uuid UUID de l'utilisateur recherché.
 * @return Pointeur vers l'utilisateur correspondant si trouvé,  
 *         `NULL` sinon.
 */
/*
t_user	*User::getUserByUUID(const std::string& uuid) const
{
	t_user	*tmp;
	size_t	i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (uuid == tmp->uuid)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}
*/

/**
 * @brief Recherche un utilisateur par son UUID.
 *
 * Parcourt la liste chaînée des utilisateurs afin de trouver
 * le premier dont le champ `uuid` correspond à celui fourni.
 * L'index courant dans la liste est passé via le pointeur `i`
 * et est incrémenté à chaque itération.
 *
 * @param uuid UUID de l'utilisateur recherché.
 * @param i Pointeur vers l'index de départ dans la liste.  
 *          La valeur est mise à jour au fur et à mesure de la recherche.
 * @return Pointeur vers l'utilisateur correspondant si trouvé,  
 *         `NULL` sinon.
 */
/*
t_user	*User::getUserByUUID(const std::string& uuid, size_t *i) const
{
	t_user	*tmp;

	tmp = getTop();
	while ((*i) < _size)
	{
		if (uuid == tmp->uuid)
			return (tmp);
		tmp = tmp->next;
		(*i)++;
	}
	return (NULL);
}
*/

/**
 * @brief Recherche un utilisateur par son nom.
 *
 * Parcourt la liste chaînée des utilisateurs afin de trouver
 * le premier dont le champ `name` correspond à celui fourni
 * en paramètre.
 *
 * @param name Nom de l'utilisateur recherché.
 * @return Pointeur vers l'utilisateur correspondant si trouvé,  
 *         `NULL` sinon.
 */
/*
t_user	*User::getUserByName(const std::string& name) const
{
	t_user	*tmp;
	size_t	i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (name == tmp->name)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}
*/

/**
 * @brief Recherche un utilisateur par son pseudonyme (nick).
 *
 * Parcourt la liste chaînée des utilisateurs afin de trouver
 * le premier dont le champ `nick` correspond à celui donné
 * en paramètre.
 *
 * @param nick Pseudonyme recherché.
 * @return Pointeur vers l'utilisateur correspondant si trouvé,  
 *         `NULL` sinon.
 */
/*
t_user	*User::getUserByNick(const std::string& nick) const
{
	t_user	*tmp;
	size_t	i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (nick == tmp->nick)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}
*/

/**
 * @brief Recherche un utilisateur par son mot de passe.
 *
 * Parcourt la liste chaînée des utilisateurs pour trouver
 * le premier dont le champ `pass` correspond à celui donné
 * en paramètre.
 *
 * @param pass Mot de passe recherché.
 * @return Pointeur vers l'utilisateur correspondant si trouvé,  
 *         `NULL` sinon.
 */
/*
t_user	*User::getUserByPass(const std::string& pass) const
{
	t_user	*tmp;
	size_t	i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (pass == tmp->pass)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}
*/

/**
 * @brief Vérifie si la liste des utilisateurs est vide.
 *
 * Cette fonction contrôle si le pointeur vers le premier
 * élément de la liste (`_top`) est nul.
 *
 * @return `true` si la liste ne contient aucun utilisateur,  
 *         `false` sinon.
 */
/*
bool	User::empty() const
{
	return (_top == NULL);
}
*/

/**
 * @brief Vérifie si un UUID existe dans la liste des utilisateurs.
 *
 * Cette fonction parcourt la liste chaînée interne des utilisateurs
 * et compare l'UUID fourni avec ceux enregistrés.  
 * Si une correspondance est trouvée, la fonction retourne `true`.
 *
 * @param uuid Identifiant unique de l'utilisateur à rechercher.
 *
 * @return `true` si l'UUID est trouvé,  
 *         `false` sinon ou si la liste est vide.
 *
 * @note La recherche est linéaire (O(n)) car elle parcourt la liste
 *       complète des utilisateurs.
 */
/*
bool	User::hasUUID(const std::string& uuid) const
{
	t_user	*tmp;
	size_t	i;

	if (empty())
		return (0);
	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (uuid == tmp->uuid)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}
*/

/**
 * @brief Vérifie si un nom existe dans la liste des utilisateurs.
 *
 * Cette fonction parcourt la liste chaînée interne des utilisateurs
 * et compare le nom fourni avec ceux enregistrés.  
 * Si une correspondance est trouvée, la fonction retourne `true`.
 *
 * @param name Nom de l'utilisateur à rechercher.
 *
 * @return `true` si le nom est trouvé,  
 *         `false` sinon ou si la liste est vide.
 *
 * @note La recherche est linéaire (O(n)) car elle parcourt la liste
 *       complète des utilisateurs.
 */
/*
bool	User::hasName(const std::string& name) const
{
	t_user	*tmp;
	size_t	i;

	if (empty())
		return (0);
	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (name == tmp->name)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}
*/

/**
 * @brief Vérifie si un pseudonyme existe dans la liste des utilisateurs.
 *
 * Cette fonction parcourt la liste chaînée interne des utilisateurs
 * et compare le pseudonyme fourni avec ceux enregistrés.  
 * Si une correspondance est trouvée, la fonction retourne `true`.
 *
 * @param nick Pseudonyme à rechercher.
 *
 * @return `true` si le pseudonyme est trouvé,  
 *         `false` sinon ou si la liste est vide.
 *
 * @note La recherche est linéaire (O(n)) car elle parcourt la liste
 *       complète des utilisateurs.
 */
/*
bool	User::hasNick(const std::string& nick) const
{
	t_user	*tmp;
	size_t	i;

	if (empty())
		return (0);
	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (nick == tmp->nick)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}
*/

/**
 * @brief Vérifie si un mot de passe existe dans la liste des utilisateurs.
 *
 * Cette fonction parcourt la liste chaînée interne des utilisateurs
 * et compare le mot de passe fourni avec ceux stockés.  
 * Si une correspondance est trouvée, la fonction retourne `true`.
 *
 * @param pass Mot de passe à rechercher.
 *
 * @return `true` si le mot de passe est trouvé,  
 *         `false` sinon ou si la liste est vide.
 *
 * @note La recherche est linéaire (O(n)) car elle parcourt la liste
 *       complète des utilisateurs.
 */
/*
bool	User::hasPass(const std::string& pass) const
{
	t_user	*tmp;
	size_t	i;

	if (empty())
		return (0);
	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (pass == tmp->pass)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}
*/

/*
void User::closeAllSockets()
{
	t_user	*tmp;
	size_t	i;

	if (empty())
		return ;
	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (tmp->socket != -1)
        {
            close(tmp->socket);
            tmp->socket = -1;
        }

		tmp = tmp->next;

		i++;
	}
}
*/

//===============
//Fonctions private
//===============

/**
 * @brief Calcule un hachage simple basé sur l’algorithme FNV-1a.
 *
 * Cette fonction applique un hachage non cryptographique sur une chaîne
 * de caractères, en utilisant la variante FNV-1a (Fowler–Noll–Vo).
 * Chaque caractère est combiné dans le hash via une opération XOR puis
 * multiplié par un nombre premier.
 *
 * @param s La chaîne d'entrée à hacher.
 *
 * @return La valeur de hachage 32 bits générée.
 *
 * @note Ce hachage est rapide et bien adapté aux comparaisons ou clés
 *       de table de hachage, mais n’est pas sûr pour des usages
 *       cryptographiques.
 */
static uint32_t	simpleHash(const std::string& s)
{
	uint32_t	hash = 2166136261u;
	for (std::string::size_type i = 0; i < s.size(); ++i)
	{
		hash ^= static_cast<unsigned char>(s[i]);
		hash *= 16777619u;
	}
	return (hash);
}

/**
 * @brief Génère un identifiant unique (UUID) basé sur le nom et le pseudo.
 *
 * Cette fonction combine le @p name et le @p nick pour produire
 * une chaîne unique, puis applique plusieurs variantes d'une fonction
 * de hachage simple avec des "salts".  
 * Les résultats sont ensuite formatés en hexadécimal afin de produire
 * une chaîne au format similaire à un UUID standard.
 *
 * @param name Le nom réel de l’utilisateur.
 * @param nick Le pseudonyme de l’utilisateur.
 *
 * @return Une chaîne de caractères représentant l’UUID généré.
 *
 * @note Le format généré n’est pas strictement conforme à la RFC 4122,
 *       mais garantit une bonne unicité au sein de l’application.
 */
std::string	User::generateUUID(const std::string& name, const std::string& nick)
{
	std::string	combined = name + "|" + nick;

	uint32_t	h1 = simpleHash(combined);
	uint32_t	h2 = simpleHash(combined + "salt1");
	uint32_t	h3 = simpleHash(combined + "salt2");
	uint32_t	h4 = simpleHash(combined + "salt3");

	std::ostringstream	oss;
	oss << std::hex << std::setfill('0')
		<< std::setw(8) << h1 << "-"
		<< std::setw(4) << (h2 & 0xFFFF) << "-"
		<< std::setw(4) << ((h2 >> 16) & 0xFFFF) << "-"
		<< std::setw(4) << (h3 & 0xFFFF) << "-"
		<< std::setw(4) << ((h3 >> 16) & 0xFFF)
		<< std::setw(8) << h4;

	return (oss.str());
}