/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 07:46:19 by kelevequ          #+#    #+#             */
/*   Updated: 2025/09/15 16:14:47 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Commands.hpp"

# include <string>
# include <netdb.h>
# include <cerrno>
# include <map>
# include <vector>
# include "user.hpp"
# include "channel.hpp"
# include "param.hpp"

class Kick;





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
       Kick* banList;

        
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

        Kick& getBanList();

        void kickUser(const std::string& nickToKick, int clientSocket, const std::string& kicker);

};



#endif //SERVER.HPP