#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "Commands.hpp"
# include "server.hpp"
# include <iostream> 
# include <string>
# include <vector>


class Kick
{
    private:
    std::vector<std::string> BanNick;

    public:
    Kick();
    Kick(const std::string& nick);
    ~Kick();
    //const std::vector<std::string>& getBAn const;

    bool removeBan(const std::string& nick);
    void addBan(const std::string& nick);
    bool isBanned(const std::string& nick) const;
    void printBanList() const;

};


#endif
