/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 08:36:42 by kelevequ          #+#    #+#             */
/*   Updated: 2025/10/31 14:02:10 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"

//=================
//Canonical Form
//=================

/**
 * @brief Constructeur par défaut de la classe Channel.
 *
 * Initialise un canal avec des valeurs par défaut :
 * - ID à 0
 * - Nom vide
 * - Limite d'utilisateurs à 0
 * - Sujet (topic) vide
 * - Tous les modes désactivés :
 *   - invite_only
 *   - topic_op_only
 *   - has_key
 *   - moderated
 *   - has_limit
 */
Channel::Channel(): _id(0), _name(""), _user_limit(0), _topic("") 
{
	_modes.invite_only = false;
	_modes.topic_op_only = false;
	_modes.has_key = false;
	_modes.moderated = false;
	_modes.has_limit = false;
}

/**
 * @brief Constructeur paramétré de la classe Channel.
 *
 * @param id L'identifiant unique du canal.
 * @param name Le nom du canal.
 *
 * Initialise les attributs du canal :
 * - _id avec l'identifiant fourni
 * - _name avec le nom fourni
 * - _user_limit à 0
 * - _topic vide
 * - _OpChannel à NULL
 * - Tous les modes (_modes) désactivés :
 *   - invite_only
 *   - topic_op_only
 *   - has_key
 *   - moderated
 *   - has_limit
 */
Channel::Channel(int id, std::string name): _user_limit(0), _topic(""), _OpChannel(NULL) 
{
	this->_id = id;
	this->_name = name;

	_modes.invite_only = false;
	_modes.topic_op_only = false;
	_modes.has_key = false;
	_modes.moderated = false;
	_modes.has_limit = false;
}

/**
 * @brief Constructeur de copie de la classe Channel.
 *
 * @param other Le canal à copier.
 *
 * Copie tous les attributs de l'autre canal, y compris :
 * - _id : identifiant du canal
 * - _name : nom du canal
 * - _modes : structure des modes du canal
 * - _key : mot de passe du canal
 * - _user_limit : limite d'utilisateurs
 * - _topic : sujet du canal
 * - _invite_list : liste des utilisateurs invités
 * - _grade_0 à _grade_3 : listes des utilisateurs selon leur grade
 * - _ban : liste des utilisateurs bannis
 */
Channel::Channel(const Channel& other)
{
	this->_id = other._id;
	this->_name = other._name;
	this->_modes = other._modes;
	this->_key = other._key;
	this->_user_limit = other._user_limit;
	this->_topic = other._topic;
	this->_invite_list = other._invite_list;
	this->_grade_0 = other._grade_0;
	this->_grade_1 = other._grade_1;
	this->_grade_2 = other._grade_2;
	this->_grade_3 = other._grade_3;
	this->_ban = other._ban;
}

/**
 * @brief Opérateur d'affectation pour la classe Channel.
 *
 * @param other Le canal à copier.
 * @return Channel& Référence au canal courant après copie.
 *
 * Vérifie l'auto-affectation, puis copie tous les attributs de l'autre canal, 
 * incluant :
 * - _id : identifiant du canal
 * - _name : nom du canal
 * - _modes : structure des modes du canal
 * - _key : mot de passe du canal
 * - _user_limit : limite d'utilisateurs
 * - _topic : sujet du canal
 * - _invite_list : liste des utilisateurs invités
 * - _grade_0 à _grade_3 : listes des utilisateurs selon leur grade
 * - _ban : liste des utilisateurs bannis
 */
Channel&	Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		this->_id = other._id;
		this->_name = other._name;
		this->_modes = other._modes;
		this->_key = other._key;
		this->_user_limit = other._user_limit;
		this->_topic = other._topic;
		this->_invite_list = other._invite_list;
		this->_grade_0 = other._grade_0;
		this->_grade_1 = other._grade_1;
		this->_grade_2 = other._grade_2;
		this->_grade_3 = other._grade_3;
		this->_ban = other._ban;
	}
	return (*this);
}

