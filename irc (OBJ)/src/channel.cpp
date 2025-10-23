/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 08:36:42 by kelevequ          #+#    #+#             */
/*   Updated: 2025/10/17 16:05:03 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"

//=================
//Canonical Form
//=================

/**
 * @brief Constructeur par défaut de la classe Channel.
 *
 * Initialise l'ID, le nom et le grade de visibilité à 0 ou vide.
 */
Channel::Channel(): _id(0), _name(""), _user_limit(0), _topic("") 
{
	_modes.invite_only = false;
	_modes.topic_op_only = false;
	_modes.has_key = false;
	_modes.moderated = false;
	_modes.has_limit = false;
}

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
 * @brief Constructeur de Channel avec ID et nom spécifiés.
 *
 * - Initialise l'ID et le nom du canal.
 * @param id Identifiant du canal.
 * @param name Nom du canal.
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
 * @brief Surcharge de l'opérateur d'affectation pour Channel.
 *
 * - Copie tous les attributs d'un autre objet Channel.
 * - Vérifie l'auto-affectation pour éviter les erreurs.
 *
 * @param other Objet Channel source à copier.
 * @return Référence sur l'objet courant après copie.
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
 * - Vide les listes de grades et de bannissements.
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
 * @param i Nouvel identifiant à attribuer.
 */
void Channel::SetId(int i)
{
	this->_id = i;
}

/**
 * @brief Retourne l'identifiant actuel du canal.
 *
 * @return ID du canal.
 */
int Channel::GetId() const
{
	return this->_id;
}


/**
 * @brief Définit le nom du canal.
 *
 * @param str Nouveau nom du canal.
 */
void Channel::SetName(const std::string& str)
{
	this->_name = str;
}

/**
 * @brief Retourne le nom actuel du canal.
 *
 * @return Nom du canal.
 */
std::string Channel::GetName() const
{
	return this->_name;
}


/**
 * @brief Ajoute un utilisateur au canal selon son grade.
 *
 * - Classe l'utilisateur dans la map correspondant à son grade (0 à 3).
 *
 * @param user Référence à l'utilisateur à ajouter.
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
 * @brief Diffuse un message de type JOIN à tous les utilisateurs du canal.
 *
 * - Parcourt toutes les maps de grades (0 à 3) et envoie le message à chaque utilisateur.
 *
 * @param msg Message à diffuser aux utilisateurs.
 */
void Channel::BroadcastAll(const std::string &msg)
{
	for (std::map<int, User>::iterator it = this->_grade_0.begin(); it != this->_grade_0.end(); ++it)
	{
		send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); ++it) 
	{
		send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); ++it) 
	{
		send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); ++it) 
	{
		send(it->first, msg.c_str(), msg.size(), 0);
	}
}

/**
 * @brief Diffuse un message à tous les utilisateurs du canal sauf l'expéditeur.
 *
 * - Parcourt toutes les maps de grades (0 à 3) et envoie le message à chaque utilisateur.
 * - Ignore le socket de l'utilisateur qui a envoyé le message.
 *
 * @param msg Message à diffuser.
 * @param sender Descripteur de socket de l'expéditeur à exclure.
 */
void Channel::Broadcast(const std::string &msg, int sender)
{
    for (std::map<int, User>::iterator it = this->_grade_0.begin(); it != this->_grade_0.end(); ++it)
	{
		if (it->second.getSocket() != sender)
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); ++it) 
	{
		if (it->second.getSocket() != sender)
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); ++it) 
	{
		if (it->second.getSocket() != sender)
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); ++it) 
	{
		if (it->second.getSocket() != sender)
        	send(it->first, msg.c_str(), msg.size(), 0);
	}
}

void Channel::RemoveUser(int sock) 
{
    this->_grade_0.erase(sock);
    this->_grade_1.erase(sock);
	this-> _grade_2.erase(sock);
    this->_grade_3.erase(sock);
}

void Channel::AddUserBan(const User& user)
{
	this->_ban.insert(std::make_pair(user.getSocket(), user));
}

void Channel::RemoveUserBan(int sock)
{
	this->_ban.erase(sock);
}

bool Channel::GetUserBan(const User& user)
{
	for (std::map<int, User>::iterator it = this->_ban.begin(); it != this->_ban.end(); ++it)
	{
		if (it->second.getName() == user.getName())
			return true;
	}
	return false;
}

std::map<int, User> Channel::GetBanMap()
{
	return this->_ban;
}



void Channel::SetOpChannel(User& user)
{
	_OpChannel = &user;
}

int Channel::GetOpChannel()
{
    if (_OpChannel)
        return _OpChannel->getSocket();
    return -1;
}

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
int Channel::GetNbUser()
{
	int pop = 0;

	pop += this->_grade_0.size();
	pop += this->_grade_1.size();
	pop += this->_grade_2.size();
	pop += this->_grade_3.size();

	return pop;
}

bool Channel::canJoin() const
{
	if (!this->_modes.has_limit) 
		return true;
	
	return (const_cast<Channel*>(this)->GetNbUser() < getUserLimit());
}

int Channel::getUserLimit() const
{
	return this->_user_limit;
}

void Channel::setUserLimit(int limit)
{
	this->_modes.has_limit = (limit > 0);
	this->_user_limit = limit;
}

void Channel::removeUserLimit()
{
	this->_modes.has_limit = false;
	this->_user_limit = 0;
}

bool Channel::hasUserLimit() const
{
	return this->_modes.has_limit;
}


// MODERATED (Modes +m/-m)
void Channel::setModerated(bool enabled)
{
	this->_modes.moderated = enabled;
}

bool Channel::isModerated() const
{
	return this->_modes.moderated;
}

bool Channel::canSpeak(const User& user) const
{
	if (!this->_modes.moderated) 
		return true;

	int grade = const_cast<Channel*>(this)->GetGradeUser(user);
	
	return (grade <= 2);
}


// TOPIC RESTRICTION (Modes +t/-t)
void Channel::SetTopic(std::string topic)
{
	this->_topic = topic;
}

std::string Channel::GetTopic()
{
	return this->_topic;
}

void Channel::setTopicOpOnly(bool enabled)
{
	this->_modes.topic_op_only = enabled;
}

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

bool Channel::isOperator(const User& user) const
{
	int grade = const_cast<Channel*>(this)->GetGradeUser(user);

	return (grade <= 1);
}


// INVITED-ONLY (Modes +i/-i)
void Channel::setInviteOnly(bool enabled)
{
	this->_modes.invite_only = enabled;
}

bool Channel::isInviteOnly() const
{
	return this->_modes.invite_only;
}

void Channel::addInvite(const std::string& nick)
{
	this->_invite_list.insert(nick);
}

void Channel::removeInvite(const std::string& nick)
{
	this->_invite_list.erase(nick);
}

bool Channel::isInvited(const std::string& nick) const
{
	return this->_invite_list.find(nick) != _invite_list.end();
}


//CHANNEL KEY (Modes +k/-k)
void Channel::setKey(const std::string& key)
{
	this->_modes.has_key = !key.empty();
	this->_key = key;
}

void Channel::removeKey()
{
	this->_modes.has_key = false;
	this->_key.clear();
}

bool Channel::hasKey() const
{
	return this->_modes.has_key;
}

bool Channel::checkKey(const std::string& provided_key) const
{
	if (!this->_modes.has_key) 
		return true;
	
	return (this->_key == provided_key);
}

std::string Channel::getKey() const
{
	return this->_key;
}


//Methode generale appliquer au modes
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