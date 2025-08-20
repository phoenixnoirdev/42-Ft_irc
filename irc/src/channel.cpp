/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 08:36:42 by kelevequ          #+#    #+#             */
/*   Updated: 2025/08/20 08:09:08 by kelevequ         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/inc.hpp"

//=================
//Canonical Form
//=================

/**
 * @brief Constructeur par défaut de la classe Channel.
 *
 * Initialise un objet Channel vide, avec une liste chaînée vide (_top et _bot à NULL)
 * et une taille (_size) égale à 0.
 */
Channel::Channel(): _top(NULL), _bot(NULL), _size(0) {}

/**
 * @brief Constructeur de copie pour la classe Channel.
 *
 * Crée un nouvel objet Channel en dupliquant tous les channels
 * de l'objet source. Chaque channel est copié individuellement
 * et ajouté à la liste chaînée de l'objet courant.
 *
 * @param other L'objet Channel source à copier.
 */
Channel::Channel(const Channel& other): _top(NULL), _bot(NULL), _size(0)
{
	t_channel	*tmp;
	size_t		len;

	len = other.getSize();
	tmp = other.getTop();
	while (_size < len && tmp != NULL)
	{
		addChannel(*tmp);
		tmp = tmp->next;
		_size++;
	}
}

/**
 * @brief Opérateur d'affectation pour la classe Channel.
 *
 * Copie la liste chaînée de channels depuis un autre objet Channel.
 * Chaque channels est dupliqué et ajouté à la liste de l'objet courant.
 *
 * @param other L'objet Channel source à copier.
 * @return Référence à l'objet Channel courant après copie.
 */
Channel&	Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		t_channel	*tmp;
		size_t		len;

		len = other.getSize();
		tmp = other.getTop();
		_size = 0;
		while (_size < len && tmp != NULL)
		{
			addChannel(*tmp);
			tmp = tmp->next;
			_size++;
		}
	}
	return (*this);
}

/**
 * @brief Destructeur de la classe User.
 *
 * Libère toute la mémoire allouée pour la liste chaînée d'utilisateurs
 * en appelant la fonction clear().
 */
Channel::~Channel()
{
	clear();
}

//=================
//Constructeur
//=================

/**
 * @brief Constructeur de Channel à partir d'une liste chaînée existante.
 *
 * Initialise la liste de channels vide puis copie chaque channels
 * de la liste fournie dans la nouvelle instance.
 *
 * @param list Pointeur vers le premier élément d'une liste chaînée de t_channel.
 */
Channel::Channel(t_channel *list): _top(NULL), _bot(NULL), _size(0)
{
	t_channel	*tmp;

	tmp = list;
	while (tmp != NULL)
	{
		addChannel(*tmp);
		tmp = tmp->next;
		_size++;
	}
}

/**
 * @brief Constructeur de User à partir d'une chaîne JSON.
 *
 * Initialise la liste chaînée de channels vide (_top et _bot à NULL,
 * _size à 0). Actuellement, le paramètre json n'est pas utilisé.
 *
 * @param json Chaîne JSON supposée contenir des données de channels.
 */
Channel::Channel(const std::string& json): _top(NULL), _bot(NULL), _size(0)
{
	(void)json;
}

//===============
//Fonctions public
//===============

void	Channel::addChannel(
	int id,
	const std::string& name,
	int grade,
	t_user *owner)
{
	t_channel	*tmp = new t_channel(id, name, grade);

	if (empty())
		_top = tmp;
	else
		_bot->next = tmp;
	_bot = tmp;
	_size++;
	addUserChannel(tmp, owner, 0);
}

void	Channel::addChannel(const t_channel& other)
{
	t_channel	*tmp = new t_channel(other);

	if (empty())
		_top = tmp;
	else
		_bot->next = tmp;
	_bot = tmp;
	_size++;
}

