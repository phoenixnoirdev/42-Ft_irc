/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelevequ <kelevequ@student.42luxembourg>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-09-29 15:57:18 by kelevequ          #+#    #+#             */
/*   Updated: 2025-09-29 15:57:18 by kelevequ         ###   ########.lu       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

# include <string>
# include <map>

class	File
{
	private:
		std::string	_fileName;
		std::string	_fileDir;
		std::string	_channel;
		std::string	_sender;
		std::string	_captor;
		bool		_global;
	
	public:
		//=================
		//Canonical Form
		//=================
		File();
		File(const File& other);
		File&	operator=(const File& other);
		~File();

		File(std::string fileName, std::string fileDir, std::string sender, std::string captor);

		std::string	getFileName() const;
		std::string	getFileDir() const;
		std::string	getChannel() const;
		std::string	getSender() const;
		std::string	getCaptor() const;
		bool		getGlobal() const;

		void		setFileName(std::string fileName);
		void		setFileDir(std::string fileDir);
		void		setChannel(std::string channel);
		void		setSender(std::string sender);
		void		setCaptor(std::string captor);
		void		setGlobal(bool global);
};

#endif