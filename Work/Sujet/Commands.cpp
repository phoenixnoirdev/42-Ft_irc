#include "Commands.hpp"
# include "../inc/inc.hpp"

Kick::Kick(){}

Kick::~Kick(){}

bool Kick::isBanned(const std::string& nick) const
{
    for (size_t i = 0; i < _banNick.size(); ++i)
    {
            if (_banNick[i] == nick)
                return true;
        }
        return false;
}

void Kick::addBan(const std::string& nick)
{
     for (size_t i = 0; i < _banNick.size(); ++i)
     {
            if (_banNick[i] == nick)
                return;
    }
        _banNick.push_back(nick);
}

void Kick::printBanList() const
{
    for (size_t i = 0; i < _banNick.size(); ++i)
            std::cout << _banNick[i] << std::endl;
}

bool Kick::removeBan(const std::string& nick)
{
    for (std::vector<std::string>::iterator it = _banNick.begin(); it != _banNick.end(); ++it) {
            if (*it == nick) {
                _banNick.erase(it);
                return true;
            }
        }
        return false;
}


/*
/KICK <Nickname>
/quote UNBAN <Nickname>
*/