/**
 * @brief Destructeur de la classe Channel.
 *
 * Libère les ressources associées au canal en vidant toutes les listes 
 * d'utilisateurs et la liste des bannis :
 * - _grade_0 à _grade_3 : listes des utilisateurs selon leur grade
 * - _ban : liste des utilisateurs bannis
 */
Channel::~Channel()
{
	this->_grade_0.clear();
    this->_grade_1.clear();
    this->_grade_2.clear();
    this->_grade_3.clear();
    this->_ban.clear();
}


//=================
//Fonction public
//=================

/**
 * @brief Définit l'identifiant du canal.
 * 
 * @param i Nouvel identifiant à attribuer au canal.
 */
void Channel::SetId(int i)
{
	this->_id = i;
}

/**
 * @brief Retourne l'identifiant du canal.
 * 
 * @return int L'identifiant du canal.
 */
int Channel::GetId() const
{
	return this->_id;
}


/**
 * @brief Définit le nom du canal.
 * 
 * @param str Nouveau nom à attribuer au canal.
 */
void Channel::SetName(const std::string& str)
{
	this->_name = str;
}

/**
 * @brief Retourne le nom du canal.
 * 
 * @return Le nom du canal sous forme de chaîne de caractères.
 */
std::string Channel::GetName() const
{
	return this->_name;
}


/**
 * @brief Ajoute un utilisateur au canal selon son grade.
 * 
 * Les utilisateurs sont stockés dans différentes maps selon leur grade :
 * - grade 0 : utilisateurs normaux
 * - grade 1 : utilisateurs avec certains privilèges
 * - grade 2 : opérateurs du canal
 * - grade 3 : créateur ou administrateur du canal
 * 
 * @param user Référence constante vers l'utilisateur à ajouter.
 */
void Channel::AddUser(const User& user)
{
	if (user.getGrade() == 0)
		this->_grade_0.insert(std::make_pair(user.getSocket(), user));
	
	if (user.getGrade() == 1)
		this->_grade_1.insert(std::make_pair(user.getSocket(), user));
	
	if (user.getGrade() == 2)
		this->_grade_2.insert(std::make_pair(user.getSocket(), user));

	if (user.getGrade() == 3)
		this->_grade_3.insert(std::make_pair(user.getSocket(), user));
}

/**
 * @brief Ajoute le créateur du canal à la liste des utilisateurs de grade 0.
 * 
 * Cette fonction est appelée lors de la création d'un canal pour ajouter
 * l'utilisateur créateur dans la map des utilisateurs normaux (grade 0).
 * 
 * @param user Référence constante vers l'utilisateur créateur.
 */
void Channel::AddUserCreatChan(const User& user)
{
	this->_grade_0.insert(std::make_pair(user.getSocket(), user));
}

/**
 * @brief Retourne le grade d'un utilisateur dans le canal.
 * 
 * Cette fonction parcourt les différentes maps d'utilisateurs du canal
 * (_grade_0 à _grade_3) et renvoie le grade correspondant à l'utilisateur
 * passé en paramètre. Si l'utilisateur n'est pas présent dans le canal,
 * la fonction renvoie -1.
 * 
 * @param user Référence constante vers l'utilisateur à vérifier.
 * @return int Grade de l'utilisateur (0 à 3), ou -1 si l'utilisateur n'est pas trouvé.
 */
int Channel::GetGradeUser(const User& user)
{
    for (std::map<int, User>::iterator it = this->_grade_0.begin(); it != this->_grade_0.end(); it++)
    {
        if (user.getName().compare(it->second.getName()) == 0)
            return 0;
    }

	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); it++)
    {
        if (user.getName().compare(it->second.getName()) == 0)
            return 1;
    }

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); it++)
    {
        if (user.getName().compare(it->second.getName()) == 0)
            return 2;
    }

	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); it++)
    {
        if (user.getName().compare(it->second.getName()) == 0)
            return 3;
    }

	return -1;
}

