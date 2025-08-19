/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/08/19 13:42:08 by kelevequ         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <map>
# include "user.hpp"

typedef struct	s_channel
{
	int					id;
	std::string			name;
	int					view_grade;

	std::map<s_user*, std::string>	grade_0;
	std::map<s_user*, std::string>	grade_1;
	std::map<s_user*, std::string>	grade_2;
	std::map<s_user*, std::string>	grade_3;
	std::map<s_user*, std::string>	ban;

	struct s_channel	*next;

	s_channel(int id_, const std::string& name_, int grade_):
		id(id_), name(name_), view_grade(grade_), next(NULL)
	{}
	s_channel(const s_channel& other):
		id(other.id),
		name(other.name),
		view_grade(other.view_grade),
		grade_0(other.grade_0),
		grade_1(other.grade_1),
		grade_2(other.grade_2),
		grade_3(other.grade_3),
		ban(other.ban),
		next(NULL)
	{}
}					t_channel;

class Channel
{
	private:
		t_channel	*_top;
		t_channel	*_bot;
		size_t		_size;

		//===============
		//Fonctions private
		//===============

	public:
		//=================
		//Canonical Form
		//=================
		Channel();
		Channel(const Channel& other);
		Channel&	operator=(const Channel& other);
		~Channel();

		//=================
		//Constructeur
		//=================
		Channel(t_channel *list);
		Channel(const std::string& json); //Not implemented

		//===============
		//Fonctions public
		//===============
		void	addChannel(int id, const std::string& name, int grade, t_user *creator);
		void	addChannel(const t_channel& other);
		bool	addUserChannel(t_channel *channel, t_user *user, int grade);

		//Deletion
		void	clear();
		bool	removeChannelByID(int id);
		bool	removeChannelByName(const std::string& name);

		//Information Retrieval
		size_t			getSize() const;
		t_channel		*getTop() const;
		t_channel		*getBot() const;
		t_channel		*getChannelByID(int id) const;
		t_channel		*getChannelByID(int id, size_t *i) const;
		t_channel		*getChannelByIndex(size_t index) const;
		t_channel		*getChannelByName(const std::string& name) const;
		t_channel		*getChannelByName(const std::string& name, size_t *i) const;
		t_user			*getChannelUserList(t_channel *channel, int grade) const; //Not implemented
		std::string&	getChannelUserDisplayName(t_channel *channel, t_user *user) const;

		//Information Modification
		bool	setChannelUserDisplayName(t_channel *channel, t_user *user, const std::string& displayName);
		bool	promoteChannelUser(t_channel *channel, t_user *user, int grade);
		bool	demoteChannelUser(t_channel *channel, t_user *user, int grade);
		bool	banChannelUser(t_channel *channel, t_user *user, const std::string& reason);

		//Information Verification
		bool	empty() const;
		bool	hasID(int id) const;
		bool	hasName(const std::string& name) const;
		bool	channelHasUser(t_channel *channel, t_user *user, int grade) const;
		bool	isChannelUserBan(t_channel *channel, t_user *user) const;

		//Information Utility
		void	printChannelDB() const; //Not implemented
		void	printChannel(t_channel *channel) const; //Not implemented
		void	saveChannelDB(const std::string& filepath) const; //Not implemented
		void	saveChannel(t_channel *channel, const std::string& filepath) const; //Not implemented
};

#endif //channel.HPP