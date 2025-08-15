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
		addUser(t_user(tmp));
		tmp = tmp->next;
		_size++;
	}
}

User&	User::operator=(const User& other)
{
	if (this != other)
	{
		t_user	*tmp;
		size_t	len;

		len = other.getSize();
		tmp = other.getTop();
		_size = 0;
		while (_size < len && tmp != NULL)
		{
			addUser(t_user(tmp));
			tmp = tmp->next;
			_size++;
		}
	}
	return (*this);
}

~User::User() {clear();}

//=================
//Constructeur
//=================

User::User(t_user *list): _top(NULL), _bot(NULL), _size(0)
{
	t_user	*tmp;

	tmp = other.getTop();
	while (tmp != NULL)
	{
		addUser(t_user(tmp));
		tmp = tmp->next;
		_size++;
	}
}

User::User(const std::string& json): _top(NULL), _bot(NULL), _size(0)
{

}

//===============
//Fonctions public
//===============

void	User::addUser(
	const std::string& uuid_,
	const std::string& name_,
	const std::string& nick_,
	const std::string& pass_,
	int grade_)
{
	t_user	*tmp = new t_user(uuid_, name_, nick_, pass_, grade_);

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

	while (top)
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
	int		i = 0;

	tmp = getUserByUUID(uuid, &i);
	prev = getUserByIndex(i - 1);
	prev->next = tmp->next;
	delete tmp;
	_size--;
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

t_user	*User::getUserByIndex(int index) const
{
	t_user	*tmp;
	int		i;

	if (index < 0 || index >= _size)
		return (NULL);
	tmp = getTop();
	else if (index == 0)
		return (tmp);
	i = 1;
	while (i < _size)
	{
		tmp = tmp->next;
		i++;
	}
	return (tmp);
}

t_user	*User::getUserByUUID(const std::string& uuid) const
{
	t_user	*tmp;
	int		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (uuid == tmp.uuid)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}

t_user	*User::getUserByUUID(const std::string& uuid, int *i) const
{
	t_user	*tmp;

	tmp = getTop();
	while ((*i) < _size)
	{
		if (uuid == tmp.uuid)
			return (tmp);
		tmp = tmp->next;
		(*i)++;
	}
	return (NULL);
}

t_user	*User::getUserByName(const std::string& name) const
{
	t_user	*tmp;
	int		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (name == tmp.name)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}

t_user	*User::getUserByNick(const std::string& nick) const
{
	t_user	*tmp;
	int		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (nick == tmp.nick)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}

t_user	*User::getUserByPass(const std::string& pass) const
{
	t_user	*tmp;
	int		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (pass == tmp.pass)
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
	int		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (uuid == tmp.uuid)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}

bool	User::hasName(const std::string& name) const
{
	t_user	*tmp;
	int		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (name == tmp.name)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}

bool	User::hasNick(const std::string& nick) const
{
	t_user	*tmp;
	int		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (nick == tmp.nick)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}

bool	User::hasPass(const std::string& pass) const
{
	t_user	*tmp;
	int		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (pass == tmp.pass)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}

//===============
//Fonctions private
//===============