/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 20:47:42 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/02 02:45:19 by astein           ###   ########.fr       */
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
    catch (std::exception &e)
    {
        return 1;
    }
	title("IRC Server stopped!", true, true);
    return 0;
}
