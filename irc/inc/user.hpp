/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include <string>

class User
{
    private:
        std::string _Name;
        std::string _Nick;
        int _Grade;

    public:
        //=================
        //Constructeur
        //=================
        User(std::string name, std::string nick, int grade);
        ~User();


        //===============
        //Fonctions public
        //===============
        std::string GetName();
        void SetName(std::string name);

        std::string GetNick();
        void SetNick(std::string nick);

        int GetGrad();
        void SetGrade(int grade);


        //===============
        //Fonctions private
        //===============
        
};

#endif //USER.HPP