/**
 * @brief Retourne la map des utilisateurs correspondant à un grade donné.
 * 
 * Cette fonction permet d'obtenir la liste des utilisateurs d'un grade
 * spécifique (0 à 3) dans le canal. Si le grade fourni n'est pas valide,
 * la fonction renvoie la liste des utilisateurs de grade 3 par défaut.
 * 
 * @param e Grade des utilisateurs à récupérer (0 à 3).
 * @return std::map<int, User> Map des utilisateurs du grade spécifié.
 */
std::map<int, User> Channel::GetLstGrade(int e) const
{
	if (e == 0)
		return this->_grade_0;

	if (e == 1)
		return this->_grade_1;

	if (e == 2)
		return this->_grade_2;

	if (e == 3)
		return this->_grade_3;
	
	return this->_grade_3;
}


/**
 * @brief Envoie un message à tous les utilisateurs du canal.
 * 
 * Cette fonction parcourt toutes les maps d'utilisateurs par grade
 * (de 0 à 3) et envoie le message spécifié à chaque utilisateur.
 * 
 * @param msg Référence constante vers le message à diffuser.
 */
void Channel::BroadcastAll(const std::string &msg)
{
	for (std::map<int, User>::iterator it = this->_grade_0.begin(); it != this->_grade_0.end(); ++it)
	{
		if (Utils::IsSocketWritable(it->second.getSocket()))
			send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); ++it) 
	{
		if (Utils::IsSocketWritable(it->second.getSocket()))
			send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); ++it) 
	{
		if (Utils::IsSocketWritable(it->second.getSocket()))
			send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); ++it) 
	{
		if (Utils::IsSocketWritable(it->second.getSocket()))
			send(it->first, msg.c_str(), msg.size(), 0);
	}
}

/**
 * @brief Envoie un message à tous les utilisateurs du canal sauf l'expéditeur.
 * 
 * Cette fonction parcourt toutes les maps d'utilisateurs par grade (0 à 3)
 * et envoie le message spécifié à chaque utilisateur dont le socket est
 * différent de celui de l'expéditeur.
 * 
 * @param msg Référence constante vers le message à diffuser.
 * @param sender Socket de l'utilisateur qui a envoyé le message.
 */
void Channel::Broadcast(const std::string &msg, int sender)
{
    for (std::map<int, User>::iterator it = this->_grade_0.begin(); it != this->_grade_0.end(); ++it)
	{
		if (it->second.getSocket() != sender && Utils::IsSocketWritable(it->second.getSocket()))
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); ++it) 
	{
		if (it->second.getSocket() != sender && Utils::IsSocketWritable(it->second.getSocket()))
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); ++it) 
	{
		if (it->second.getSocket() != sender && Utils::IsSocketWritable(it->second.getSocket()))
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); ++it) 
	{
		if (it->second.getSocket() != sender && Utils::IsSocketWritable(it->second.getSocket()))
        	send(it->first, msg.c_str(), msg.size(), 0);
	}
}

/**
 * @brief Supprime un utilisateur du canal en fonction de son socket.
 * 
 * Cette fonction retire l'utilisateur des quatre maps de grades (0 à 3),
 * garantissant qu'il n'est plus présent dans le canal quel que soit son grade.
 * 
 * @param sock Socket de l'utilisateur à supprimer.
 */
void Channel::RemoveUser(int sock) 
{
    this->_grade_0.erase(sock);
    this->_grade_1.erase(sock);
	this-> _grade_2.erase(sock);
    this->_grade_3.erase(sock);
}

/**
 * @brief Ajoute un utilisateur à la liste des utilisateurs bannis du canal.
 * 
 * L'utilisateur est ajouté à la map `_ban` avec sa socket comme clé.
 * 
 * @param user Référence constante vers l'utilisateur à bannir.
 */
void Channel::AddUserBan(const User& user)
{
	this->_ban.insert(std::make_pair(user.getSocket(), user));
}

/**
 * @brief Retire un utilisateur de la liste des utilisateurs bannis du canal.
 * 
 * L'utilisateur correspondant à la socket spécifiée est supprimé de la map `_ban`.
 * 
 * @param sock La socket de l'utilisateur à débannir.
 */
void Channel::RemoveUserBan(int sock)
{
	this->_ban.erase(sock);
}

