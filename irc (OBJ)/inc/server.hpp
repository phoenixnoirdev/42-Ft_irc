/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 07:46:19 by kelevequ          #+#    #+#             */
/*   Updated: 2025/09/29 12:43:37 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#pragma once

#include <string>
#include <netdb.h>
#include <cerrno>
#include <map>
#include <vector>
#include "user.hpp"
#include "channel.hpp"
#include "admin.hpp"
#include "file.hpp"





# define BUF_SIZE 4096 //Size max du buffer de lecture pour le message client

class Server
{
	private:
		std::string	_ServName;
		
		std::string		_Pass;
		uint16_t		_Port;
		struct in_addr	_Ip;

		std::string	_RecvBuffer; 
		int			_Listening;
		bool		_ServeurOn;
		
		std::map<int, std::string>	_PendingData;

		std::map<int, User>			_User;
		std::map<int, Channel>		_Chan;
		std::map<std::string, File>	_Files;

		User	_UserObj;
		Channel	_ChanObj;
		AdminConfig     _AdminConfig;
        bool            _hasMainAdmin;
        std::string     _mainAdminNick;

		fd_set	_Readfds;

		
		//===============
		//Fonctions private
		//===============
		bool	NickIsList(std::string nick);
		void	_msgUser(User *target, User& user, std::string filename);

        void handleWelcome(const User& user);


	public:
		//=================
		//Constructeur
		//=================
		Server(std::string port, std::string pass);
		~Server();

		//===============
		//Fonctions public
		//===============
		void	Shutdown();
		void	ShutSign();
		void	Run();
		void	Init();
		void	AcceptClient();
		void	HandleClientData(int clientSocket);

		bool	PassCont(const std::string& str);
		bool	IsNickIsList(std::string nick);

		std::string	GetPwd(const std::string& str);
		std::string	GetNick(const std::string& str);
		std::string	GetName(const std::string&  str, bool auth);

        void handleLogin(int clientSocket, User& user);
        
        void handleKickCommand(int clientSocket, const std::string& line);
        void handleBanCommand(int clientSocket, const std::string& chanName, const std::string mask);
        void handleBanlistCommand(int clientSocket, const std::string& chanName);
        void handleUnbanCommand(int clientSocket, const std::string& chanName, const std::string mask);

		void	handleBrodcastMsgKB(User& user, std::string line);
		void	handleBrodcastPrivateMsg(User& user, std::string line);
		void	handleBrodcastMsgChann(int clientSocket, User& user, std::string line, int idchan);

		void	handleJoin(int clientSocket, User& user, const std::string& line);

		void	handleQuit(int clientSocket, User& user, const std::string& line);

		void	handleNames(User& user, const std::string& line);

		void	handleList(User& user);

		void	handleFileList();
		void	handleFileSend(User& user, const std::string& line);
		void	handleFileGet(User& user, const std::string& line);

        void handleTopic(User& user, const std::string& line);

		
		void loadAdminConfig();
        bool checkUserBanned(const std::string& nickname);
        void giveAutoOpPrivileges(int clientSocket, const std::string& channel);
        void handleOperCommand(int clientSocket, const std::string& line);
        void handleAdminCommand(int clientSocket, const std::string& line);
        void handleGradesCommand(int clientSocket, const std::string& line);
        void handleSetGradeCommand(int clientSocket, const std::string& line);
        void sendWelcomeMessage(int clientSocket, const User& user);
        void displayAdminInfo();
};

// Funções auxiliares para MODE (declaradas externamente)
std::vector<std::string> splitString(const std::string& str);
Channel* findChannelByName(const std::string& channelName, std::map<int, Channel>& channels);
User* findUserInChannelByNick(const std::string& nick, Channel& channel, std::map<int, User>& users);
void sendIRCError(int clientSocket, int errorCode, const std::string& message);
void sendModeResponse(Channel& channel, const User& user, const std::string& modeString, const std::string& param);

// Handlers principais para MODE
void handleModeCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels);
void handleInviteCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels);
void handleTopicCommand(int clientSocket, const std::string& line, std::map<int, User>& users, std::map<int, Channel>& channels);



#endif //SERVER.HPP
