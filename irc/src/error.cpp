/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/inc.hpp"

//=================
//Constructeur
//=================


//===============
//Fonctions public
//===============

void Error::ErrorServ(int err)
{
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;

    if (err == 1)
        std::cerr << RED << "Error " << Error::GetValErr(err) << " : port conversion fail" << RESET << std::endl;
    if (err == 2)
        std::cerr << RED << "Error " << Error::GetValErr(err) << " : Port out of range" << RESET << std::endl;
    
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;

    std::exit(EXIT_FAILURE);
}


//===============
//Fonctions private
//===============

std::string Error::GetValErr(int err)
{
    return std::string(GREEN) + std::to_string(err) + RED;
}

