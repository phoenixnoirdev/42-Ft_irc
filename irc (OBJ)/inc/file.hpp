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
		std::string	_sender;
		std::string	_captor;
	
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
		std::string	getSender() const;
		std::string	getCaptor() const;
};

#endif