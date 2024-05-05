/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 01:28:24 by astein            #+#    #+#             */
/*   Updated: 2024/05/04 01:36:08 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

std::ofstream Logger::logFile;

void Logger::init()
{
    // Open the log file in append mode
    logFile.open("./log.txt", std::ios::app);
    if (!logFile.is_open())
        std::cerr << "Error opening log file." << std::endl;
}

void Logger::log(const std::string& logmsg)
{
    // If the file is not open, return
    if (!logFile.is_open())
        return;

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
    logFile << timestamp << " " << logmsg << std::endl;
}

void Logger::close()
{
    // Close the log file
    logFile.close();
}