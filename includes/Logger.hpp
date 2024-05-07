/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 23:48:47 by astein            #+#    #+#             */
/*   Updated: 2024/05/07 16:51:16 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>

class Logger
{
public:
	static void init(); // Initialize the logger
    static void log(const std::string& logmsg);
    static void close(); // Close the logger

private:
    static std::ofstream logFile;
};

#endif
