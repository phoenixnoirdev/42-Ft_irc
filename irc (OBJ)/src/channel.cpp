/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 08:36:42 by kelevequ          #+#    #+#             */
/*   Updated: 2025/10/02 17:42:06 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/inc.hpp"

//=================
//Canonical Form
//=================

/**
 * @brief Constructeur par défaut de la classe Channel.
 *
 * Initialise l'ID, le nom et tous les modes à leurs valeurs par défaut.
 */
Channel::Channel(): _id(0), _name(""), _user_limit(0), _topic("") 
{
	_modes.invite_only = false;
	_modes.topic_op_only = false;
	_modes.has_key = false;
	_modes.moderated = false;
	_modes.has_limit = false;
}

Channel::Channel(int id, std::string name): _user_limit(0), _topic("Canal criado - Bem-vindos!") 
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
 * - Initialise le grade de visibilité à 0 par défaut.
 *
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



void Channel::SetMode(const User& user)
{
	std::string ircMsg = "";
	std::string msg = "";

	if (this->_modes.moderated == false)
	{
		this->_modes.moderated = true;

		ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost MODE #" + GetName() + " +m\r\n";
		::send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);

		msg = "Le channel vient de passer en mode: Moderer";
	}
	else
	{
		this->_modes.moderated = false;

		ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost MODE #" + GetName() + " -m\r\n";
		::send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);

		msg = "Le channel vient de passer en mode: Normal";
	}


	std::string ircMsgUser = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG #" + GetName() + " :" + msg + "\r\n";
	BroadcastAll(ircMsgUser);
}


void Channel::GetMode(const User& user)
{
	std::string ircMsg = "";

	if (this->_modes.moderated == false)
	{
		std::string msg = "Le channel est en mode: Normal";
		ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG #" + GetName() + " :" + msg + "\r\n";

		::send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);
	}
	else
	{
		std::string msg = "Le channel est en mode: Moderer";
		ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG #" + GetName() + " :" + msg + "\r\n";

		::send(user.getSocket(), ircMsg.c_str(), ircMsg.size(), 0);
	}
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
    // Verificar por socket (mais confiável) e depois por nick
    for (std::map<int, User>::iterator it = this->_grade_0.begin(); it != this->_grade_0.end(); it++)
    {
        if (user.getSocket() == it->second.getSocket() || 
            user.getNick().compare(it->second.getNick()) == 0)
            return 0;
    }

	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); it++)
    {
        if (user.getSocket() == it->second.getSocket() || 
            user.getNick().compare(it->second.getNick()) == 0)
            return 1;
    }

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); it++)
    {
        if (user.getSocket() == it->second.getSocket() || 
            user.getNick().compare(it->second.getNick()) == 0)
            return 2;
    }

	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); it++)
    {
        if (user.getSocket() == it->second.getSocket() || 
            user.getNick().compare(it->second.getNick()) == 0)
            return 3;
    }

	return -1;
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
	for (size_t i = 0; i < this->_ban.size(); ++i)
	{
		if (this->_ban[i].getName().compare(user.getName()) == 0)
			return true;
	}

	return false;
}

std::map<int, User> Channel::GetBanMap()
{
	return this->_ban;
}

int Channel::GetPop()
{
	int pop = 0;

	pop += this->_grade_0.size();
	pop += this->_grade_1.size();
	pop += this->_grade_2.size();
	pop += this->_grade_3.size();

	return pop;
}

//=================
//Novos métodos para modos IRC
//=================

//Modo +i/-i (Invite-only)
void Channel::setInviteOnly(bool enabled)
{
	_modes.invite_only = enabled;
}

bool Channel::isInviteOnly() const
{
	return _modes.invite_only;
}

void Channel::addInvite(const std::string& nick)
{
	_invite_list.insert(nick);
}

void Channel::removeInvite(const std::string& nick)
{
	_invite_list.erase(nick);
}

bool Channel::isInvited(const std::string& nick) const
{
	return _invite_list.find(nick) != _invite_list.end();
}

//Modo +t/-t (Topic restriction)
void Channel::setTopicOpOnly(bool enabled)
{
	_modes.topic_op_only = enabled;
}

bool Channel::isTopicOpOnly() const
{
	return _modes.topic_op_only;
}

