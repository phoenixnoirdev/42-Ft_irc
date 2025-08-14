/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
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

User::User(std::string name, std::string nick, int grade)
{
    this->_Name = name;
    this->_Nick = nick;
    this->_Grade = grade;


    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;
    std::cout << GREEN << "New user" << RESET << std::endl;
    std::cout << YELLOW << "-- Name:" << CYAN  << this->_Name << RESET << std::endl;
    std::cout << YELLOW << "-- Nick: " << CYAN << this->_Nick <<  RESET << std::endl;
    std::cout << YELLOW << "-- Grade: " << CYAN << this->_Grade <<  RESET << std::endl;
    std::cout << YELLOW << "------------------------------------" << RESET << std::endl;   

    return ;
}


User::~User()
{
    
    return ;
}

//===============
//Fonctions public
//===============

std::string User::GetName()
{
    return this->_Name;
}

void User::SetName(std::string name)
{
    this->_Name = name;;
}


std::string User::GetNick()
{
    return this->_Nick;
}

void User::SetNick(std::string nick)
{
    this->_Nick = nick;
}


int User::GetGrad()
{
    return this->_Grade;
}

void User::SetGrade(int grade)
{
    this->_Grade = grade;
}

//===============
//Fonctions private
//===============