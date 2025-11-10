/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_utils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 13:27:59 by luis-fif          #+#    #+#             */
/*   Updated: 2025/08/21 14:14:56 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "command.hpp"

std::vector<std::string> splitInput(const std::string &input)
{
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string word;

    while (iss >> word)
    {
        if (word[0] == ':')
        {
            // junta o resto da linha como um único argumento
            std::string rest;
            std::getline(iss, rest);
            if (!rest.empty())
                word += rest;
            tokens.push_back(word.substr(1)); // remove o ':'
            return tokens;
        }
        tokens.push_back(word);
    }
    return tokens;
}

/*bool Channel::userHasGrade(t_channel *chan, t_user *user, int requiredGrade)
{
    switch (requiredGrade) {
        case 0: return chan->grade_0.count(user) > 0;
        case 1: return chan->grade_0.count(user) > 0 || chan->grade_1.count(user) > 0;
        case 2: return chan->grade_0.count(user) > 0 || chan->grade_1.count(user) > 0 || chan->grade_2.count(user) > 0;
        case 3: return chan->grade_0.count(user) > 0 || chan->grade_1.count(user) > 0 || chan->grade_2.count(user) > 0 || chan->grade_3.count(user) > 0;
        default: return false;
    }
}*/

//  Channel.hpp
bool Channel::userHasGrade(t_channel *chan, t_user *user, int requiredGrade) const
{
    // Supondo que mudaste para: std::map<s_user*, int> grades; 0=root,1=admin,2=user,3=viewer
    std::map<s_user*, int>::iterator it = chan->grades.find(user);
    if (it == chan->grades.end())
        return false; // user not in channel

    int userGrade = it->second;
    return userGrade <= requiredGrade; // grade menor = mais poder
}
