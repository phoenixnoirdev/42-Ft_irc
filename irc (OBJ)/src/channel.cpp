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

# include "../inc/inc.hpp"

//=================
//Canonical Form
//=================

/**
 * @brief Constructeur par défaut de la classe Channel.
 *
 * Initialise l'ID, le nom et le grade de visibilité à 0 ou vide.
 */
Channel::Channel(): _id(0), _name(""), _view_grade(0) {}

Channel::Channel(int id, std::string name): _view_grade(0) 
{
	this->_id = id;
	this->_name = name;
}

/**
 * @brief Constructeur de Channel avec ID et nom spécifiés.
 *
 * - Initialise l'ID et le nom du canal.
 * - Initialise le grade de visibilité à 0 par défaut.
 *
 * @param id Identifiant du canal.
 * @param name Nom du canal.
 */
Channel::Channel(const Channel& other)
{
	this->_id = other._id;
	this->_name = other._name;
	this->_view_grade = other._view_grade;
	this->_grade_0 = other._grade_0;
	this->_grade_1 = other._grade_1;
	this->_grade_2 = other._grade_2;
	this->_grade_3 = other._grade_3;
	this->_ban = other._ban;
}

/**
 * @brief Surcharge de l'opérateur d'affectation pour Channel.
 *
 * - Copie tous les attributs d'un autre objet Channel.
 * - Vérifie l'auto-affectation pour éviter les erreurs.
 *
 * @param other Objet Channel source à copier.
 * @return Référence sur l'objet courant après copie.
 */
Channel&	Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		this->_id = other._id;
		this->_name = other._name;
		this->_view_grade = other._view_grade;
		this->_grade_0 = other._grade_0;
		this->_grade_1 = other._grade_1;
		this->_grade_2 = other._grade_2;
		this->_grade_3 = other._grade_3;
		this->_ban = other._ban;
	}
	return (*this);
}

/**
 * @brief Destructeur de la classe Channel.
 *
 * - Vide les listes de grades et de bannissements.
 */
Channel::~Channel()
{
	this->_grade_0.clear();
    this->_grade_1.clear();
    this->_grade_2.clear();
    this->_grade_3.clear();
    this->_ban.clear();
}


//=================
//Fonction public
//=================

/**
 * @brief Définit l'identifiant du canal.
 *
 * @param i Nouvel identifiant à attribuer.
 */
void Channel::SetId(int i)
{
	this->_id = i;
}

/**
 * @brief Retourne l'identifiant actuel du canal.
 *
 * @return ID du canal.
 */
int Channel::GetId() const
{
	return this->_id;
}


/**
 * @brief Définit le nom du canal.
 *
 * @param str Nouveau nom du canal.
 */
void Channel::SetName(const std::string& str)
{
	this->_name = str;
}

/**
 * @brief Retourne le nom actuel du canal.
 *
 * @return Nom du canal.
 */
std::string Channel::GetName() const
{
	return this->_name;
}


/**
 * @brief Définit le grade minimal requis pour voir le canal.
 *
 * @param i Nouveau grade de visibilité.
 */
void Channel::SetViewGrad(int i)
{
	this->_view_grade = i;
}

/**
 * @brief Retourne le grade minimal requis pour voir le canal.
 *
 * @return Grade de visibilité du canal.
 */

int Channel::GetViewGradd() const
{
	return this->_view_grade;
}


/**
 * @brief Ajoute un utilisateur au canal selon son grade.
 *
 * - Classe l'utilisateur dans la map correspondant à son grade (0 à 3).
 *
 * @param user Référence à l'utilisateur à ajouter.
 */
void Channel::AddUser(const User& user)
{
	if (user.getGrade() == 0)
		this->_grade_0.insert(std::make_pair(user.getSocket(), user));
	
	if (user.getGrade() == 1)
		this->_grade_1.insert(std::make_pair(user.getSocket(), user));
	
	if (user.getGrade() == 2)
		this->_grade_2.insert(std::make_pair(user.getSocket(), user));

	if (user.getGrade() == 3)
		this->_grade_3.insert(std::make_pair(user.getSocket(), user));
}

/**
 * @brief Diffuse un message de type JOIN à tous les utilisateurs du canal.
 *
 * - Parcourt toutes les maps de grades (0 à 3) et envoie le message à chaque utilisateur.
 *
 * @param msg Message à diffuser aux utilisateurs.
 */
void Channel::BroadcastJoin(const std::string &msg)
{
	for (std::map<int, User>::iterator it = this->_grade_0.begin(); it != this->_grade_0.end(); ++it)
	{
		send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); ++it) 
	{
		send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); ++it) 
	{
		send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); ++it) 
	{
		send(it->first, msg.c_str(), msg.size(), 0);
	}
}

/**
 * @brief Diffuse un message à tous les utilisateurs du canal sauf l'expéditeur.
 *
 * - Parcourt toutes les maps de grades (0 à 3) et envoie le message à chaque utilisateur.
 * - Ignore le socket de l'utilisateur qui a envoyé le message.
 *
 * @param msg Message à diffuser.
 * @param sender Descripteur de socket de l'expéditeur à exclure.
 */
void Channel::Broadcast(const std::string &msg, int sender)
{
    for (std::map<int, User>::iterator it = this->_grade_0.begin(); it != this->_grade_0.end(); ++it)
	{
		if (it->second.getSocket() != sender)
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_1.begin(); it != this->_grade_1.end(); ++it) 
	{
		if (it->second.getSocket() != sender)
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_2.begin(); it != this->_grade_2.end(); ++it) 
	{
		if (it->second.getSocket() != sender)
        	send(it->first, msg.c_str(), msg.size(), 0);
	}

	for (std::map<int, User>::iterator it = this->_grade_3.begin(); it != this->_grade_3.end(); ++it) 
	{
		if (it->second.getSocket() != sender)
        	send(it->first, msg.c_str(), msg.size(), 0);
	}
}