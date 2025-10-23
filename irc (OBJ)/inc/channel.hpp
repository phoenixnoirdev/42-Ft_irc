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
# include <set>
# include "user.hpp"

class Channel
{
	private:

		//Structure des modes de canal
		struct ChannelModes {
			bool invite_only;    // +i
			bool topic_op_only;  // +t
			bool has_key;        // +k
			bool moderated;      // +m
			bool has_limit;      // +l
		} _modes;
	
		int					_id;			// Ref du channel
		std::string			_name;			// Chan name
		int					_user_limit; 	// Limite du nombre d'utilisateur (+l)
		std::string			_topic;			// Topice 
		
		std::string			_key;			// Password du channel

		User* _OpChannel;	// Operateur du channel (Createur du channel)
		
		std::map<int, User> _grade_0; 	// OP IRC SERV = operateur serveur
		std::map<int, User> _grade_1; 	// Modo = Moderateur du channel
		std::map<int, User> _grade_2; 	// Voix = peu discuter si le chan est en mode moderer
		std::map<int, User> _grade_3; 	// User = Peu discuter si le chan est en mode normal
		std::map<int, User> _ban;		// Liste desnuser banni du channel
		
		std::set<std::string> _invite_list;	// Liste des user inviter (+i)


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
		void SetId(int i);	// Set la reference du chan
		int GetId() const;	// Retourn la reference du chan

		//Name
		void SetName(const std::string& str);	// Set le nom du chan
		std::string GetName() const;			// Retourn le nom du chan

		//OP Channel
		void SetOpChannel(User& user);		// Set l'user qui a cree le channel
		int GetOpChannel();					// Retourn le socket de l'user qui a cree le channel
		bool isOpChannel(User user);		// Retourn si l'user a cree le chan

		// USER 
		void AddUser(const User& user);		// Ajoute un user lorsqu'il se log au channel
		int GetGradeUser(const User& user);	// Retourn le grade d'un user dans le channel
		std::map<int, User> GetLstGrade(int e) const; // Retourn la liste des grade souhaiter

		void AddUserBan(const User& user); 	// Ajoute un user a la liste des ban
		void RemoveUserBan(int sock);		// Unban un user
		bool GetUserBan(const User& user);	// Retourn si un user est ban
		std::map<int, User> GetBanMap();	// Retourn la liste des user ban
		void RemoveUser(int socket);		// Retire un user lorsqu'il se delog du chan 

		//MSG
		void BroadcastAll(const std::string &msg);
		void Broadcast(const std::string &msg, int sender);


		//===============
		//GESTION DES MODES IRC
		//===============

		// UserLimit (Modes +l/-l)
		int GetNbUser();				// Retourn le nombre d'user dans le chan
		void setUserLimit(int limit);	// Active la limt d'user si limit superieure a 0
		void removeUserLimit();			// Desactive la limite d'user
		bool hasUserLimit() const;		// Retourn si la limite d'user est active
		int getUserLimit() const;		// Retourn la limite d'utilisateur max du channel
		bool canJoin() const;			// Retourn si l'user peu se connecter au chan

		// Moderated (Modes +m/-m)
		void setModerated(bool enabled);		// permet de set le mode Moderated
		bool isModerated() const;				// Retourne l'etat du Moderated
		bool canSpeak(const User& user) const;	// Permet de confirmet l'autorisation de parler a un user (seul les grade <= 2)  
		
		//Topic Restriction (Modes +t/-t)
		void SetTopic(std::string topic);				// Permet de set le topic
		std::string GetTopic();							// Permet de retourner le topic
		void setTopicOpOnly(bool enabled);				// Permet de d'activer ou desactiver la restriction de topic
		bool isTopicOpOnly() const;						// Permet de retourner si la restriction est active ou non
		bool canChangeTopic(const User& user) const;	// Permet de retourner si l'user a le droit d'editer le topic (Seul les grade <= 1)

		//Moderateur privileg (Modes +o/-o)
		void giveOp(const std::string& nick);		// Permet d'ajouter un moderateur
		void removeOp(const std::string& nick);		// Permet de retirer un moderateur
		bool isOperator(const User& user) const;	// Retourn si l'user est operateur (Seul les grade 1 et 0)

		//Invite-only (Modes +i/-i)
		void setInviteOnly(bool enabled);					// Permet d'activer le mode inviter uniquement
		bool isInviteOnly() const;							// Retourn si l'etat du mode inviter
		void addInvite(const std::string& nick);			// Ajoute un user a la liste des inviter
		void removeInvite(const std::string& nick);			// Retire un user a la liste des inviter
		bool isInvited(const std::string& nick) const;		// Retourn si l'user fait partie des inviter

		//Channel key (Modes +k/-k)
		void setKey(const std::string& key);						// Active le password sur le channel si key et non vide
		void removeKey();											// Desactive le password et clear key
		bool hasKey() const;										// Retourn si le password est actife sur le chan
		bool checkKey(const std::string& provided_key) const;		// Retourn si le passwotd correspond a celui du chan
		std::string getKey() const;									// Retourn le password

		//Methode generale appliquer au modes
		bool applyMode(const User& user, const std::string& modestring, const std::string& param = "");
		std::string getModeString() const;
};

#endif //channel.HPP
