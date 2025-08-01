/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phkevin <phkevin@42luxembourg.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 11:21:29 by phkevin           #+#    #+#             */
/*   Updated: 2025/04/12 11:21:29 by phkevin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
#define ERROR_HPP

class Error
{
    private:


    public:
        //=================
        //Constructeur
        //=================
        

        //===============
        //Fonctions public
        //===============
        static void ErrorServ(int err);


        //===============
        //Fonctions private
        //===============
        static std::string GetValErr(int err);
        
};

#endif //ERROR.HPP