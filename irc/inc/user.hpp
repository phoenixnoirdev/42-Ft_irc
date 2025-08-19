/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/08/19 13:41:55 by kelevequ         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <string>

typedef struct	s_user
{
	std::string		uuid;
	std::string		name;
	std::string		nick;
	std::string		pass;
	int				grade;
	struct s_user	*next;

	s_user(	const std::string& uuid_,
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
	s_user(const s_user& other):
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
		std::string	generateUUID(const std::string& name, const std::string& nick);

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
		User(const std::string& json); //Not implemented

		//===============
		//Fonctions public
		//===============
		void	addUser(const std::string& name_,
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
		t_user	*getUserByIndex(size_t index) const;
		t_user	*getUserByUUID(const std::string& uuid) const;
		t_user	*getUserByName(const std::string& name) const;
		t_user	*getUserByNick(const std::string& nick) const;
		t_user	*getUserByPass(const std::string& pass) const;
		t_user	*getUserByUUID(const std::string& uuid, size_t *id) const;

		//Information Verification
		bool	empty() const;
		bool	hasUUID(const std::string& uuid) const;
		bool	hasName(const std::string& name) const;
		bool	hasNick(const std::string& nick) const;
		bool	hasPass(const std::string& pass) const;

		//Information Utility
		void	printUserDB() const; //Not implemented
		void	printUser(t_user *user) const; //Not implemented
		void	saveUserDB(const std::string& filepath) const; //Not implemented
		void	saveUser(t_user *user, const std::string& filepath) const; //Not implemented
};

#endif //USER.HPP