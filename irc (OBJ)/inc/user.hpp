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
};

#endif //USER.HPP