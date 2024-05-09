/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:17:38 by astein            #+#    #+#             */
/*   Updated: 2024/05/09 22:27:33 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

// Helper functions for printing
void	title(std::string str, bool newline_before, bool newline_after)
{
	std::string msg = ">>> " + str + " <<<";
	
	if (newline_before)
		std::cout << std::endl;
	std::cout << CLR_ORN << msg << CLR_RST << std::endl;
	if (newline_after)
		std::cout << std::endl;
	Logger::log("Title message: " + msg);
}

void	info(std::string str, std::string clr)
{
	std::string msg = " >> " + str;

	std::cout << clr << msg << CLR_RST << std::endl;
	Logger::log("Info message: " + msg);
}

bool	intNoOverflow(std::string token)
{
    const std::string maxInt = "2147483647";

    token.erase(0, token.find_first_not_of('0'));
    if (token.empty())
        return (false);
    if (token.length() > maxInt.size())
        return (false);
    if (token.length() < maxInt.size())
        return (true);
    return (token.compare(maxInt) <= 0);
}