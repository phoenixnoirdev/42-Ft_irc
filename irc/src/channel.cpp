/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42.lu>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 08:36:42 by kelevequ          #+#    #+#             */
/*   Updated: 2025/08/21 09:51:34 by kelevequ         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/inc.hpp"

//=================
//Canonical Form
//=================

/**
 * @brief Constructeur par défaut de la classe Channel.
 *
 * Initialise un objet Channel vide, avec une liste chaînée vide (_top et _bot à NULL)
 * et une taille (_size) égale à 0.
 */
Channel::Channel(): _top(NULL), _bot(NULL), _size(0) {}

/**
 * @brief Constructeur de copie pour la classe Channel.
 *
 * Crée un nouvel objet Channel en dupliquant tous les channels
 * de l'objet source. Chaque channel est copié individuellement
 * et ajouté à la liste chaînée de l'objet courant.
 *
 * @param other L'objet Channel source à copier.
 */
Channel::Channel(const Channel& other): _top(NULL), _bot(NULL), _size(0)
{
	t_channel	*tmp;
	size_t		len;

	len = other.getSize();
	tmp = other.getTop();
	while (_size < len && tmp != NULL)
	{
		addChannel(*tmp);
		tmp = tmp->next;
		_size++;
	}
}

/**
 * @brief Opérateur d'affectation pour la classe Channel.
 *
 * Copie la liste chaînée de channels depuis un autre objet Channel.
 * Chaque channels est dupliqué et ajouté à la liste de l'objet courant.
 *
 * @param other L'objet Channel source à copier.
 * @return Référence à l'objet Channel courant après copie.
 */
Channel&	Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		t_channel	*tmp;
		size_t		len;

		len = other.getSize();
		tmp = other.getTop();
		_size = 0;
		while (_size < len && tmp != NULL)
		{
			addChannel(*tmp);
			tmp = tmp->next;
			_size++;
		}
	}
	return (*this);
}

/**
 * @brief Destructeur de la classe User.
 *
 * Libère toute la mémoire allouée pour la liste chaînée d'utilisateurs
 * en appelant la fonction clear().
 */
Channel::~Channel()
{
	clear();
}

//=================
//Constructeur
//=================

/**
 * @brief Constructeur de Channel à partir d'une liste chaînée existante.
 *
 * Initialise la liste de channels vide puis copie chaque channels
 * de la liste fournie dans la nouvelle instance.
 *
 * @param list Pointeur vers le premier élément d'une liste chaînée de t_channel.
 */
Channel::Channel(t_channel *list): _top(NULL), _bot(NULL), _size(0)
{
	t_channel	*tmp;

	tmp = list;
	while (tmp != NULL)
	{
		addChannel(*tmp);
		tmp = tmp->next;
		_size++;
	}
}

/**
 * @brief Constructeur de User à partir d'une chaîne JSON.
 *
 * Initialise la liste chaînée de channels vide (_top et _bot à NULL,
 * _size à 0). Actuellement, le paramètre json n'est pas utilisé.
 *
 * @param json Chaîne JSON supposée contenir des données de channels.
 */
Channel::Channel(const std::string& json): _top(NULL), _bot(NULL), _size(0)
{
	(void)json;
}

//===============
//Fonctions public
//===============

/**
 * @brief Crée et ajoute un nouveau channel à la liste.
 *
 * Recois un id qui devrait etre creer a partir d'un compte global du nombre
 * de channels fait au total puis crée un objet t_channel avec les infos fournies.
 * Le channel est ajouté à la fin de la liste chaînée. Les pointeurs
 * _top et _bot ainsi que le compteur _size sont mis à jour.
 * Le createur du channel est aussi passer en parametre pour ajouter un membre.
 *
 * @param id    ID unique du channel.
 * @param name  Nom du channel.
 * @param grade Grade requis pour voir channel.
 * @param owner Createur du channel.
 */
