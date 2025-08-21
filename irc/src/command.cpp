/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 13:05:03 by luis-fif          #+#    #+#             */
/*   Updated: 2025/08/21 14:15:07 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "command.hpp"

void Command::commandHandler(const std::string& input, User *userDB, Channel *channelDB, t_user *user)
{
    if (input.empty())
        return;

    std::vector<std::string> tokens = splitInput(input); // handle by spaces
    std::string cmd = toUpper(tokens[0]);


    if (cmd == "KICK" || cmd == "INVITE" || cmd == "TOPIC" || cmd == "MODE")
    {
        if (tokens.size() < 2)
        {
            sendError(user, "461 " + cmd + " :Not enough parameters");
            return;
        }

        t_channel *chan = channelDB->getChannelByName(tokens[1]);
        if (!chan)
        {
            sendError(user, "403 " + tokens[1] + " :No such channel");
            return;
        }

        //  only grade 0 ou 1 can use KICK/INVITE/MODE/TOPIC
        int requiredGrade = 1; // operador mínimo
        if (!channelDB->userHasGrade(chan, user, requiredGrade))
        {
            sendError(user, "482 :You're not channel operator");
            return;
        }

        // Execut command
        if (cmd == "KICK")
            handleKick(tokens, userDB, channelDB);
        else if (cmd == "INVITE")
            handleInvite(tokens, userDB, channelDB);
        else if (cmd == "TOPIC")
            handleTopic(tokens, userDB, channelDB);
        else if (cmd == "MODE")
            handleMode(tokens, userDB, channelDB);
    }
    else
    {
        // command error
        sendError(user, "421 " + cmd + " :Unknown command");
    }
}



void handleKick(const std::vector<std::string>& args, User *userDB, Channel *channelDB)
{
    std::cout << YELLOW << "Kick called" << RESET << std::endl;
}

void handleInvite(const std::vector<std::string>& args, User *userDB, Channel *channelDB)
{
    std::cout << YELLOW << "Invite called" << RESET << std::endl;
}

void handleTopic(const std::vector<std::string>& args, User *userDB, Channel *channelDB)
{
    std::cout << YELLOW << "Topic called" << RESET << std::endl;
}
    
void handleMode(const std::vector<std::string>& args, User *userDB, Channel *channelDB)
{
    std::cout << YELLOW << "Mode called" << RESET << std::endl;
}

