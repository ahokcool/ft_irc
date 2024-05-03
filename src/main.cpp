/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 20:47:42 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/03 16:56:26 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

int main(int ac, char **av)
{

    if (ac != 3)
    {
        std::cout << "Usage: ./ircserv <port> <pswd>" << std::endl;
        return 1;
    }
	Server::setupSignalHandling();
    try
    {
		title("IRC Server", true, false);
		info("Welcome to the IRC Server", CLR_GRN);
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
	catch (...)
    { //TODO: check if used
		info ("UNKNOWN EXCEPTION CAUGHT: ", CLR_RED);
    }
	title("IRC Server stopped!", true, true);
    return 0;
}