void	Channel::addChannel(
	int id,
	const std::string& name,
	int grade,
	t_user *owner)
{
	t_channel	*tmp = new t_channel(id, name, grade);

	if (empty())
		_top = tmp;
	else
		_bot->next = tmp;
	_bot = tmp;
	_size++;
	addUserChannel(tmp, owner, 0);
}

/**
 * @brief Ajoute un nouveau channel à la liste.
 *
 * Crée une copie dynamique du channel fourni et l'ajoute
 * à la fin de la liste chaînée. Met à jour les pointeurs _top et _bot
 * ainsi que le compteur _size.
 *
 * @param other Channel à ajouter à la liste.
 */
void	Channel::addChannel(const t_channel& other)
{
	t_channel	*tmp = new t_channel(other);

	if (empty())
		_top = tmp;
	else
		_bot->next = tmp;
	_bot = tmp;
	_size++;
}

/**
 * @brief Ajoute un utilisateur au channel.
 *
 * Verifie que l'utilisateur et le channel existe, et que l'utilisateur
 * n'est pas deja dans le channel ou est ban du channel.
 * Puis rajoute l'utilisateur dans les maps approprier en fonction du grade.
 * Si l'utilisateur existe deja dans le channel, utiliser les 3 fonctions suivantes:
 * promoteChannelUser(t_channel *channel, t_user *user, int grade),
 * demoteChannelUser(t_channel *channel, t_user *user, int grade),
 * banChannelUser(t_channel *channel, t_user *user, const string& reason);
 * pour modifier les permissions.
 *
 * @param channel Le channel dans lequel l'utilisateur va etre rajouter.
 * @param user    L'utilisateur qui va etre rajouter au channel.
 * @param grade   Le grade de l'utilisateur dans le channel.
 * @return true si l'utilisateur a etait ajouter, sinon false.
 *
 * Il faut verifier que le t_user existe dans la userDB avant d'appeler la fonction.
 * Et bien faire attention a utiliser un pointeur retourner pas la userDB et non une copie.
 */