/**
 * @brief Vérifie si un utilisateur est banni du canal.
 * 
 * Parcourt la liste `_ban` pour déterminer si l'utilisateur spécifié
 * est présent dans la liste des utilisateurs bannis.
 * 
 * @param user Référence constante vers l'utilisateur à vérifier.
 * @return true si l'utilisateur est banni, false sinon.
 */
bool Channel::GetUserBan(const User& user)
{
	for (std::map<int, User>::iterator it = this->_ban.begin(); it != this->_ban.end(); ++it)
	{
		if (it->second.getName() == user.getName())
			return true;
	}
	return false;
}

/**
 * @brief Retourne la liste des utilisateurs bannis du canal.
 * 
 * Cette fonction renvoie la map `_ban` contenant tous les utilisateurs
 * actuellement bannis, associant leur socket à leur objet `User`.
 * 
 * @return std::map<int, User> La map des utilisateurs bannis.
 */
std::map<int, User> Channel::GetBanMap()
{
	return this->_ban;
}

/**
 * @brief Définit l'opérateur du canal.
 * 
 * Cette fonction attribue un utilisateur comme opérateur principal du canal,
 * lui donnant les privilèges nécessaires pour gérer le canal (modérer, kick, ban, etc.).
 * 
 * @param user Référence vers l'utilisateur à définir comme opérateur.
 */
void Channel::SetOpChannel(User& user)
{
	_OpChannel = &user;
}

/**
 * @brief Récupère le socket de l'opérateur du canal.
 * 
 * Cette fonction retourne le socket de l'utilisateur actuellement désigné comme
 * opérateur du canal. Si aucun opérateur n'est défini, elle retourne -1.
 * 
 * @return int Socket de l'opérateur ou -1 si aucun opérateur.
 */
int Channel::GetOpChannel()
{
    if (_OpChannel)
        return _OpChannel->getSocket();
    return -1;
}

/**
 * @brief Vérifie si un utilisateur est l'opérateur du canal.
 * 
 * Cette fonction compare le socket de l'utilisateur donné avec celui de 
 * l'opérateur du canal. Si l'utilisateur est l'opérateur, elle retourne true,
 * sinon false.
 * 
 * @param user L'utilisateur à vérifier.
 * @return true Si l'utilisateur est l'opérateur du canal.
 * @return false Sinon.
 */
bool Channel::isOpChannel(User user)
{
	if (_OpChannel->getSocket() == user.getSocket())
		return true;
	
	return false;
}


//===============
//GESTION DES MODES IRC
//===============

// USERLIMIT (Modes +l/-l)

/**
 * @brief Retourne le nombre total d'utilisateurs dans le canal.
 * 
 * Cette fonction calcule la somme des utilisateurs de tous les grades (0 à 3)
 * présents dans le canal.
 * 
 * @return int Le nombre total d'utilisateurs.
 */
int Channel::GetNbUser()
{
	int pop = 0;

	pop += this->_grade_0.size();
	pop += this->_grade_1.size();
	pop += this->_grade_2.size();
	pop += this->_grade_3.size();

	return pop;
}

/**
 * @brief Vérifie si de nouveaux utilisateurs peuvent rejoindre le canal.
 * 
 * Si le mode de limitation d'utilisateurs n'est pas activé, la fonction
 * retourne toujours true. Sinon, elle compare le nombre actuel d'utilisateurs
 * avec la limite définie.
 * 
 * @return true si un utilisateur peut rejoindre, false sinon.
 */
bool Channel::canJoin() const
{
	if (!this->_modes.has_limit) 
		return true;
	
	return (const_cast<Channel*>(this)->GetNbUser() < getUserLimit());
}

/**
 * @brief Retourne la limite maximale d'utilisateurs autorisés dans le canal.
 * 
 * @return int Nombre maximal d'utilisateurs pouvant rejoindre le canal.
 */
int Channel::getUserLimit() const
{
	return this->_user_limit;
}

/**
 * @brief Définit la limite maximale d'utilisateurs pour le canal.
 * 
 * Active le mode +l si la limite est supérieure à 0.
 * 
 * @param limit Nombre maximal d'utilisateurs autorisés.
 */
