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

User::User(): _top(NULL), _bot(NULL), _size(0) {}

User::User(const User& other): _top(NULL), _bot(NULL), _size(0)
{
	t_user	*tmp;
	size_t	len;

	len = other.getSize();
	tmp = other.getTop();
	while (_size < len && tmp != NULL)
	{
		addUser(*tmp);
		tmp = tmp->next;
		_size++;
	}
}

User&	User::operator=(const User& other)
{
	if (this != &other)
	{
		t_user	*tmp;
		size_t	len;

		len = other.getSize();
		tmp = other.getTop();
		_size = 0;
		while (_size < len && tmp != NULL)
		{
			addUser(*tmp);
			tmp = tmp->next;
			_size++;
		}
	}
	return (*this);
}

User::~User()
{
	clear();
}

//=================
//Constructeur
//=================

User::User(t_user *list): _top(NULL), _bot(NULL), _size(0)
{
	t_user	*tmp;

	tmp = list;
	while (tmp != NULL)
	{
		addUser(*tmp);
		tmp = tmp->next;
		_size++;
	}
}

User::User(const std::string& json): _top(NULL), _bot(NULL), _size(0)
{
	(void)json;
}

//===============
//Fonctions public
//===============

void	User::addUser(
	const std::string& name_,
	const std::string& nick_,
	const std::string& pass_,
	int grade_)
{
	t_user	*tmp = new t_user(generateUUID(name_, nick_), name_, nick_, pass_, grade_);

	if (empty())
		_top = tmp;
	else
		_bot->next = tmp;
	_bot = tmp;
	_size++;
}

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

size_t	User::getSize() const
{
	return (_size);
}

t_user	*User::getTop() const
{
	return (_top);
}

t_user	*User::getBot() const
{
	return (_bot);
}

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

bool	User::empty() const
{
	return (_top == NULL);
}

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

//===============
//Fonctions private
//===============

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