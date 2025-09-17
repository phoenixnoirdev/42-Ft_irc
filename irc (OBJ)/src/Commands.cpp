#include "Commands.hpp"
# include "../inc/inc.hpp"

Kick::Kick()
{

}

Kick::Kick(const std::string& nick)
{
    addBan(nick);
}

Kick::~Kick()
{

}

bool Kick::isBanned(const std::string& nick) const
{
    for(std::vector<std::string>::const_iterator it = BanNick.begin(); it != BanNick.end(); ++it)
        if (*it == nick) return true;
    return false;
}

void Kick::addBan(const std::string& nick)
{
    if (!isBanned(nick))
        BanNick.push_back(nick);
}

void Kick::printBanList() const
{
    for(std::vector<std::string>::const_iterator it = BanNick.begin(); it != BanNick.end(); ++it)
    {
        std::cout << YELLOW << "- " << *it << RESET << std::endl;
    }
}

bool Kick::removeBan(const std::string& nick)
{
    for (std::vector<std::string>::iterator it = BanNick.begin(); it != BanNick.end(); ++it)
    {
        if (*it == nick)
        {
            BanNick.erase(it);
            return true;
        }
    }
    return false;
}

/*
const std::vector<std::string>& Kick::getBAn const
{
    return (BanNick);
}
*/