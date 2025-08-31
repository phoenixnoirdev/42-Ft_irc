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

class User
{
	private:
		std::string		_uuid;
		std::string		_name;
		std::string		_nick;
		std::string		_pass;
		std::string 	_recvBuffer;

		int				_grade;
		int 			_socket;
		int				_idChan;

		bool 			_auth;


		//===============
		//Fonctions private
		//===============
		std::string	generateUUID(const std::string& name, const std::string& nick);

	public:
		//=================
		//Canonical Form
		//=================
		User();
		User(int socket);
		User(const User& other);
		User&	operator=(const User& other);
		~User();

		//===============
		//Fonctions public
		//===============

		//UUID
		void setUuid(std::string str);
		std::string getUuid() const;

		//Name
		void setName(std::string str);
		std::string getName() const;

		//Nick
		void setNick(std::string str);
		std::string getNick() const;

		//Pass
		void setPass(std::string str);
		std::string getPass() const;

		//Buffer
		void setRcvBuff(std::string str);
		const std::string getRcvBuff() const;
		std::string& getRcvBuff();

		//Grade
		void setGrade(int i);
		int getGrade() const;

		//Socket
		void setSocket(int i);
		int getSocket() const;

		//User in chan
		void setIdChan(int i);
		int getIdChan() const;

		//Authentification
		void setAuth(bool e);
		bool getAuth() const;

		//Ferme le socket
		void closeSocket();




		/*
		void	addUser(const std::string& name_,
						const std::string& nick_,
						const std::string& pass_,
						int grade,
						int socket);
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
		void	closeAllSockets(); //Close all socket
		*/
};

#endif //USER.HPP