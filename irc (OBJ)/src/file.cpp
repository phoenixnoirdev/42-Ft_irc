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

File::File(): _fileName(NULL), _fileDir(NULL), _sender(NULL), _captor(NULL) {}

File::File(const File& other): _fileName(other._fileName), _fileDir(other._fileDir), _sender(other._sender), _captor(other._captor) {}

File&	File::operator=(const File& other)
{
	if (this != &other)
	{
		_fileName = other.getFileName();
		_fileDir = other.getFileDir();
		_sender = other.getSender();
		_captor = other.getCaptor();
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
			_sender(sender),
			_captor(captor) {}

std::string	File::getFileName() const
{
	return (_fileName);
}

std::string	File::getFileDir() const
{
	return (_fileDir);
}

std::string	File::getSender() const
{
	return (_sender);
}

std::string	File::getCaptor() const
{
	return (_captor);
}
