/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 20:47:42 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/11 18:27:22 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

int main(int ac, char **av)
{
	Logger::init();
	Logger::activateLogger();
	Logger::deactivateLogger();
    if (ac != 3)
    {
		info("Usage: ./ircserv <port> <pswd>", CLR_RED);
        return 1;
    }
	Server::setupSignalHandling();
    try
    {
		title("IRC Server", true, false);
		info("Welcome to " + std::string(PROMT), CLR_GRN);
		info("End the server with Ctrl+C", CLR_GRN);
		info("~~~~~~~~~~~~~~~~~~~~~~~~~~", CLR_GRN);
		info("Create server instance", CLR_BLU);
        Server server(av[1], av[2]);
        server.initNetwork();
        server.goOnline();
    }
	catch (const ServerException &se)
	{
		info ("SERVER EXCEPTION CAUGHT: ", CLR_RED);
		info(se.what(), CLR_RED);
	}
    catch (const std::exception &e)
    {
		info ("STANDARD EXCEPTION CAUGHT: ", CLR_RED);
		info(e.what(), CLR_RED);
    }
	Logger::close();
	title("IRC Server stopped!", true, true);
    return 0;
}
