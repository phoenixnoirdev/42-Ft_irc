/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/10/02 13:54:28 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <map>
# include <set>
# include "user.hpp"

class Channel
{
	private:
		int					_id;
		std::string			_name;
		
		// Estrutura para os modos do canal
		struct ChannelModes {
			bool invite_only;    // +i
			bool topic_op_only;  // +t
			bool has_key;        // +k
			bool moderated;      // +m
			bool has_limit;      // +l
		} _modes;
		
		std::string			_key;			// senha do canal (+k)
		int					_user_limit;	// limite de usuários (+l)
		std::string			_topic;			// tópico do canal
		std::set<std::string> _invite_list;	// lista de convidados (+i)
		
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

		//Mode Gest (Antigo sistema - manter compatibilidade)
		void SetMode(const User& user);
		void GetMode(const User& user);
		
		//Novos métodos para modos IRC
		//Modo +i/-i (Invite-only)
		void setInviteOnly(bool enabled);
		bool isInviteOnly() const;
		void addInvite(const std::string& nick);
		void removeInvite(const std::string& nick);
		bool isInvited(const std::string& nick) const;
		
		//Modo +t/-t (Topic restriction)
		void setTopicOpOnly(bool enabled);
		bool isTopicOpOnly() const;
		bool canChangeTopic(const User& user) const;
		void setTopic(const std::string& topic);
		std::string getTopic() const;
		
		//Modo +k/-k (Channel key)
		void setKey(const std::string& key);
		void removeKey();
		bool hasKey() const;
		bool checkKey(const std::string& provided_key) const;
		std::string getKey() const;
		
		//Modo +o/-o (Operator privilege)
		void giveOp(const std::string& nick);
		void removeOp(const std::string& nick);
		bool isOperator(const User& user) const;
		
		//Modo +l/-l (User limit)
		void setUserLimit(int limit);
		void removeUserLimit();
		bool hasUserLimit() const;
		int getUserLimit() const;
		bool canJoin() const;
		
		//Modo +m/-m (Moderated)
		void setModerated(bool enabled);
		bool isModerated() const;
		bool canSpeak(const User& user) const;
		
		//Método geral para aplicar modos
		bool applyMode(const User& user, const std::string& modestring, const std::string& param = "");
		std::string getModeString() const;

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
