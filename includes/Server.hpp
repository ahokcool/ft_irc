/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:18 by astein            #+#    #+#             */
/*   Updated: 2024/05/12 18:46:38 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>		// For input and output
#include <exception>	// Standard exceptions
#include <list>			// For list container
#include <vector>		// For storring the poll fds
#include <string>		// For string operations
#include <sstream>		// For string streams
#include <cstdlib>		// General purpose utilities: memory management, program utilities, string conversions, random numbers, etc.
#include <cstring>		// For string manipulation and memory functions
#include <cstdio> 		// For C-style input-output like perror
#include <fcntl.h>		// For file control options
#include <unistd.h>		// Provides access to the POSIX operating system API
#include <sys/socket.h>	// For socket operations
#include <netinet/in.h>	// For internet addresses
#include <arpa/inet.h>	// For internet operations
#include <sys/select.h>	// For select function
#include <signal.h>		// For signal handling
#include <map>
#include <cstdlib>		// For getting the ip of the server
#include <errno.h>
#include <poll.h>

#include "Channel.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include "Logger.hpp"

class Client;
class Channel;
class Message;

#define BUFFER_SIZE 512	// IRC message buffer size
#define PROMT ">>>FINISHERS IRC NET<<<"

// -------------------------------------------------------------------------
// Standard exception class for server
// -------------------------------------------------------------------------
class ServerException : public std::exception
{
	public:
		ServerException(const std::string &msg);
		~ServerException() 			throw();
		const char	*what() const	throw();
		
	private:
		std::string _msg;
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
		void				broadcastMessage(const std::string &msg) const;

	// -------------------------------------------------------------------------
	// Processing the Messages
	// -------------------------------------------------------------------------
	private:
		void	processMessage(Client *sender, const std::string &ircMessage);
		bool	isLoggedIn(Message *msg);
		void	chooseCommand(Message *msg);

		typedef void	(Server::*CommandFunction)(Message*);
		void	pass	(Message *msg);		// WORKS
		void	nick	(Message *msg);		// WORKS
		void	user	(Message *msg);		// WORKS
		void	who		(Message *msg);		// WORKS
		void	whois	(Message *msg);		// WORKS
		void	privmsg	(Message *msg);		// WORKS
		void	join	(Message *msg);		// WORKS
		void	invite	(Message *msg);		// WORKS
		void	topic	(Message *msg);
		void	mode	(Message *msg);
		void	kick	(Message *msg);		// ERORRO NO SUCH USER
		void	part	(Message *msg);

	// -------------------------------------------------------------------------
	// Client Methods
	// -------------------------------------------------------------------------
	public:
		// Needed by Channel Mode 'o'
		Client	*getClientByNick(const std::string &nickname);	
	private:
		Client	*getClientByFd(int fd);
	
	// -------------------------------------------------------------------------
	// Channel Methods
	// -------------------------------------------------------------------------
	private:
		void	addChannel(const Channel *channel);
		void	removeChannel(Channel *channel);
		Channel	*createNewChannel(Message *msg);

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
		
		if (name.empty())
			return false;
		if (list.empty())
			return true;
		Logger::log("Check if name is available: " + name);

		for (it = list.begin(); it != list.end(); ++it)
		{
			if (it->getUniqueName() == name)
			{
				Logger::log("FALSE Check if name is available: " + name);
				return false;
			}
		}
		Logger::log("TRUE Check if name is available: " + name);
		return true;
	}

	template <typename L>
	typename L::value_type	*getInstanceByName(L &list, const std::string &name)
	{
		typename L::iterator	it;
		Logger::log("Check if object is there: " + name);

		if (name.empty() || list.empty())
		{
			Logger::log("Check if object is there: NAME is not given or LIST is empty!");
			return NULL;
		}
		for (it = list.begin(); it != list.end(); ++it)
		{
			if (it->getUniqueName() == name)
			{
				Logger::log("TRUE Check if object is there: " + name);
				return &(*it);
			}
		}
		Logger::log("FALSE Check if object is there: " + name);
		return NULL;
	}
};
	
#endif