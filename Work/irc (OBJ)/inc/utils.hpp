/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <netdb.h>

//==============================
// PORT
//==============================
#define PORT_MAX 65535
#define PORT_MIN 1

class Utils
{
    private:

        //===============
        //Fonctions private
        //===============

    public:
        //=================
        //Constructeur
        //=================

        //===============
        //Fonctions public
        //===============
        static uint16_t PortConvert(const std::string& arg);
        
        static struct in_addr IpConvert(const std::string& ipStr);
        
        static std::string IntToString(int i);
        
        static bool IsSocketWritable(int sock);
             
};

#endif //SERVER.HPP