/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "bot.hpp"

Bot::Bot(std::string ip, std::string port, std::string pass)
{
    this->_Port = Utils::PortConvert(port);
    this->_Ip = Utils::IpConvert("localhost");
    this->_Pass = pass;
}

Bot::~Bot(void)
{

}
