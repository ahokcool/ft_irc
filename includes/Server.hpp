/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:18 by astein            #+#    #+#             */
/*   Updated: 2024/05/04 23:38:49 by astein           ###   ########.fr       */
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
#include <map>
#include <cstdlib>		 // For getting the ip of the server
#include <errno.h>
#include <poll.h>

#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include "Logger.hpp"

#define BUFFER_SIZE 512	// IRC message buffer size

// -------------------------------------------------------------------------
// Standard exception class for server
// -------------------------------------------------------------------------
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
	// -------------------------------------------------------------------------
	// Construction / Destruction
	// -------------------------------------------------------------------------
	public:
		Server(const std::string &port, const std::string &password);
		~Server();
		void parseArgs(const std::string &port, const std::string &password);
	private:
		Server(); // Private default constructor

	// -------------------------------------------------------------------------
	// Server Methods
	// -------------------------------------------------------------------------
	public :
		void				initNetwork();
		void				goOnline();
		void				shutDown();
	private:
		std::vector<pollfd>	getFdsAsVector() const;
		void				broadcastMessage(const std::string &message) const;

	// -------------------------------------------------------------------------
	// Processing the Messages
	// -------------------------------------------------------------------------
	private:
		void	processMessage(Client &sender, const std::string &ircMessage);
		bool	isLoggedIn(Message &msg);
		void	chooseCommand(Message &msg);

		typedef void	(Server::*CommandFunction)(Message&);
		void	nick	(Message &message);
		void	user	(Message &message);
		void	whois	(Message &message);
		void	privmsg	(Message &message);
		void	join	(Message &message);
		void	invite	(Message &message);
		void	topic	(Message &message);
		void	mode	(Message &message);
		void	kick	(Message &message);
		void	part	(Message &message);

	// -------------------------------------------------------------------------
	// Client Methods
	// -------------------------------------------------------------------------
	private:
		void	addClient(Client client);
		void	removeClient(Client *client);
		Client	*getClientByFd(int fd);
		Client	*getClientByNick(const std::string &nickname);	
	
	// -------------------------------------------------------------------------
	// Channel Methods
	// -------------------------------------------------------------------------
	private:
		void	addChannel(Channel channel);
		void	removeChannel(Channel *channel);
		Channel	*createNewChannel(Message &msg);

	// -------------------------------------------------------------------------
	// Attributes
	// -------------------------------------------------------------------------
	private:
		std::string			_serverIP;
		struct sockaddr_in	_address;
		int					_socket;
		u_int16_t			_port;
		std::string			_password;
		std::list<Client>	_clients;
		std::list<Channel>	_channels;
		
		// Declare the map of all allowed cmds
		std::map<std::string, CommandFunction> _cmds;

	// -------------------------------------------------------------------------
	// Static Signal handling (for exit with CTRL C)
	// -------------------------------------------------------------------------
	public:
		static volatile sig_atomic_t	_keepRunning;
		static void						setupSignalHandling();
		static void						sigIntHandler(int sig);

	template <typename L>
	bool	isNameAvailable(L &list, const std::string &name)
	{
		typename L::iterator it;

		for (it = list.begin(); it != list.end(); ++it)
		{
			if (it->getUniqueName() == name)
				return false;
		}
		return true;
	}

	template <typename L>
	typename L::value_type	*getInstanceByName(L &list, const std::string &name)
	{
		typename L::iterator	it;

		if (name.empty())
			return NULL;
		for (it = list.begin(); it != list.end(); ++it)
		{
			if (it->getUniqueName() == name)
			{
				return &(*it);
				break ;
			}
		}
		return NULL;
	}
};
	
#endif