/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 07:46:19 by kelevequ          #+#    #+#             */
/*   Updated: 2025/08/19 07:49:32 by kelevequ         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netdb.h>
#include <cerrno>
#include <map>
#include "user.hpp"
#include "channel.hpp"

# define BUF_SIZE 4096 //Size max du buffer de lecture pour le message client

// Petite structure pour stocker les infos d’un client
/*
struct ClientInfo
{
    std::string nick;
    std::string user;
    std::string pass;

    bool gotNick;
    bool gotUser;
    bool gotPass;

    ClientInfo()
        : nick(""), user(""), pass(""),
          gotNick(false), gotUser(false), gotPass(false) {}
};*/

class Server
{
    private:
        std::string     _Pass;
        uint16_t	    _Port;
        struct in_addr  _Ip;

        std::string     _RecvBuffer; 
        int             _Listening;
        bool            _ServeurOn;
        
        //std::map<int, ClientInfo> _ClientsInfo;
        std::map<int, std::string> _PendingData; // buffer par client

        std::map<int, User> _User;
        std::map<int, Channel> _Chan;

        User            _UserObj;
        Channel         _ChanObj;

        fd_set _Readfds;


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

        //===============
        //Fonctions private
        //===============
        void Init();
        void AcceptClient();
        void Run();

        bool PassCont(const std::string& str);

        std::string GetPwd(const std::string& str);
        std::string GetNick(const std::string& str);
        std::string GetName(const std::string& str);

        void HandleClientData(int clientSocket);
};

#endif //SERVER.HPP