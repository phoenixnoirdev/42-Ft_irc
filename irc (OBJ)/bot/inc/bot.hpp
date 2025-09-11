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
# include "botutils.hpp"

class Bot
{
    private:
        uint16_t	    _Port;
        struct in_addr  _Ip;
        std::string     _Pass;

    public:
        //=================
        //Constructeur
        //=================
        Bot(std::string ip, std::string port, std::string pass);

        //===============
        //Fonctions public
        //===============


        //===============
        //Fonctions private
        //===============
        
};

#endif //BOT.HPP