/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/09/17 13:09:54 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <map>
# include "user.hpp"

class Channel
{
	private:
		int					_id;
		std::string			_name;
		int					_Mode;
		
		std::map<int, User> _grade_0; // OP IRC SERV = operateur serveur
		std::map<int, User> _grade_1; // Op = Operateur channel
		std::map<int, User> _grade_2; // Voix = peu discuter si le chan est en mode moderer
		std::map<int, User> _grade_3; // User = Peu discuter si le chan est en mode normal
		std::map<int, User> _ban;

		//===============
		//Fonctions private
		//===============

	public:
		//=================
		//Canonical Form
		//=================
		Channel();
		Channel(int id, std::string name);
		Channel(const Channel& other);
		Channel&	operator=(const Channel& other);
		~Channel();


		//===============
		//Fonctions public
		//===============
		
		//Id
		void SetId(int i);
		int GetId() const;

		//Name
		void SetName(const std::string& str);
		std::string GetName() const;

		//Mode Gest
		void SetMode(const User& user);
		void GetMode(const User& user);

		void AddUser(const User& user);
		int GetGradeUser(const User& user);

		void AddUserBan(const User& user);
		void RemoveUserBan(int sock);
		bool GetUserBan(const User& user);
		std::map<int, User> GetBanMap();

		void BroadcastAll(const std::string &msg);
		void Broadcast(const std::string &msg, int sender);

		void RemoveUser(int socket);

		int GetPop();
};

#endif //channel.HPP
