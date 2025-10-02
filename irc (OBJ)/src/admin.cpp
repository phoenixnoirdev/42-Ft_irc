#include "../inc/admin.hpp"
#include <cstdlib>

AdminConfig::AdminConfig() : maxUsersDefault(50) {
}

std::string AdminConfig::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> AdminConfig::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string current = "";
    
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == delimiter) {
            if (!current.empty()) {
                result.push_back(trim(current));
                current = "";
            }
        } else {
            current += str[i];
        }
    }
    
    if (!current.empty()) {
        result.push_back(trim(current));
    }
    
    return result;
}

bool AdminConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open admin config file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Find the = separator
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key == "MAIN_ADMIN") {
            mainAdmin = value;
        } else if (key == "ADMIN_PASSWORD") {
            adminPassword = value;
        } else if (key == "OPERATORS") {
            operators = split(value, ',');
        } else if (key == "VOICE_USERS") {
            voiceUsers = split(value, ',');
        } else if (key == "AUTO_OP_CHANNELS") {
            autoOpChannels = split(value, ',');
        } else if (key == "BANNED_USERS") {
            bannedUsers = split(value, ',');
        } else if (key == "OPER_PASSWORD") {
            operPassword = value;
        } else if (key == "MAX_USERS_DEFAULT") {
            maxUsersDefault = atoi(value.c_str());
        }
    }
    
    file.close();
    
    std::cout << "Admin config loaded: Main admin=" << mainAdmin 
              << ", Operators=" << operators.size() 
              << ", Voice users=" << voiceUsers.size()
              << ", Auto-op channels=" << autoOpChannels.size() << std::endl;
    
    return true;
}

bool AdminConfig::isOperator(const std::string& nickname) const {
    for (size_t i = 0; i < operators.size(); i++) {
        if (operators[i] == nickname) {
            return true;
        }
    }
    return false;
}

bool AdminConfig::isAutoOpChannel(const std::string& channel) const {
    for (size_t i = 0; i < autoOpChannels.size(); i++) {
        if (autoOpChannels[i] == channel) {
            return true;
        }
    }
    return false;
}

bool AdminConfig::isBannedUser(const std::string& nickname) const {
    for (size_t i = 0; i < bannedUsers.size(); i++) {
        if (bannedUsers[i] == nickname) {
            return true;
        }
    }
    return false;
}

bool AdminConfig::isVoiceUser(const std::string& nickname) const {
    for (size_t i = 0; i < voiceUsers.size(); i++) {
        if (voiceUsers[i] == nickname) {
            return true;
        }
    }
    return false;
}

int AdminConfig::getUserGrade(const std::string& nickname) const {
    if (nickname == mainAdmin) {
        return 0; // Server Admin
    } else if (isOperator(nickname)) {
        return 1; // Channel Operator
    } else if (isVoiceUser(nickname)) {
        return 2; // Voice User
    } else {
        return 3; // Regular User
    }
}

bool AdminConfig::hasServerAdminPrivs(const std::string& nickname) const {
    return (nickname == mainAdmin);
}

bool AdminConfig::hasOperatorPrivs(const std::string& nickname) const {
    return (nickname == mainAdmin || isOperator(nickname));
}

bool AdminConfig::validateAdminCredentials(const std::string& nickname, const std::string& password) const {
    return (nickname == mainAdmin && password == adminPassword);
}