bool Channel::canChangeTopic(const User& user) const
{
	if (!_modes.topic_op_only) return true;
	int grade = const_cast<Channel*>(this)->GetGradeUser(user);
	return (grade <= 1); // Grade 0 ou 1 = operador
}

void Channel::setTopic(const std::string& topic)
{
	_topic = topic;
}

std::string Channel::getTopic() const
{
	return _topic;
}

//Modo +k/-k (Channel key)
void Channel::setKey(const std::string& key)
{
	_modes.has_key = !key.empty();
	_key = key;
}

void Channel::removeKey()
{
	_modes.has_key = false;
	_key.clear();
}

bool Channel::hasKey() const
{
	return _modes.has_key;
}

bool Channel::checkKey(const std::string& provided_key) const
{
	if (!_modes.has_key) return true;
	return (_key == provided_key);
}

std::string Channel::getKey() const
{
	return _key;
}

//Modo +o/-o (Operator privilege)
void Channel::giveOp(const std::string& nick)
{
	// Procurar usuário em grade_3 (usuários normais) e mover para grade_1 (ops)
	for (std::map<int, User>::iterator it = _grade_3.begin(); it != _grade_3.end(); ++it)
	{
		if (it->second.getNick() == nick)
		{
			User user = it->second;
			// NÃO alterar grade administrativo do sistema - apenas mover para lista de ops do canal
			_grade_1.insert(std::make_pair(it->first, user));
			_grade_3.erase(it);
			break;
		}
	}
	
	// Também verificar em grade_2 (voice)
	for (std::map<int, User>::iterator it = _grade_2.begin(); it != _grade_2.end(); ++it)
	{
		if (it->second.getNick() == nick)
		{
			User user = it->second;
			// NÃO alterar grade administrativo do sistema - apenas mover para lista de ops do canal
			_grade_1.insert(std::make_pair(it->first, user));
			_grade_2.erase(it);
			break;
		}
	}
}

void Channel::removeOp(const std::string& nick)
{
	// Procurar usuário em grade_1 (ops) e mover para grade_3 (usuários normais)
	for (std::map<int, User>::iterator it = _grade_1.begin(); it != _grade_1.end(); ++it)
	{
		if (it->second.getNick() == nick)
		{
			User user = it->second;
			// NÃO alterar grade administrativo do sistema - apenas mover para lista de usuários do canal
			_grade_3.insert(std::make_pair(it->first, user));
			_grade_1.erase(it);
			break;
		}
	}
}

bool Channel::isOperator(const User& user) const
{
	int grade = const_cast<Channel*>(this)->GetGradeUser(user);
	return (grade <= 1); // Grade 0 ou 1 = operador
}

//Modo +l/-l (User limit)
void Channel::setUserLimit(int limit)
{
	_modes.has_limit = (limit > 0);
	_user_limit = limit;
}

void Channel::removeUserLimit()
{
	_modes.has_limit = false;
	_user_limit = 0;
}

bool Channel::hasUserLimit() const
{
	return _modes.has_limit;
}

int Channel::getUserLimit() const
{
	return _user_limit;
}

bool Channel::canJoin() const
{
	if (!_modes.has_limit) return true;
	return (const_cast<Channel*>(this)->GetPop() < _user_limit);
}

//Modo +m/-m (Moderated) - Atualizar o sistema existente
void Channel::setModerated(bool enabled)
{
	_modes.moderated = enabled;
}

bool Channel::isModerated() const
{
	return _modes.moderated;
}

bool Channel::canSpeak(const User& user) const
{
	if (!_modes.moderated) return true;
	int grade = const_cast<Channel*>(this)->GetGradeUser(user);
	return (grade <= 2); // Grade 0, 1 ou 2 = pode falar (ops e voice)
}

//Método geral para aplicar modos
bool Channel::applyMode(const User& user, const std::string& modestring, const std::string& param)
{
	if (!isOperator(user)) return false; // Apenas ops podem alterar modos
	
	if (modestring.length() < 2) return false;
	
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
	
	if (_modes.invite_only) modes += "i";
	if (_modes.topic_op_only) modes += "t";
	if (_modes.has_key) modes += "k";
	if (_modes.moderated) modes += "m";
	if (_modes.has_limit) modes += "l";
	
	if (modes == "+") modes = "";
	
	return modes;
}