bool	Channel::addUserChannel(t_channel *channel, t_user *user, int grade)
{
	if (user == NULL || channel == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	if (isChannelUserBan(channel, user))
	{
		std::cerr << user->name << " is banned from " << channel->name << std::endl;
		return (false);
	}
	if (channelHasUser(channel, user, 3))
	{
		std::cerr << user->name << " already exists in " << channel->name  << std::endl;
		return (false);
	}
	switch (grade)
	{
		case 0:
			channel->grade_0[user] = "owner";
			//fallthrough
		case 1:
			channel->grade_1[user] = "admin";
			//fallthrough
		case 2:
			channel->grade_2[user] = "user";
			//fallthrough
		case 3:
			channel->grade_3[user] = "default";
			break ;
		default:
			std::cerr << "Invalid grade: " << grade << std::endl;
			return (false);
	}
	return (true);
}

/**
 * @brief Supprime tous les channels de la liste.
 *
 * Cette fonction parcourt la liste chaînée des channels et
 * libère la mémoire de chaque élément. Après l'exécution, la
 * liste est vide, _top et _bot sont mis à NULL, et _size est 0.
 */
void	Channel::clear()
{
	t_channel	*tmp;

	if (empty())
		return ;
	while (_size > 0)
	{
		tmp = _top;
		_top = _top->next;
		delete tmp;
		_size--;
	}
	_bot = NULL;
}

/**
 * @brief Supprime un channel de la liste en fonction de son ID.
 *
 * Cette fonction recherche le channel correspondant à l'ID fourni
 * dans la liste chaînée. Si le channel est trouvé, il est retiré
 * de la liste et sa mémoire est libérée. La taille de la liste (_size)
 * est décrémentée.
 *
 * Cas particuliers gérés :
 * - Le channel est en tête (_top) de la liste.
 * - Le channel est en queue (_bot) de la liste.
 * - Le channel est au milieu de la liste.
 *
 * @param id L'ID du channel à supprimer.
 * @return true si un channel a été supprimé, false si non.
 */
bool	Channel::removeChannelByID(int id)
{
	t_channel	*tmp;
	t_channel	*prev;
	size_t		i = 0;

	tmp = getChannelByID(id, &i);
	if (!tmp)
		return (false);
	if (tmp == _top)
	{
		_top = tmp->next;
	}
	else if (tmp == _bot)
	{
		prev = getChannelByIndex(i - 1);
		prev->next = NULL;
		_bot = prev;
	}
	else
	{
		prev = getChannelByIndex(i - 1);
		prev->next = tmp->next;
	}
	delete tmp;
	_size--;
	return (true);
}

/**
 * @brief Supprime un channel de la liste en fonction de son nom.
 *
 * Cette fonction recherche le channel correspondant au nom fourni
 * dans la liste chaînée. Si le channel est trouvé, il est retiré
 * de la liste et sa mémoire est libérée. La taille de la liste (_size)
 * est décrémentée.
 *
 * Cas particuliers gérés :
 * - Le channel est en tête (_top) de la liste.
 * - Le channel est en queue (_bot) de la liste.
 * - Le channel est au milieu de la liste.
 *
 * @param name Le nom du channel à supprimer.
 * @return true si un channel a été supprimé, false si non.
 */
bool	Channel::removeChannelByName(const std::string& name)
{
	t_channel	*tmp;
	t_channel	*prev;
	size_t		i = 0;

	tmp = getChannelByName(name, &i);
	if (!tmp)
		return (false);
	if (tmp == _top)
	{
		_top = tmp->next;
	}
	else if (tmp == _bot)
	{
		prev = getChannelByIndex(i - 1);
		prev->next = NULL;
		_bot = prev;
	}
	else
	{
		prev = getChannelByIndex(i - 1);
		prev->next = tmp->next;
	}
	delete tmp;
	_size--;
	return (true);
}

/**
 * @brief Retourne le nombre total de channels.
 *
 * Cette fonction renvoie la taille actuelle de la liste de channels
 * contenue dans l'objet Channel.
 *
 * @return Nombre de channels (_size).
 */
size_t	Channel::getSize() const
{
	return (_size);
}

/**
 * @brief Retourne le premier channel de la liste.
 *
 * Cette fonction renvoie un pointeur vers la structure t_channel
 * correspondant au premier channel stocké dans l'objet Channel.
 *
 * @return Pointeur vers le premier channel, ou NULL si la liste est vide.
 */
t_channel	*Channel::getTop() const
{
	return (_top);
}

/**
 * @brief Retourne le bot associé au channel.
 *
 * Cette fonction renvoie un pointeur vers la structure t_channel
 * correspondant au bot géré par cette instance.
 *
 * @return Pointeur vers le bot, ou NULL si aucun bot n'est défini.
 */
t_channel	*Channel::getBot() const
{
	return (_bot);
}

/**
 * @brief Récupère un channel par son ID dans la liste.
 *
 * Parcourt la liste chaînée des channels et retourne
 * le pointeur vers le channel avec l'ID spécifié.
 *
 * @param id L'id du channel a retrouver.
 * @return Pointeur vers le channel si l'id est trouver,  
 *         `NULL` sinon.
 */
t_channel	*Channel::getChannelByID(int id) const
{
	t_channel	*tmp;
	size_t		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (id == tmp->id)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}

/**
 * @brief Récupère un channel par son ID dans la liste.
 *
 * Parcourt la liste chaînée des channels et retourne
 * le pointeur vers le channel avec l'ID spécifié.
 *
 * @param id L'id du channel a retrouver.
 * @param i  L'index commencant a 0 qui sera incrementer a la position 
 *           dans la liste ou le t_channel a ete trouver.
 * @return Pointeur vers le channel si l'id est trouver,  
 *         `NULL` sinon.
 */
t_channel	*Channel::getChannelByID(int id, size_t *i) const
{
	t_channel	*tmp;

	tmp = getTop();
	while (*i < _size)
	{
		if (id == tmp->id)
			return (tmp);
		tmp = tmp->next;
		(*i)++;
	}
	return (NULL);
}

/**
 * @brief Récupère un channel par son index dans la liste.
 *
 * Parcourt la liste chaînée des channels et retourne
 * le pointeur vers le channel situé à l'index spécifié.
 *
 * @param index Position du channel dans la liste (0-based).
 * @return Pointeur vers le channel si l'index est valide,  
 *         `NULL` sinon.
 */
t_channel	*Channel::getChannelByIndex(size_t index) const
{
	t_channel	*tmp;
	size_t		i;

	if (index >= _size)
		return (NULL);
	tmp = getTop();
	if (index == 0)
		return (tmp);
	i = 1;
	while (i <= index)
	{
		tmp = tmp->next;
		i++;
	}
	return (tmp);
}

/**
 * @brief Récupère un channel par son nom dans la liste.
 *
 * Parcourt la liste chaînée des channels et retourne
 * le pointeur vers le channel avec le nom spécifié.
 *
 * @param name Le nom du channel a retrouver.
 * @return Pointeur vers le channel si le nom est trouver,  
 *         `NULL` sinon.
 */
t_channel	*Channel::getChannelByName(const std::string& name) const
{
	t_channel	*tmp;
	size_t		i;

	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (name == tmp->name)
			return (tmp);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}

/**
 * @brief Récupère un channel par son nom dans la liste.
 *
 * Parcourt la liste chaînée des channels et retourne
 * le pointeur vers le channel avec le nom spécifié.
 *
 * @param name Le nom du channel a retrouver.
 * @param i  L'index commencant a 0 qui sera incrementer a la position 
 *           dans la liste ou le t_channel a ete trouver.
 * @return Pointeur vers le channel si le nom est trouver,  
 *         `NULL` sinon.
 */
t_channel	*Channel::getChannelByName(const std::string& name, size_t *i) const
{
	t_channel	*tmp;

	tmp = getTop();
	while (*i < _size)
	{
		if (name == tmp->name)
			return (tmp);
		tmp = tmp->next;
		(*i)++;
	}
	return (NULL);
}

/**
 * @brief Recupere le surnom de l'utilisateur dans le channel specifier.
 *
 * Verifie si le surnom est different de default puis renvoie le surnom approprier
 * de l'utilisateur dans le channel demander.
 * L'acces d'un std::map avec la methode ci dessous est safe.
 *
 * @param channel Le channel ou est situer l'utilisateur.
 * @param user    L'utilisateur dont on veut recuperer le surnom.
 * @return Si == 'default' alors le nom de l'utilisateur dans s_user,
 *         Si != 'default' alors le surnom de l'utilisateur dans s_channel.
 */
std::string&	Channel::getChannelUserDisplayName(t_channel *channel, t_user *user) const
{
	if (channel->grade_3[user] != "default")
		return (channel->grade_3[user]);
	else
		return (user->name);
}

/**
 * @brief Modifie le surnom de l'utilisateur dans le channel specifier.
 *
 * Verifie si le channel et l'utilisateur existe, que le surnom donner est valid.
 * Invalide == admin, owner, user ou default.
 * Puis modifie le surnom de l'utilisateur dans le channel. Stocker dans grade_3.
 *
 * @param channel Le channel ou est situer l'utilisateur.
 * @param user    L'utilisateur dont on veut modifier le surnom.
 * @return        Si le nom a ete modifier avec succes.
 */
bool	Channel::setChannelUserDisplayName(t_channel *channel, t_user *user, const std::string& displayName)
{
	if (channel == NULL || user == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	if (displayName == "admin" || displayName == "owner" || displayName == "user" || displayName == "default")
	{
		std::cerr << "Invalid Display Name" << std::endl;
		return (false);
	}
	channel->grade_3[user] = displayName;
	return (true);
}

/**
 * @brief Rajoute des permissions a l'utilisateur dans le channel en fonction du grade donner.
 *
 * Verifie si le channel et user existe, que l'utilisateur n'a pas deja le grade demander,
 * puis rajoute l'utilisateur dans tous les grades sauf grade_3.
 * L'utilisateur doit exister dans le channel avant de pouvoir etre promu, cette verification
 * doit se faire avant d'appeler la fonction.
 *
 * @param channel Le channel ou est situer l'utilisateur.
 * @param user    L'utilisateur dont on veut modifier le grade.
 * @param grade   Nouveau grade de l'utilisateur dans le channel.
 * @return        Si le grade de l'utilisateur dans le channel a ete modifier avec succes.
 */
bool	Channel::promoteChannelUser(t_channel *channel, t_user *user, int grade)
{
	if (channel == NULL || user == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	if (channelHasUser(channel, user, grade))
	{
		std::cerr << user->name << " already exists in " << channel->name << " as grade " << grade << std::endl;
		return (false);
	}
	switch (grade)
	{
		case 0:
			channel->grade_0[user] = "owner";
			//fallthrough
		case 1:
			channel->grade_1[user] = "admin";
			//fallthrough
		case 2:
			channel->grade_2[user] = "user";
			//fallthrough
			break ;
		default:
			std::cerr << "Invalid grade: " << grade << std::endl;
			return (false);
	}
	return (true);
}

/**
 * @brief Enleve des permissions a l'utilisateur dans le channel en fonction du grade donner.
 *
 * Verifie si le channel et user existe,
 * puis enleve l'utilisateur de tous les grades sauf grade_3.
 * L'utilisateur doit exister dans le channel avant de pouvoir etre demoter, cette verification
 * doit se faire avant d'appeler la fonction.
 * Cette fonction ne peut pas enlever l'utilisateur du channel. Elle ne peut que le mute.
 *
 * @param channel Le channel ou est situer l'utilisateur.
 * @param user    L'utilisateur dont on veut modifier le grade.
 * @param grade   Nouveau grade de l'utilisateur dans le channel.
 * @return        Si le grade de l'utilisateur dans le channel a ete modifier avec succes.
 */
bool	Channel::demoteChannelUser(t_channel *channel, t_user *user, int grade)
{
	if (channel == NULL || user == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	switch (grade)
	{
		case 3:
			channel->grade_2.erase(user);
			//fallthrough
		case 2:
			channel->grade_1.erase(user);
			//fallthrough
		case 1:
			channel->grade_0.erase(user);
			break ;
		default:
			std::cerr << "Invalid grade: " << grade << std::endl;
			return (false);
	}
	return (true);
}

/**
 * @brief Banni l'utilisateur du channel.
 *
 * Verifie si le channel et user existe,
 * puis enleve l'utilisateur de tous les grades et le rajoute dans la liste des user bannis.
 * L'utilisateur n'a pas besoin d'exister dans le channel pour etre banni cette fois-ci.
 * Par contre il faut tout de meme verifier que l'utilisateur existe dans la userDB avant
 * d'appeler cette fonction.
 *
 * @param channel Le channel ou est situer l'utilisateur.
 * @param user    L'utilisateur dont on veut bannir du channel.
 * @param reason  Raison du ban, qui va etre sauvegarder dans channel->ban[user]
 * @return        Si l'utilisateur dans le channel a ete banni avec succes.
 */
bool	Channel::banChannelUser(t_channel *channel, t_user *user, const std::string& reason)
{
	if (channel == NULL || user == NULL)
	{
		std::cerr << "Missing user/channel" << std::endl;
		return (false);
	}
	channel->grade_0.erase(user); //It's safe, even in c++98
	channel->grade_1.erase(user);
	channel->grade_2.erase(user);
	channel->grade_3.erase(user);
	channel->ban[user] = reason;
	return (true);
}

/**
 * @brief Vérifie si la liste des channels est vide.
 *
 * Cette fonction contrôle si le pointeur vers le premier
 * élément de la liste (`_top`) est nul.
 *
 * @return `true` si la liste ne contient aucun channel,  
 *         `false` sinon.
 */
bool	Channel::empty() const
{
	return (_top == NULL);
}

/**
 * @brief Vérifie si un ID existe dans la liste des channels.
 *
 * Cette fonction parcourt la liste chaînée interne des channels
 * et compare l'ID fourni avec ceux enregistrés.  
 * Si une correspondance est trouvée, la fonction retourne `true`.
 *
 * @param id ID unique du channel à rechercher.
 *
 * @return `true` si l'ID est trouvé,  
 *         `false` sinon ou si la liste est vide.
 *
 * @note La recherche est linéaire (O(n)) car elle parcourt la liste
 *       complète des channels.
 */
bool	Channel::hasID(int id) const
{
	t_channel	*tmp;
	size_t		i;

	if (empty())
		return (false);
	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (id == tmp->id)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}

/**
 * @brief Vérifie si un nom existe dans la liste des channels.
 *
 * Cette fonction parcourt la liste chaînée interne des channels
 * et compare le nom fourni avec ceux enregistrés.  
 * Si une correspondance est trouvée, la fonction retourne `true`.
 *
 * @param name Nom du channel à rechercher.
 *
 * @return `true` si le nom est trouvé,  
 *         `false` sinon ou si la liste est vide.
 *
 * @note La recherche est linéaire (O(n)) car elle parcourt la liste
 *       complète des channels.
 */
bool	Channel::hasName(const std::string& name) const
{
	t_channel	*tmp;
	size_t		i;

	if (empty())
		return (false);
	tmp = getTop();
	i = 0;
	while (i < _size)
	{
		if (name == tmp->name)
			return (true);
		tmp = tmp->next;
		i++;
	}
	return (false);
}

/**
 * @brief Vérifie si un utilisateur existe dans le channel au grade demander.
 *
 * Cette fonction cherche dans le channel fourni au grade fourni pour
 * l'utilisateur fourni.
 * Si une correspondance est trouvée, la fonction retourne `true`.
 *
 * @param channel Channel de l'utilisateur.
 * @param user    Utilisateur dont on veut confirmer l'existance.
 * @param grade   Grade dans lequel verifier pour l'utilisateur.
 *
 * @return `true` si l'utilisateur est trouvé,  
 *         `false` sinon.
 */
bool	Channel::channelHasUser(t_channel *channel, t_user *user, int grade) const
{
	if (grade > 3 || grade < 0)
		return (false);
	std::map<s_user*, std::string>::iterator	it;
	switch (grade)
	{
		case 0:
			it = channel->grade_0.find(user);
			return (it != channel->grade_0.end());
		case 1:
			it = channel->grade_1.find(user);
			return (it != channel->grade_1.end());
		case 2:
			it = channel->grade_2.find(user);
			return (it != channel->grade_2.end());
		case 3:
			it = channel->grade_3.find(user);
			return (it != channel->grade_3.end());
		default:
			std::cerr << "Invalid grade: " << grade << std::endl;
			return (false);
	}
}

/**
 * @brief Vérifie si un utilisateur a ete banni du channel.
 *
 * Cette fonction cherche dans le channel fourni si l'utilisateur fourni a ete banni.
 * Si une correspondance est trouvée, la fonction retourne `true`.
 *
 * @param channel Channel de l'utilisateur.
 * @param user    Utilisateur dont on veut confirmer la deploration.
 *
 * @return `true` si l'utilisateur est banni,  
 *         `false` sinon.
 */
bool	Channel::isChannelUserBan(t_channel *channel, t_user *user) const
{
	std::map<s_user*, std::string>::iterator	it = channel->ban.find(user);
	return (it != channel->ban.end());
}