void Channel::setUserLimit(int limit)
{
	this->_modes.has_limit = (limit > 0);
	this->_user_limit = limit;
}

/**
 * @brief Supprime la limite d'utilisateurs du canal.
 * 
 * Cette fonction désactive le mode +l et remet le nombre maximal
 * d'utilisateurs (_user_limit) à 0.
 */
void Channel::removeUserLimit()
{
	this->_modes.has_limit = false;
	this->_user_limit = 0;
}

/**
 * @brief Vérifie si le canal a une limite d'utilisateurs.
 * 
 * @return true si le canal a un nombre maximal d'utilisateurs défini.
 * @return false sinon.
 */
bool Channel::hasUserLimit() const
{
	return this->_modes.has_limit;
}



// MODERATED (Modes +m/-m)

/**
 * @brief Active ou désactive le mode "moderated" du canal.
 * 
 * Un canal modéré nécessite des permissions spéciales pour parler.
 * 
 * @param enabled true pour activer le mode modéré, false pour le désactiver.
 */
void Channel::setModerated(bool enabled)
{
	this->_modes.moderated = enabled;
}

/**
 * @brief Vérifie si le canal est en mode modéré.
 * 
 * @return true si le canal est modéré, false sinon.
 */
bool Channel::isModerated() const
{
	return this->_modes.moderated;
}

/**
 * @brief Vérifie si un utilisateur peut parler dans le canal.
 * 
 * Si le canal est en mode modéré, seuls les utilisateurs de grade 0, 1 ou 2
 * peuvent envoyer des messages.
 * 
 * @param user Référence constante vers l'utilisateur à vérifier.
 * @return true si l'utilisateur peut parler, false sinon.
 */
bool Channel::canSpeak(const User& user) const
{
	if (!this->_modes.moderated) 
		return true;

	int grade = const_cast<Channel*>(this)->GetGradeUser(user);
	
	return (grade <= 2);
}



// TOPIC RESTRICTION (Modes +t/-t)

/**
 * @brief Définit le sujet (topic) du canal.
 * 
 * Cette fonction met à jour le topic affiché aux utilisateurs du canal.
 * 
 * @param topic La nouvelle chaîne de caractères représentant le topic.
 */
void Channel::SetTopic(std::string topic)
{
	this->_topic = topic;
}

/**
 * @brief Retourne le sujet (topic) actuel du canal.
 * 
 * @return Une chaîne de caractères contenant le topic du canal.
 */
std::string Channel::GetTopic()
{
	return this->_topic;
}

/**
 * @brief Active ou désactive l'option qui restreint la modification du topic aux opérateurs.
 * 
 * @param enabled True pour que seuls les opérateurs puissent changer le topic, false sinon.
 */
void Channel::setTopicOpOnly(bool enabled)
{
	this->_modes.topic_op_only = enabled;
}

/**
 * @brief Vérifie si seul un opérateur peut modifier le topic du canal.
 * 
 * @return true si la modification du topic est réservée aux opérateurs, false sinon.
 */
bool Channel::isTopicOpOnly() const
{
	return this->_modes.topic_op_only;
}

bool Channel::canChangeTopic(const User& user) const
{
	if (!this->_modes.topic_op_only)
		return true;
	
	int grade = const_cast<Channel*>(this)->GetGradeUser(user);
	
	return (grade <= 1);
}


//Moderateur privileg (Modes +o/-o)

/**
 * @brief Vérifie si un utilisateur peut modifier le topic du canal.
 * 
 * Si le mode "topic_op_only" est activé, seuls les utilisateurs de grade 0 ou 1
 * (opérateurs) peuvent modifier le topic.
 * 
 * @param user Référence constante vers l'utilisateur à vérifier.
 * @return true si l'utilisateur peut changer le topic, false sinon.
 */
