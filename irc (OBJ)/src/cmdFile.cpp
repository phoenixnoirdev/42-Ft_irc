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
	char 		opt = 'd';
	File		file;

	size_t	i = 0;

	//0: Skip spaces
	while (i < tmp.size() && tmp[i] == ' ')
		i++;
	//1.0: Check for option
	if (tmp[i] == '-')
	{
		i++;
		//1.1: Check option format
		if ((i < tmp.size() && tmp[i] == ' ') || (i + 1 < tmp.size() && tmp[i + 1] == ' '))
		{	//There should be only 1 letter not multiple
			std::cout << "FSEND: Missing or multiple options" << std::endl;
			return ;
		}
		//1.2: Check option content
		else if (tmp[i] != 'p' || tmp[i] != 'c' || tmp[i] != 'g')
		{	//The valid options are p for private, c for channel, g for global
			std::cout << "FSEND: Invalid option" << std::endl;
			return ;
		}
		//1.3: Save option
		else
			opt = tmp[i];

		//1.4: Skip spaces
		while (i < tmp.size() && tmp[i] == ' ')
			i++;
	}

	//2.0: Parse args
	User	*target = NULL;
	Channel	*chan = NULL;
	switch(opt)
	{
		case 'g':
			//2a.0: Update global
			file.setGlobal(1);
			break ;
		case 'c':
		{
			//2b.0: Parse channel
			size_t	k = 0;
			while (i + k < tmp.size() && tmp[i + k] != ' ')
				k++;
			//2b.1: Was there channel?
			if (k == 0)
			{	//No target channel
				std::cout << "FSEND: Missing target channel" << std::endl;
				return ;
			}
			//2b.2: Save target channel
			std::string	targetChan = tmp.substr(i, k); //Channel without spaces

			//2b.3: Find channel in database
			for (std::map<int, Channel>::iterator it = this->_Chan.begin(); it != this->_Chan.end(); it++)
			{
				if (targetChan.compare(it->second.GetName()) == 0)
				{
					chan = &it->second;
					break ;
				}
			}
			//2b.4: Was channel found?
			if (chan == NULL)
			{	//Channel doesn't exist
				std::cout << "FSEND: Target channel not found" << std::endl;
				return ;
			}

			//2b.5: Save target channel
			file.setChannel(targetChan);

			//2b.6: Skip space
			i += k;
			while (i < tmp.size() && tmp[i] == ' ')
				i++;
			break ;
		}
		default:
		{
			//2c.0: Parse user
			size_t	k = 0;
			while (i + k < tmp.size() && tmp[i + k] != ' ')
				k++;
			//2c.1: Was there user?
			if (k == 0)
			{	//No target user
				std::cout << "FSEND: Missing target user" << std::endl;
				return ;
			}
			//2c.2: Save target user
			std::string	targetNick = tmp.substr(i, k); //Nick without spaces

			//2c.3: Find user in database
			for (std::map<int, User>::iterator it = this->_User.begin(); it != this->_User.end(); it++)
			{
				if (targetNick.compare(it->second.getNick()) == 0)
				{
					target = &it->second;
					break ;
				}
			}
			//2c.4: Was user found?
			if (target == NULL)
			{	//User doesn't exist
				std::cout << "FSEND: Target user not found" << std::endl;
				return ;
			}

			//2c.5: Save target user
			file.setCaptor(targetNick);

			//2c.6: Skip space
			i += k;
			while (i < tmp.size() && tmp[i] == ' ')
				i++;
		}
	}
	tmp = tmp.substr(i);

	//3.0: Parse file path
	i = 0;
	while (i < tmp.size() && tmp[i] != ' ')
		i++;

	//3.1: Was there a file?
	if (i == 0)
	{	//No target file
		std::cout << "FSEND: Missing target file" << std::endl;
		return ;
	}
	//3.2: Save target file
	std::string	targetFile = tmp.substr(0, i); //File without spaces
	
	//3.3: Skip end spaces
	while (i < tmp.size() && tmp[i] == ' ')
		i++;
	//3.4
	if (i != tmp.size()) //if j + k not the end of line
	{	//too many arguments or simply ignore extra parts :shrug:
		std::cout << "FSEND: Too many arguments" << std::endl;
		return ;
	}

	//4.0: Check file path/permissions
	std::fstream	ifs(targetFile.c_str(), std::fstream::in);
	if (ifs.fail())
	{	//Invalid path
		std::cout << "FSEND: Invalid path" << std::endl;
		return ;
	}
	//4.1: Extract file name
	std::string filename = targetFile.substr(targetFile.find_last_of('/') + 1);

	//5.0: Update file object
	file.setFileName(filename);
	file.setFileDir(targetFile);
	file.setSender(user.getNick());
	//5.1: Verify filename is unique
	if (this->_Files.find(filename) != this->_Files.end())
	{	//File already exists by that name
		std::cout << "FSEND: File already exists" << std::endl;
		return ;
	}
	//5.2: Insert file
	this->_Files.insert(std::pair<std::string, File>(filename, file));
	//5.3: Inform target user
	_msgUser(target, user, filename);
}

