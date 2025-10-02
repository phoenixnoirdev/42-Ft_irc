/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdFile.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42luxembourg>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-09-29 16:29:50 by kelevequ          #+#    #+#             */
/*   Updated: 2025-09-29 16:29:50 by kelevequ         ###   ########.lu       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include "../inc/inc.hpp"

/*void	Server::handleFileList(int clientSocket, User& user, const std::string& line)
{
	std::map<std::string, File>::iterator	it;
	std::string								msg;
	File									tmp;

	msg = ":" + this->_ServName + " NOTICE " + user.getNick() + ;
	for (it = _Files.begin(); it != _Files.end(); it++)
	{
		tmp = it->second;
	}
}*/

void	Server::_msgUser(User *target, User& user, std::string filename)
{
	std::string	ircMsg;

	ircMsg = ":" + user.getNick() + "!~" + user.getName() + "@localhost PRIVMSG " + target->getNick() + " : " + user.getNick() + " is trying to send you a file called \"" + filename + "\"\n";
	send(target->getSocket(), ircMsg.c_str(), ircMsg.size(), 0);
}

void	Server::handleFileSend(User& user, const std::string& line)
{
	std::string	tmp = line.substr(6); // after "FSEND "

	size_t	i = 0;
	while (i < tmp.size() && tmp[i] != ' ')
		i++;
	size_t	k = 0;
	while (i + k < tmp.size() && tmp[i + k] == ' ')
		k++;
	if (i == 0)
	{	//No target user
		std::cout << "FSEND: Missing target user" << std::endl;
		return ;
	}
	std::string	targetNick = tmp.substr(0, i); //Nick without spaces
	i += k;
	size_t	j = 0;
	while (i + j < tmp.size() && tmp[i + j] != ' ')
		j++;
	k = i;
	while (j + k < tmp.size() && tmp[j + k] == ' ')
		k++;
	if (j == 0)
	{	//No target file
		std::cout << "FSEND: Missing target file" << std::endl;
		return ;
	}
	std::string	targetFile = tmp.substr(i, j); //File without spaces

	if (j + k != tmp.size())
	{	//too many arguments or simply ignore extra parts :shrug:
		std::cout << "FSEND: Too many arguments" << std::endl;
		return ;
	}

	User	*target;
	for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end(); it++)
	{
		if (targetNick.compare(it->second.getNick()) == 0)
		{
			target = &it->second;
			break ;
		}
	}

	if (it == this->_User.end())
	{	//User doesn't exist
		std::cout << "FSEND: Target user not found" << std::endl;
		return ;
	}
	std::fstream	ifs(targetFile.c_str(), std::fstream::in);
	if (ifs.fail())
	{	//Invalid path
		std::cout << "FSEND: Invalid path" << std::endl;
		return ;
	}
	std::string filename = targetFile.substr(targetFile.find_last_of('/') + 1);
	File	file(filename, targetFile, user.getNick(), targetNick);
	if (this->_Files.find(filename) != this->_Files.end())
	{	//File already exists by that name
		std::cout << "FSEND: File already exists" << std::endl;
		return ;
	}
	this->_Files.insert(std::pair<std::string, File>(filename, file));
	_msgUser(target, user, filename);
}

void	Server::handleFileGet(User& user, const std::string& line)
{
	std::string	tmp = line.substr(5); // after "FGET "

	size_t	i = 0;
	while (i < tmp.size() && tmp[i] != ' ')
		i++;
	size_t	k = 0;
	while (i + k < tmp.size() && tmp[i + k] == ' ')
		k++;
	if (i == 0)
	{	//No target file
		std::cout << "FGET: Missing target file" << std::endl;
		return ;
	}
	std::string	targetFile = tmp.substr(0, i); //File without spaces
	i += k;
	size_t	j = 0;
	while (i + j < tmp.size() && tmp[i + j] != ' ')
		j++;
	k = i;
	while (j + k < tmp.size() && tmp[j + k] == ' ')
		k++;
	std::string	targetDir = (k == 0) ? "./" : tmp.substr(i, k) + "/"; //Optional Dir without spaces

	if (this->_Files.find(targetFile) == this->_Files.end())
	{	//Target file doesn't exist
		std::cout << "FGET: Couldn't find target file" << std::endl;
		return ;
	}
	File	file(this->_Files.find(targetFile)->second);
	if (file.getCaptor() != user.getNick())
	{	//File exists but is not meant for this user
		std::cout << "FGET: No permission to download this file" << std::endl;
		return ;
	}

	std::string		full_path(targetDir + targetFile);
	std::fstream	ofs(full_path.c_str(), std::fstream::out);
	std::fstream	ifs(file.getFileDir().c_str(), std::fstream::in);
	if (ofs.is_open())
		ofs << ifs.rdbuf();
	if (ofs)
	{
		this->_Files.erase(file.getFileName());
		std::cout << "FGET: Succesfull, deleting _Files entry" << std::endl;
	}
	else
	{	//File didn't get written
		std::cout << "FGET: Error while outputting file" << std::endl;
		return ;
	}
}
/*
void	Server::handleFileUpload(int clientSocket, User& user, const std::string& line)
{

}

void	Server::handleFileDownload(int clientSocket, User& user, const std::string& line)
{

}
*/