void Channel::giveOp(const std::string& nick)
{
	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); ++it)
	{
		if (it->second.getNick() == nick)
		{
			User user = it->second;

			this->_grade_1.insert(std::make_pair(it->first, user));
			this->_grade_3.erase(it);

			return;
		}
	}
	

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); ++it)
	{
		if (it->second.getNick() == nick)
		{
			User user = it->second;

			this->_grade_1.insert(std::make_pair(it->first, user));
			this->_grade_2.erase(it);

			return;
		}
	}
}

/**
 * @brief Retire le statut d'opérateur d'un utilisateur sur le canal.
 * 
 * Cette fonction déplace l'utilisateur de la map des grade 1 (opérateurs)
 * vers la map des grade 3 (utilisateurs normaux), en conservant ses informations.
 * 
 * @param nick Le pseudo de l'utilisateur dont le statut d'opérateur doit être retiré.
 */
void Channel::removeOp(const std::string& nick)
{
	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); ++it)
	{
		if (it->second.getNick() == nick)
		{
			User user = it->second;
			
			this->_grade_3.insert(std::make_pair(it->first, user));
			this->_grade_1.erase(it);

			return;
		}
	}
}

/**
 * @brief Vérifie si un utilisateur est opérateur sur le canal.
 * 
 * Un utilisateur est considéré comme opérateur si son grade est 0 ou 1.
 * 
 * @param user Référence constante vers l'utilisateur à vérifier.
 * @return true si l'utilisateur est opérateur, false sinon.
 */
bool Channel::isOperator(const User& user) const
{
	int grade = const_cast<Channel*>(this)->GetGradeUser(user);

	return (grade <= 1);
}



// INVITED-ONLY (Modes +i/-i)

/**
 * @brief Active ou désactive le mode "invite only" du canal.
 * 
 * Lorsqu'activé, seuls les utilisateurs invités peuvent rejoindre le canal.
 * 
 * @param enabled true pour activer le mode, false pour le désactiver.
 */
void Channel::setInviteOnly(bool enabled)
{
	this->_modes.invite_only = enabled;
}

/**
 * @brief Vérifie si le canal est en mode "invite only".
 * 
 * @return true si seuls les utilisateurs invités peuvent rejoindre le canal,
 *         false sinon.
 */
bool Channel::isInviteOnly() const
{
	return this->_modes.invite_only;
}

/**
 * @brief Ajoute un utilisateur à la liste des invitations du canal.
 * 
 * Cette fonction permet d'ajouter le pseudo d'un utilisateur à la liste
 * des utilisateurs autorisés à rejoindre un canal en mode "invite only".
 * 
 * @param nick Pseudo de l'utilisateur à inviter.
 */
void Channel::addInvite(const std::string& nick)
{
	this->_invite_list.insert(nick);
}

/**
 * @brief Retire un utilisateur de la liste des invitations du canal.
 * 
 * Cette fonction permet de supprimer le pseudo d'un utilisateur de la liste
 * des utilisateurs autorisés à rejoindre un canal en mode "invite only".
 * 
 * @param nick Pseudo de l'utilisateur à retirer de la liste d'invitation.
 */
void Channel::removeInvite(const std::string& nick)
{
	this->_invite_list.erase(nick);
}

/**
 * @brief Vérifie si un utilisateur est dans la liste d'invitation du canal.
 * 
 * Cette fonction teste si le pseudo fourni se trouve dans la liste des
 * utilisateurs autorisés à rejoindre un canal en mode "invite only".
 * 
 * @param nick Pseudo de l'utilisateur à vérifier.
 * @return true Si l'utilisateur est invité.
 * @return false Si l'utilisateur n'est pas invité.
 */
bool Channel::isInvited(const std::string& nick) const
{
	return this->_invite_list.find(nick) != _invite_list.end();
}



//CHANNEL KEY (Modes +k/-k)

/**
 * @brief Définit la clé (mot de passe) du canal.
 * 
 * Si la clé n'est pas vide, le canal devient protégé par mot de passe.
 * 
 * @param key Chaîne de caractères représentant la clé du canal.
 */
void Channel::setKey(const std::string& key)
{
	this->_modes.has_key = !key.empty();
	this->_key = key;
}

