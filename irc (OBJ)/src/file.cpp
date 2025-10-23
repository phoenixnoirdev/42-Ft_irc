/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42luxembourg>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-09-29 16:03:37 by kelevequ          #+#    #+#             */
/*   Updated: 2025-09-29 16:03:37 by kelevequ         ###   ########.lu       */
/*                                                                            */
/* ************************************************************************** */

#include "file.hpp"

File::File(): _fileName(NULL), _fileDir(NULL), _channel(NULL), _sender(NULL), _captor(NULL), _global(0) {}

File::File(const File& other): _fileName(other._fileName), _fileDir(other._fileDir), _channel(other._channel), _sender(other._sender), _captor(other._captor), _global(other._global) {}

File&	File::operator=(const File& other)
{
	if (this != &other)
	{
		_fileName = other.getFileName();
		_fileDir = other.getFileDir();
		_channel = other.getChannel();
		_sender = other.getSender();
		_captor = other.getCaptor();
		_global = other.getGlobal();
	}
	return (*this);
}

File::~File() {}

File::File(	std::string fileName,
			std::string fileDir,
			std::string sender,
			std::string captor):
			_fileName(fileName),
			_fileDir(fileDir),
			_channel(NULL),
			_sender(sender),
			_captor(captor),
			_global(0) {}

std::string	File::getFileName() const
{
	return (_fileName);
}

std::string	File::getFileDir() const
{
	return (_fileDir);
}

std::string	File::getChannel() const
{
	return (_channel);
}

std::string	File::getSender() const
{
	return (_sender);
}

std::string	File::getCaptor() const
{
	return (_captor);
}

bool	File::getGlobal() const
{
	return (_global);
}

void		File::setFileName(std::string fileName)
{
	_fileName = fileName;
}

void		File::setFileDir(std::string fileDir)
{
	_fileDir = fileDir;
}

void		File::setChannel(std::string channel)
{
	_channel = channel;
}

void		File::setSender(std::string sender)
{
	_sender = sender;
}

void		File::setCaptor(std::string captor)
{
	_captor = captor;
}

void		File::setGlobal(bool global)
{
	_global = global;
}
