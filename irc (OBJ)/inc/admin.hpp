#ifndef ADMIN_HPP
#define ADMIN_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

class AdminConfig {
private:
    std::string mainAdmin;
    std::string adminPassword;
    std::vector<std::string> operators;
    std::vector<std::string> voiceUsers;
    std::vector<std::string> autoOpChannels;
    std::vector<std::string> bannedUsers;
    std::string operPassword;
    int maxUsersDefault;
    
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);

public:
    AdminConfig();
    bool loadFromFile(const std::string& filename);
    
    // Getters
    std::string getMainAdmin() const { return mainAdmin; }
    std::string getAdminPassword() const { return adminPassword; }
    bool isOperator(const std::string& nickname) const;
    bool isVoiceUser(const std::string& nickname) const;
    bool isAutoOpChannel(const std::string& channel) const;
    bool isBannedUser(const std::string& nickname) const;
    std::string getOperPassword() const { return operPassword; }
    int getMaxUsersDefault() const { return maxUsersDefault; }
    
    // User privilege levels
    int getUserGrade(const std::string& nickname) const;
    bool hasServerAdminPrivs(const std::string& nickname) const;
    bool hasOperatorPrivs(const std::string& nickname) const;
    
    // Authentication methods
    bool validateAdminCredentials(const std::string& nickname, const std::string& password) const;
};

#endif