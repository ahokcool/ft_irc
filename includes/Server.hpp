/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:18 by astein            #+#    #+#             */
/*   Updated: 2024/05/02 02:49:10 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <exception>
#include <list>
#include <poll.h>
#include <cstdlib> // For atoi
#include <unistd.h> // For sleep
#include "utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <string>
#include <sstream>
#include <signal.h>

class Server
{
    public:
        Server(const std::string &port, const std::string &password) throw(std::exception);
        ~Server();

		// Public Member Functions
        void                    initNetwork() throw(std::exception);
        void                    goOnline();
        void                    shutDown();

		// Signal handling for exit
    	static void 					setupSignalHandling();
    	static void 					sigIntHandler(int sig);

    private:
        Server(); // No default constructor
        
		// Private Member Functions
        void                    parseArgs(const std::string &port, const std::string &password) throw(std::exception);
        // void                    addClient(Client *client);
        // void                    removeClient(Client *client);
        // void                    addChannel(Channel &channel);
        // void                    removeChannel(Channel *channel);
		void 					dealWithChannelMsg(Client *client, const std::string &ircMessage);

        void                    broadcastMessage(const std::string &message) const;
        
		// Attributes
		static volatile sig_atomic_t 	_keepRunning;
        std::string             		_port;
        std::string             		_password;
        std::list<Client>       		_clients;
        std::list<Channel>      		_channels;

		
};

#endif