#include "../inc/admin.hpp"
#include <cstdlib>

/**
 * @brief Constructeur par défaut de la classe AdminConfig.
 * 
 * Initialise la configuration par défaut de l'administrateur, notamment
 * le nombre maximal d'utilisateurs autorisés par défaut.
 * 
 * @note La valeur par défaut de maxUsersDefault est fixée à 50.
 */
AdminConfig::AdminConfig() : maxUsersDefault(50) {}

/**
 * @brief Supprime les espaces et caractères de contrôle en début et fin de chaîne.
 * 
 * Cette fonction enlève les espaces, tabulations, retours chariot et sauts de ligne
 * au début et à la fin de la chaîne passée en paramètre.
 * 
 * @param str La chaîne à nettoyer.
 * @return std::string La chaîne sans espaces ni caractères de contrôle en début et fin.
 */
std::string AdminConfig::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Sépare une chaîne en sous-chaînes selon un délimiteur donné.
 * 
 * Cette fonction parcourt la chaîne `str` et découpe ses éléments lorsqu'elle
 * rencontre le caractère `delimiter`. Chaque sous-chaîne est également nettoyée
 * des espaces et caractères de contrôle en début et fin grâce à la fonction `trim`.
 * 
 * @param str La chaîne à découper.
 * @param delimiter Le caractère délimiteur utilisé pour séparer les sous-chaînes.
 * @return std::vector<std::string> Un vecteur contenant toutes les sous-chaînes extraites.
 */
std::vector<std::string> AdminConfig::split(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    std::string current = "";
    
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == delimiter)
        {
            if (!current.empty())
            {
                result.push_back(trim(current));
                current = "";
            }
        }
        else
        {
            current += str[i];
        }
    }
    
    if (!current.empty())
    {
        result.push_back(trim(current));
    }
    
    return result;
}

/**
 * @brief Charge la configuration des administrateurs à partir d'un fichier.
 * 
 * Cette fonction lit le fichier `filename` ligne par ligne, ignore les lignes
 * vides et les commentaires, puis extrait les paires clé=valeur pour remplir
 * les champs de configuration tels que `mainAdmin`, `adminPassword`, `operators`,
 * `voiceUsers`, `autoOpChannels`, `bannedUsers`, `operPassword` et `maxUsersDefault`.
 * 
 * @param filename Le chemin du fichier de configuration à lire.
 * @return true Si le fichier a été lu et analysé avec succès.
 * @return false Si le fichier n'a pas pu être ouvert.
 */
bool AdminConfig::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        std::cerr << "Warning: Could not open admin config file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        
        if (line.empty() || line[0] == '#')
        {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos)
        {
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key == "MAIN_ADMIN")
        {
            mainAdmin = value;
        }
        else if (key == "ADMIN_PASSWORD")
        {
            adminPassword = value;
        }
        else if (key == "OPERATORS")
        {
            operators = split(value, ',');
        }
        else if (key == "VOICE_USERS")
        {
            voiceUsers = split(value, ',');
        }
        else if (key == "AUTO_OP_CHANNELS")
        {
            autoOpChannels = split(value, ',');
        }
        else if (key == "BANNED_USERS")
        {
            bannedUsers = split(value, ',');
        }
        else if (key == "OPER_PASSWORD")
        {
            operPassword = value;
        }
        else if (key == "MAX_USERS_DEFAULT")
        {
            maxUsersDefault = atoi(value.c_str());
        }
    }
    
    file.close();
    
    std::cout << "Admin config loaded: Main admin=" << mainAdmin << ", Operators=" << operators.size() << ", Voice users=" << voiceUsers.size() << ", Auto-op channels=" << autoOpChannels.size() << std::endl;
    
    return true;
}

/**
 * @brief Vérifie si un utilisateur est un opérateur.
 * 
 * Parcourt la liste des opérateurs définis dans la configuration et
 * retourne true si le pseudo fourni correspond à un opérateur.
 * 
 * @param nickname Le pseudo de l'utilisateur à vérifier.
 * @return true Si l'utilisateur est un opérateur.
 * @return false Sinon.
 */
