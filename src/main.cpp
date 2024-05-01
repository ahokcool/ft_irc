/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 20:47:42 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/01 21:45:12 by anshovah         ###   ########.fr       */
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
    //TODO: Signal handling for Ctrl+C(exiting the server gracefully)
    try
    {
        Server server(av[1], av[2]);
        server.initNetwork();
        server.goOnline();
    }
    catch (std::exception &e)
    {
        return 1;
    }
    return 0;
}
