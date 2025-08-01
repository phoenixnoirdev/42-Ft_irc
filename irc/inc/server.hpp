/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netdb.h>

//==============================
// PORT
//==============================
#define PORT_MAX 65535
#define PORT_MIN 1

# define BUF_SIZE 4096 //Size max du buffer de lecture pour le message client

class Server
{
    private:
        std::string _Pass;
        uint16_t	_Port;


    public:
        //=================
        //Constructeur
        //=================
        Server(std::string port, std::string pass);
        ~Server();

        //===============
        //Fonctions public
        //===============



        //===============
        //Fonctions private
        //===============
        void PortConvert(const std::string& arg);
};

#endif //SERVER.HPP