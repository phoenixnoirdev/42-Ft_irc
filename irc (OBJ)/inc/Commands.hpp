#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#pragma once
#include <vector>
#include <string>
#include <iostream>

class Server;

class Kick {
private:
    std::vector<std::string> _banNick;

public:
    Kick();
    ~Kick();

    void addBan(const std::string& nick);
    bool removeBan(const std::string& nick);
    bool isBanned(const std::string& nick) const;
    void printBanList() const;
};

#endif