bool AdminConfig::isOperator(const std::string& nickname) const
{
    for (size_t i = 0; i < operators.size(); i++)
    {
        if (operators[i] == nickname)
            return true;
    }

    if (nickname == mainAdmin)
        return true;

    return false;
}

/**
 * @brief Vérifie si un canal est configuré pour obtenir automatiquement les droits d'opérateur.
 * 
 * Parcourt la liste des canaux auto-op définis dans la configuration et
 * retourne true si le canal fourni correspond à un canal auto-op.
 * 
 * @param channel Le nom du canal à vérifier.
 * @return true Si le canal est auto-op.
 * @return false Sinon.
 */
bool AdminConfig::isAutoOpChannel(const std::string& channel) const
{
    for (size_t i = 0; i < autoOpChannels.size(); i++)
    {
        if (autoOpChannels[i] == channel)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Vérifie si un utilisateur est banni selon la configuration.
 * 
 * Parcourt la liste des utilisateurs bannis et retourne true si le
 * pseudo fourni correspond à un utilisateur banni.
 * 
 * @param nickname Le pseudo de l'utilisateur à vérifier.
 * @return true Si l'utilisateur est banni.
 * @return false Sinon.
 */
bool AdminConfig::isBannedUser(const std::string& nickname) const
{
    for (size_t i = 0; i < bannedUsers.size(); i++)
    {
        if (bannedUsers[i] == nickname)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Vérifie si un utilisateur a le droit de parler (voice) selon la configuration.
 * 
 * Parcourt la liste des utilisateurs avec privilège de voix et retourne true si
 * le pseudo fourni correspond à un utilisateur ayant le droit de parler.
 * 
 * @param nickname Le pseudo de l'utilisateur à vérifier.
 * @return true Si l'utilisateur a le droit de parler.
 * @return false Sinon.
 */
bool AdminConfig::isVoiceUser(const std::string& nickname) const
{
    for (size_t i = 0; i < voiceUsers.size(); i++)
    {
        if (voiceUsers[i] == nickname)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Retourne le grade d'un utilisateur selon la configuration.
 * 
 * Les grades sont définis comme suit :
 * - 0 : Administrateur principal
 * - 1 : Opérateur
 * - 2 : Utilisateur avec droit de parole (voice)
 * - 3 : Utilisateur normal
 * 
 * @param nickname Le pseudo de l'utilisateur.
 * @return int Le grade correspondant à l'utilisateur.
 */
int AdminConfig::getUserGrade(const std::string& nickname) const
{
    if (nickname == mainAdmin)
    {
        return 0;
    }
    else if (isOperator(nickname))
    {
        return 1;
    }
    else if (isVoiceUser(nickname))
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

/**
 * @brief Vérifie si un utilisateur possède les privilèges d'administrateur du serveur.
 * 
 * @param nickname Le pseudo de l'utilisateur à vérifier.
 * @return true Si l'utilisateur est l'administrateur principal.
 * @return false Sinon.
 */
bool AdminConfig::hasServerAdminPrivs(const std::string& nickname) const 
{
    return (nickname == mainAdmin);
}

/**
 * @brief Vérifie si un utilisateur possède les privilèges d'opérateur.
 * 
 * Un utilisateur a les privilèges d'opérateur si c'est l'administrateur principal
 * ou s'il figure dans la liste des opérateurs.
 * 
 * @param nickname Le pseudo de l'utilisateur à vérifier.
 * @return true Si l'utilisateur est l'administrateur principal ou un opérateur.
 * @return false Sinon.
 */
bool AdminConfig::hasOperatorPrivs(const std::string& nickname) const 
{
    return (nickname == mainAdmin || isOperator(nickname));
}

/**
 * @brief Valide les identifiants de l'administrateur principal.
 * 
 * Vérifie si le pseudo et le mot de passe fournis correspondent
 * à ceux de l'administrateur principal configuré.
 * 
 * @param nickname Le pseudo de l'utilisateur à valider.
 * @param password Le mot de passe fourni pour validation.
 * @return true Si le pseudo et le mot de passe correspondent à l'administrateur principal.
 * @return false Sinon.
 */
bool AdminConfig::validateAdminCredentials(const std::string& nickname, const std::string& password) const 
{
    return (nickname == mainAdmin && password == adminPassword);
}