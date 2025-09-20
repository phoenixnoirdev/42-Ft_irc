/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 07:46:19 by kelevequ          #+#    #+#             */
/*   Updated: 2025/09/17 14:26:44 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#pragma once

#include "Commands.hpp"   // aqui DEVE haver a definição de Kick (e NUNCA incluir server.hpp)
#include <string>
#include <netdb.h>
#include <cerrno>
#include <map>
#include <vector>
#include "user.hpp"
#include "channel.hpp"





# define BUF_SIZE 4096 //Size max du buffer de lecture pour le message client

class Server
{
    private:
        std::string     _Pass;
        uint16_t	    _Port;
        struct in_addr  _Ip;

        std::string     _RecvBuffer; 
        int             _Listening;
        bool            _ServeurOn;
        
        std::map<int, std::string> _PendingData;

        std::map<int, User> _User;
        std::map<int, Channel> _Chan;

        User            _UserObj;
        Channel         _ChanObj;

        fd_set _Readfds;

        //std::vector<std::string> _BanList; // List of banned nicks
       // BanNick banList;
        Kick _kick;

        
        //===============
        //Fonctions private
        //===============


    public:
        //=================
        //Constructeur
        //=================
        Server(std::string port, std::string pass);
        ~Server();

        //===============
        //Fonctions public
        //===============
        void Shutdown();
        void ShutSign();
        void Run();
        void Init();
        void AcceptClient();
        void HandleClientData(int clientSocket);

        bool PassCont(const std::string& str);

        std::string GetPwd(const std::string& str);
        std::string GetNick(const std::string& str);
        std::string GetName(const std::string&  str, bool auth);

        void handleKickCommand(int clientSocket, const std::string& line);
        void handleBanCommand(int clientSocket, const std::string& chanName, const std::string mask);
        //void handleUnbanCommand(int clientSocket, const std::string& line);

};



#endif //SERVER.HPP
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 07:46:19 by kelevequ          #+#    #+#             */
/*   Updated: 2025/09/05 13:58:43 by phkevin          ###   Luxembourg.lu     */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <netdb.h>
# include <cerrno>
# include <map>
# include "user.hpp"
# include "channel.hpp"

# define BUF_SIZE 4096 //Size max du buffer de lecture pour le message client

class Server
{
    private:
        std::string     _Pass;
        uint16_t	    _Port;
        struct in_addr  _Ip;

        std::string     _RecvBuffer; 
        int             _Listening;
        bool            _ServeurOn;
        
        std::map<int, std::string> _PendingData;

        std::map<int, User> _User;
        std::map<int, Channel> _Chan;

        User            _UserObj;
        Channel         _ChanObj;

        fd_set _Readfds;

        
        //===============
        //Fonctions private
        //===============


    public:
        //=================
        //Constructeur
        //=================
        Server(std::string port, std::string pass);
        ~Server();

        //===============
        //Fonctions public
        //===============
        void Shutdown();
        void ShutSign();
        void Run();
        void Init();
        void AcceptClient();
        void HandleClientData(int clientSocket);

        bool PassCont(const std::string& str);

        std::string GetPwd(const std::string& str);
        std::string GetNick(const std::string& str);
        std::string GetName(const std::string&  str, bool auth);
};

#endif //SERVER.HPP