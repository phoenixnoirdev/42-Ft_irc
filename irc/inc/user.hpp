/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include <string>

typedef struct	s_user
{
	std::string		uuid;
	std::string		name;
	std::string		nick;
	std::string		pass;
	int				grade;
	struct s_user	*next;

	t_user(	const std::string& uuid_,
			const std::string& name_,
			const std::string& nick_,
			const std::string& pass_,
			int grade_):
		uuid(uuid_),
		name(name_),
		nick(nick_),
		pass(pass_),
		grade(grade_),
		next(NULL)
	{}
	t_user(const t_user& other):
		uuid(other.uuid),
		name(other.name),
		nick(other.nick),
		pass(other.pass),
		grade(other.grade),
		next(NULL)
	{}
}					t_user;

class User
{
	private:
		t_user	*_top;
		t_user	*_bot;
		size_t	_size;

		//===============
		//Fonctions private
		//===============

	public:
		//=================
		//Canonical Form
		//=================
		User();
		User(const User& other);
		User&	operator=(const User& other);
		~User();

		//=================
		//Constructeur
		//=================
		User(t_user *list);
		User(const std::string& json);

		//===============
		//Fonctions public
		//===============
		void	addUser(const std::string& uuid_,
						const std::string& name_,
						const std::string& nick_,
						const std::string& pass_,
						int grade_);
		void	addUser(const t_user& other);

		//Deletion
		void	clear();
		bool	removeUserByUUID(const std::string& uuid);

		//Information Retrieval
		size_t	getSize() const;
		t_user	*getTop() const;
		t_user	*getBot() const;
		t_user	*getUserByIndex(int index) const;
		t_user	*getUserByUUID(const std::string& uuid) const;
		t_user	*getUserByName(const std::string& name) const;
		t_user	*getUserByNick(const std::string& nick) const;
		t_user	*getUserByPass(const std::string& pass) const;
		t_user	*getUserByUUID(const std::string& uuid, int *id) const;

		//Information Verification
		bool	empty() const;
		bool	hasUUID(const std::string& uuid) const;
		bool	hasName(const std::string& name) const;
		bool	hasNick(const std::string& nick) const;
		bool	hasPass(const std::string& pass) const;
};

#endif //USER.HPP