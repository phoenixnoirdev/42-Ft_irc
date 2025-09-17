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
		int					_view_grade;
		
		std::map<int, User> _grade_0;
		std::map<int, User> _grade_1;
		std::map<int, User> _grade_2;
		std::map<int, User> _grade_3;
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

		//View Grade
		void SetViewGrad(int i);
		int GetViewGradd() const;

		void AddUser(const User& user);

		void BroadcastJoin(const std::string &msg);
		void Broadcast(const std::string &msg, int sender);

		void RemoveUser(int socket);
};

#endif //channel.HPP