void	Server::handleFileGet(User& user, const std::string& line)
{
	std::string	tmp = line.substr(5); // after "FGET "

	//1.0: Extract target file
	size_t	i = 0;
	while (i < tmp.size() && tmp[i] != ' ')
		i++;
	//1.1: Skip spaces
	size_t	k = 0;
	while (i + k < tmp.size() && tmp[i + k] == ' ')
		k++;
	//1.2: Was there file?
	if (i == 0)
	{	//No target file
		std::cout << "FGET: Missing target file" << std::endl;
		return ;
	}
	//1.3: Save target file
	std::string	targetFile = tmp.substr(0, i); //File without spaces

	//2.0: Extract optional target directory
	i += k;
	size_t	j = 0;
	while (i + j < tmp.size() && tmp[i + j] != ' ')
		j++;
	k = i;
	while (j + k < tmp.size() && tmp[j + k] == ' ')
		k++;
	//2.1: Save target directory or create "./"
	std::string	targetDir = (k == 0) ? "./" : tmp.substr(i, k) + "/"; //Optional Dir without spaces

	//3.0: Check file existence
	if (this->_Files.find(targetFile) == this->_Files.end())
	{	//Target file doesn't exist
		std::cout << "FGET: Couldn't find target file" << std::endl;
		return ;
	}
	//3.1: Get file
	File	file(this->_Files.find(targetFile)->second);

	//4.0a: Check if global
	if (file.getGlobal())
		; //continue
	//4.0b: Check if channel specific
	else if (file.getChannel().empty())
	{	//File exists but user is not in target channel
		Channel* channel = findChannelByName(file.getChannel(), this->_Chan);
		if (channel == NULL)
		{
			std::cout << "FGET: Channel no longer exists" << std::endl;
			this->_Files.erase(file.getFileName());
			return ;
		}
		if (channel->GetGradeUser(user) == -1)
		{
			std::cout << "FGET: User is not in the target channel" << std::endl;
			return ;
		}
	}
	//4.0c: Check if target user
	else if (file.getCaptor() != user.getNick())
	{	//File exists but is not meant for this user
		std::cout << "FGET: File is not meant for " << user.getNick() << std::endl;
		return ;
	}

	//4.1: Merge fileDir and fileName
	std::string		full_path(targetDir + targetFile);
	//4.2: Open output
	std::fstream	ofs(full_path.c_str(), std::fstream::out);
	//4.3: Open input
	std::fstream	ifs(file.getFileDir().c_str(), std::fstream::in);
	//4.4: Check if open and read input into output
	if (ofs.is_open())
		ofs << ifs.rdbuf();
	//4.5a: If private transfer successful delete file
	if (ofs && !file.getGlobal() && file.getChannel().empty())
	{
		this->_Files.erase(file.getFileName());
		std::cout << "FGET: Succesfull, deleting _Files entry" << std::endl;
	}
	//4.5b: If trasnfer succesful
	else if (ofs)
	{
		std::cout << "FGET: Succesfull" << std::endl;
	}
	//4.5c: If transfer failed
	else
	{	//File didn't get written
		std::cout << "FGET: Error while outputting file" << std::endl;
		return ;
	}
}

void	Server::handleFileList()
{
	for (std::map<std::string, File>::iterator it = this->_Files.begin(); it != this->_Files.end(); it++)
	{
		std::cout << it->second.getFileName() << std::endl;
	}
}
