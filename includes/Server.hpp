/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:18 by astein            #+#    #+#             */
/*   Updated: 2024/05/03 18:29:16 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>      // For input and output
#include <exception>     // Standard exceptions
#include <list>          // For list container
#include <vector>        // For storring the poll fds
#include <string>        // For string operations
#include <sstream>       // For string streams
#include <cstdlib>       // General purpose utilities: memory management, program utilities, string conversions, random numbers, etc.
#include <cstring>       // For string manipulation and memory functions
#include <cstdio>        // For C-style input-output like perror
#include <fcntl.h>       // For file control options
#include <unistd.h>      // Provides access to the POSIX operating system API
#include <sys/socket.h>  // For socket operations
#include <netinet/in.h>  // For internet addresses
#include <arpa/inet.h>   // For internet operations
#include <sys/select.h>  // For select function
#include <signal.h>      // For signal handling
#include <cstdlib>		 // For getting the ip of the server
#include <errno.h>
#include <poll.h>

#include "utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"

#define BUFFER_SIZE 1024

// Standart exception class for Server
class ServerException : public std::exception
{
	public:
		ServerException(const std::string &message);
		~ServerException() 			throw();
		const char	*what() const	throw();

	private:
		std::string _message;
};

class Server
{
	public:
		Server(const std::string &port, const std::string &password) throw(ServerException);
		~Server();

		// Public Member Functions
		void    		initNetwork()   throw(ServerException);
		void    		goOnline()      throw(ServerException);
		void    		shutDown();

		// Signal handling for exit
		static void 	setupSignalHandling();
		static void 	sigIntHandler(int sig);

	private:
		Server(); // No default constructor
		
		// Private Member Functions
		void                    	parseArgs(const std::string &port, const std::string &password) throw(ServerException);
		std::vector<pollfd> 		getFdsAsVector() const;
		void                    	addClient(Client client);
		// void                    	removeClient(Client *client);
		// void                    	addChannel(Channel &channel);
		Client 						*getClientByFd(int fd);
		// void                    	removeChannel(Channel *channel);
		Client                  	*findUserByNickname(const std::string &nickname);
		void 						processMessage(Client *client, const std::string &ircMessage);
		Channel						*getChannelByName(const std::string &name);
		bool						isLoggedIn(Message &msg);

		void                    	broadcastMessage(const std::string &message) const;
		
		void                    	invite(Message &message, Channel *channel);
		void                    	mode(Message &message, Channel *channel);
		void                    	kick(Message &message, Channel *channel);
		void                    	join(Message &message, Channel *channel);
		void                    	topic(Message &message, Channel *channel);
		void                    	part(Message &message, Channel *channel);
		void                    	privmsg(Message &message);

		// Attributes
		static volatile sig_atomic_t 	_keepRunning;
	
		// This struct is holding the adress information about the socket
		// It's part of the socket.h library <netinet/in.h>
		struct sockaddr_in 				_address;
	
		int                     		_socket;
		u_int16_t		           		_port;
		std::string             		_password;
		std::list<Client>       		_clients;
		std::list<Channel>      		_channels;

	
	// template <typename T, typename O>
	// bool findInList(const std::string &name, const T &l)
	// {
	//     for (typename T::iterator it = l.begin(); it != l.end(); ++it)
	//     {
	//         if (it->getName() == name)
	//             return true;
	//     }
	//     return false;
	// }

	// template <typename T>
	// bool findInList(const std::string &name, const T &l)
	// {
	//     for (typename T::iterator it = l.begin(); it != l.end(); ++it)
	//     {
	//         if (it->getName() == name)
	//             return true;
	//     }
	//     return false;
	// }
};
	
#endif