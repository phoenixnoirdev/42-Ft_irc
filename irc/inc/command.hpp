/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-fif <luis-fif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 10:37:33 by luis-fif          #+#    #+#             */
/*   Updated: 2025/08/21 14:14:37 by luis-fif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include <sstream>
#include <vector>
#include <string>

class   Command
{
    private:
        Command();
        Command(const Command& other);
        Command&    operator=(const Command& other);
        ~Command();

        std::vector<std::string> splitInput(const std::string& input);
        std::string toUpper(const std::string& s);

        void handleKick(const std::vector<std::string>& args, User *userDB, Channel *channelDB);
        void handleInvite(const std::vector<std::string>& args, User *userDB, Channel *channelDB);
        void handleTopic(const std::vector<std::string>& args, User *userDB, Channel *channelDB);
        void handleMode(const std::vector<std::string>& args, User *userDB, Channel *channelDB);

    public:
        void commandHandler(const std::string& input, User *userDB, Channel *channelDB);
}