bool	Channel::addUserChannel(t_channel *channel, t_user *user, int grade)
{
	if (user == NULL || channel == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	if (isChannelUserBan(channel, user))
	{
		std::cerr << user->name << " is banned from " << channel->name << std::endl;
		return (false);
	}
	if (channelHasUser(channel, user, 3))
	{
		std::cerr << user->name << " already exists in " << channel->name  << std::endl;
		return (false);
	}
	switch (grade)
	{
		case 0:
			channel->grade_0[user] = "owner";
		case 1:
			channel->grade_1[user] = "admin";
		case 2:
			channel->grade_2[user] = "user";
		case 3:
			channel->grade_3[user] = "default";
			break ;
		default:
			std::cerr << "Invalid grade: " << grade << std::endl;
			return (false);
	}
	return (true);
}

void	Channel::clear()
{
	t_channel	*tmp;

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

bool	Channel::removeChannelByID(int id)
{
	t_channel	*tmp;
	t_channel	*prev;
	size_t		i = 0;

	tmp = getChannelByID(id, &i);
	if (!tmp)
		return (false);
	if (tmp == _top)
	{
		_top = tmp->next;
	}
	else if (tmp == _bot)
	{
		prev = getChannelByIndex(i - 1);
		prev->next = NULL;
		_bot = prev;
	}
	else
	{
		prev = getChannelByIndex(i - 1);
		prev->next = tmp->next;
	}
	delete tmp;
	_size--;
	return (true);
}

bool	Channel::removeChannelByName(const std::string& name)
{
	t_channel	*tmp;
	t_channel	*prev;
	size_t		i = 0;

	tmp = getChannelByName(name, &i);
	if (!tmp)
		return (false);
	if (tmp == _top)
	{
		_top = tmp->next;
	}
	else if (tmp == _bot)
	{
		prev = getChannelByIndex(i - 1);
		prev->next = NULL;
		_bot = prev;
	}
	else
	{
		prev = getChannelByIndex(i - 1);
		prev->next = tmp->next;
	}
	delete tmp;
	_size--;
	return (true);
}

size_t	Channel::getSize() const
{
	return (_size);
}

t_channel	*Channel::getTop() const
{
	return (_top);
}

t_channel	*Channel::getBot() const
{
	return (_bot);
}

t_channel	*Channel::getChannelByID(int id) const
{
	t_channel	*tmp;
	size_t		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (id == tmp->id)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}

t_channel	*Channel::getChannelByID(int id, size_t *i) const
{
	t_channel	*tmp;

	tmp = getTop();
	while (*i < _size)
	{
		if (id == tmp->id)
			return (tmp);
		tmp = tmp->next;
		(*i)++;
	}
	return (NULL);
}

t_channel	*Channel::getChannelByIndex(size_t index) const
{
	t_channel	*tmp;
	size_t		i;

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

t_channel	*Channel::getChannelByName(const std::string& name) const
{
	t_channel	*tmp;
	size_t		i;

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

t_channel	*Channel::getChannelByName(const std::string& name, size_t *i) const
{
	t_channel	*tmp;

	tmp = getTop();
	while (*i < _size)
	{
		if (name == tmp->name)
			return (tmp);
		tmp = tmp->next;
		(*i)++;
	}
	return (NULL);
}

std::string&	Channel::getChannelUserDisplayName(t_channel *channel, t_user *user) const
{
	if (channel->grade_3[user] != "default")
		return (channel->grade_3[user]);
	else
		return (user->name);
}

bool	Channel::setChannelUserDisplayName(t_channel *channel, t_user *user, const std::string& displayName)
{
	if (channel == NULL || user == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	if (displayName == "admin" || displayName == "owner" || displayName == "user" || displayName == "default")
	{
		std::cerr << "Invalid Display Name" << std::endl;
		return (false);
	}
	channel->grade_3[user] = displayName;
	return (true);
}

bool	Channel::promoteChannelUser(t_channel *channel, t_user *user, int grade)
{
	if (channel == NULL || user == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	if (channelHasUser(channel, user, grade))
	{
		std::cerr << user->name << " already exists in " << channel->name << " as grade " << grade << std::endl;
		return (false);
	}
	switch (grade)
	{
		case 0:
			channel->grade_0[user] = "owner";
		case 1:
			channel->grade_1[user] = "admin";
		case 2:
			channel->grade_2[user] = "user";
			break ;
		default:
			std::cerr << "Invalid grade: " << grade << std::endl;
			return (false);
	}
	return (true);
}

bool	Channel::demoteChannelUser(t_channel *channel, t_user *user, int grade)
{
	if (channel == NULL || user == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	switch (grade)
	{
		case 3:
			channel->grade_2.erase(user);
		case 2:
			channel->grade_1.erase(user);
		case 1:
			channel->grade_0.erase(user);
			break ;
		default:
			std::cerr << "Invalid grade: " << grade << std::endl;
			return (false);
	}
	return (true);
}

bool	Channel::banChannelUser(t_channel *channel, t_user *user, const std::string& reason)
{
	if (channel == NULL || user == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	channel->grade_0.erase(user); //It's safe, even in c++98
	channel->grade_1.erase(user);
	channel->grade_2.erase(user);
	channel->grade_3.erase(user);
	channel->ban[user] = reason;
	return (true);
}

bool	Channel::empty() const
{
	return (_top == NULL);
}

bool	Channel::hasID(int id) const
{
	t_channel	*tmp;
	size_t		i;

	if (empty())
		return (0);
	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (id == tmp->id)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}

bool	Channel::hasName(const std::string& name) const
{
	t_channel	*tmp;
	size_t		i;

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

bool	Channel::channelHasUser(t_channel *channel, t_user *user, int grade) const
{
	if (grade > 3 || grade < 0)
		return (false);
	std::map<s_user*, std::string>::iterator	it;
	switch (grade)
	{
		case 0:
			it = channel->grade_0.find(user);
			return (it != channel->grade_0.end());
		case 1:
			it = channel->grade_1.find(user);
			return (it != channel->grade_1.end());
		case 2:
			it = channel->grade_2.find(user);
			return (it != channel->grade_2.end());
		case 3:
			it = channel->grade_3.find(user);
			return (it != channel->grade_3.end());
		default:
			std::cerr << "Invalid grade: " << grade << std::endl;
			return (false);
	}
}

bool	Channel::isChannelUserBan(t_channel *channel, t_user *user) const
{
	std::map<s_user*, std::string>::iterator	it = channel->ban.find(user);
	return (it != channel->ban.end());
}