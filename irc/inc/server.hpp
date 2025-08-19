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

# define BUF_SIZE 4096 //Size max du buffer de lecture pour le message client

class Server
{
    private:
        std::string     _Pass;
        uint16_t	    _Port;
        struct in_addr  _Ip;

        std::string     _RecvBuffer; 
        int             _Listening;
        int			    _ClientSocket;
        bool            _ServeurOn;


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

        //===============
        //Fonctions private
        //===============
        void Init();
        void AcceptClient();
        void Run();

        bool PassCont();
};

#endif //SERVER.HPP