/**
 * @brief Supprime la clé (mot de passe) du canal.
 * 
 * Après l'appel de cette fonction, le canal n'est plus protégé par mot de passe.
 */
void Channel::removeKey()
{
	this->_modes.has_key = false;
	this->_key.clear();
}

/**
 * @brief Vérifie si le canal est protégé par une clé (mot de passe).
 * 
 * @return true si le canal a une clé, false sinon.
 */
bool Channel::hasKey() const
{
	return this->_modes.has_key;
}

/**
 * @brief Vérifie si la clé fournie correspond à celle du canal.
 * 
 * Si le canal n'a pas de clé, la vérification retourne toujours vrai.
 * 
 * @param provided_key La clé fournie par l'utilisateur.
 * @return true si la clé est correcte ou si le canal n'a pas de clé, false sinon.
 */
bool Channel::checkKey(const std::string& provided_key) const
{
	if (!this->_modes.has_key) 
		return true;
	
	return (this->_key == provided_key);
}

/**
 * @brief Récupère la clé actuelle du canal.
 * 
 * @return La clé du canal sous forme de chaîne de caractères.
 */
std::string Channel::getKey() const
{
	return this->_key;
}


/**
 * @brief Applique un mode sur le canal si l'utilisateur est autorisé.
 * 
 * Cette fonction permet à un opérateur du canal d'activer ou de désactiver
 * différents modes du canal tels que :
 * - 'i' : canal sur invitation seulement
 * - 't' : seuls les opérateurs peuvent changer le topic
 * - 'k' : définir ou supprimer une clé d'accès
 * - 'o' : donner ou retirer le statut d'opérateur à un utilisateur
 * - 'l' : définir ou supprimer la limite d'utilisateurs
 * - 'm' : activer ou désactiver le mode modéré
 * 
 * @param user L'utilisateur qui tente de modifier le mode.
 * @param modestring La chaîne représentant l'opération (+ ou -) et le mode.
 * @param param Paramètre optionnel pour certains modes (ex. clé ou limite).
 * @return true si le mode a été appliqué avec succès, false sinon.
 */
bool Channel::applyMode(const User& user, const std::string& modestring, const std::string& param)
{
	if (!isOpChannel(user) && !isOperator(user)) 
		return false;
	
	if (modestring.length() < 2) 
		return false;
	
	bool adding = (modestring[0] == '+');
	char mode = modestring[1];
	
	switch(mode)
	{
		case 'i':
			setInviteOnly(adding);
			return true;
		case 't':
			setTopicOpOnly(adding);
			return true;
		case 'k':
			if (adding && !param.empty())
				setKey(param);
			else if (!adding)
				removeKey();
			return true;
		case 'o':
			if (!param.empty())
			{
				if (adding)
					giveOp(param);
				else
					removeOp(param);
			}
			return true;
		case 'l':
			if (adding && !param.empty())
				setUserLimit(std::atoi(param.c_str()));
			else if (!adding)
				removeUserLimit();
			return true;
		case 'm':
			setModerated(adding);
			return true;
	}
	
	return false;
}

/**
 * @brief Retourne une représentation sous forme de chaîne des modes actifs du canal.
 * 
 * Cette fonction parcourt les différents modes du canal et construit une chaîne
 * commençant par '+' suivie des lettres correspondant aux modes activés :
 * - 'i' : canal sur invitation seulement
 * - 't' : seuls les opérateurs peuvent changer le topic
 * - 'k' : canal protégé par une clé
 * - 'm' : mode modéré
 * - 'l' : limite d'utilisateurs définie
 * 
 * Si aucun mode n'est actif, la chaîne retournée est vide.
 * 
 * @return std::string Chaîne représentant les modes actifs du canal.
 */
std::string Channel::getModeString() const
{
	std::string modes = "+";
	
	if (this->_modes.invite_only) modes += "i";
	if (this->_modes.topic_op_only) modes += "t";
	if (this->_modes.has_key) modes += "k";
	if (this->_modes.moderated) modes += "m";
	if (this->_modes.has_limit) modes += "l";
	
	if (modes == "+") modes = "";
	
	return modes;
}