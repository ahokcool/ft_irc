/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 01:28:24 by astein            #+#    #+#             */
/*   Updated: 2024/05/10 23:06:33 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

std::ofstream Logger::_logFile;
bool Logger::_active = true;

void Logger::init()
{
    // Open the log file in append mode
    _logFile.open("./log.txt", std::ios::app);
    if (!_logFile.is_open())
        std::cerr << "Error opening log file." << std::endl;
}

void Logger::activateLogger()
{
	_active = true;
}

void Logger::deactivateLogger()
{
	_active = false;
}

void Logger::log(const std::string& logmsg)
{
	// If not active, return
	if (!_active)
		return;

    // If the file is not open, return
    if (!_logFile.is_open())
		return;

	if (logmsg.empty())
		return;

	std::string msg;
	if (logmsg[0] == '\n')
	{
		_logFile << "\n";
		msg = logmsg.substr(1);
	}
	else
	{
		msg = logmsg;
	}
    // Get current time
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Format time as a timestamp
    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", timeinfo);
    std::string timestamp(buffer);

    // Write the log message with timestamp prefix to the file
    _logFile << timestamp << " " << msg << std::endl;
}

void Logger::close()
{
    // Close the log file
    _logFile.close();
}
