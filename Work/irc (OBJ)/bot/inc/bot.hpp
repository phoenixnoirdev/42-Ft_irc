/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

# include <string>
# include <netdb.h>
# include <vector>
# include <cstdlib>
# include <ctime>
# include "botutils.hpp"
# include "botinc.hpp"

class Bot
{
    private:
        uint16_t	                _Port;
        struct in_addr              _Ip;
        std::string                 _Pass;
        std::string                 _Chan;
        int                         _Sock;
        bool                        _BotOn;
        std::vector<std::string>    _MsgFR;
        std::vector<std::string>    _MsgENG;

        //===============
        //Fonctions private
        //===============

    

    public:
        //=================
        //Constructeur
        //=================
        Bot(std::string ip, std::string port, std::string pass, std::string chan);
        ~Bot();

        //===============
        //Fonctions public
        //===============
        void shutSign();
        void run();
        void connectToServer();
        void loginBot();
        void sendRaw(const std::string& msg);
        void shutdown();
        void sendMessage();
        void initMsgFr();
        void initMsgEng();     
};

#endif //BOT.HPP