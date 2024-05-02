/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:11 by astein            #+#    #+#             */
/*   Updated: 2024/05/02 22:32:54 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Constructor
Server::Server(const std::string &port, const std::string &password) throw(ServerException)
{
	parseArgs(port, password);
}

// Destructor
Server::~Server()
{
	// NOthing to do
	// TODO: Close all sockets
}

// Public Member Functions
// -----------------------------------------------------------------------------
void Server::initNetwork() throw(ServerException)
{
	info("[START] Init network", CLR_YLW);

    // Create a master socket for the server
	// AF_INET:			to use IPv4 (AF = Adress Family) (AF_INET6 for IPv6)
	// SOCK_STREAM:		TCP two way connection using a stream of bytes
	// 0:				Protocol (0 = default aka TCP/IP)
	// https://pubs.opengroup.org/onlinepubs/009604499/functions/socket.html
    if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw ServerException("Socket creation failed:\n\t" +	std::string(strerror(errno)));

    
	// TODO: REVISE THIS
	// here we futher configure the socket
    // SOL_SOCKET:		Use SOL_SOCKET for general settings
	// SO_REUSEADDR:	Allow the socket to be reused immediately after it is closed
	// opt:				Option value set to 1; needs to be a void pointer
    int opt = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<void*>(&opt), sizeof(opt)) < 0)
		throw ServerException("Setsockopt failed\n\t" +	std::string(strerror(errno)));

    // Specify the type of socket created
	// AF_INET:			to use IPv4 (AF = Adress Family) (AF_INET6 for IPv6)
	// INADDR_ANY:		accept any incoming messages from all interfaces of the host machine
	// htons(_port):	converts the port number to network byte order
    _address.sin_family 		= AF_INET;
    _address.sin_addr.s_addr	= INADDR_ANY;
    _address.sin_port 			= htons(_port);

	// TODO: SET THE PASSWORD

	// Binds the socket to the previously defined address
	// https://pubs.opengroup.org/onlinepubs/009695399/functions/bind.html
    if (bind(_socket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throw ServerException("Bind failed\n\t" +	std::string(strerror(errno)));

	// Listen for incoming connections
	// 3: The maximum length to which the queue of pending connections for sockfd may grow
	// https://pubs.opengroup.org/onlinepubs/009695399/functions/listen.html
	if (listen(_socket, 3) < 0)
		throw ServerException("Listen failed\n\t" +	std::string(strerror(errno)));

	info("[>DONE] Init network", CLR_GRN);
	info("Server IP: ", CLR_GRN);
	system("hostname -I | awk '{print $1}'");
}

void Server::goOnline() throw(ServerException)
{
	info("[START] Go online", CLR_GRN);
	std::vector<pollfd> fds;
	while (_keepRunning)
	{
		fds = getFdsAsVector();
		info ("Waiting for connections ...", CLR_ORN);
		int pollReturn = poll(fds.data(), fds.size() , -1);
		info ("DONE Waiting for connections ...", CLR_ORN);
		if (pollReturn == -1)
			throw ServerException("Poll failed\n\t" + std::string(strerror(errno)));
		if (pollReturn == 0)
			continue;

		// Check for new connections
        if (fds[0].revents & POLLIN)
		{
			// https://pubs.opengroup.org/onlinepubs/009695399/functions/accept.html
			int	addrlen = sizeof(_address);
			int new_socket = accept(fds[0].fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen);

            if (new_socket < 0)
				throw ServerException("Accept failed\n\t" + std::string(strerror(errno)));
			try
			{
				processNewClient(Client(new_socket,"nicknamedefault")); //TODO: CHANGE NICKNAME
            	std::cout << "New client connected with fd: " << new_socket << std::endl;
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
        }

		// TODO: in client check for to long msgs
		// Read from clients
		char buffer[1024];
		for (size_t i = 1; i < fds.size(); ++i)
		{
            if (fds[i].revents & POLLIN)
			{
                int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                if (valread == 0)
				{
                    std::cout << "Client disconnected" << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    --i; // Adjust loop counter since we removed an element
                } else if (valread < 0)
				{
                    perror("read");
                    exit(EXIT_FAILURE);
                } else
				{
                    buffer[valread] = '\0';
                    std::cout << "Message from client: " << buffer << std::endl;
					dealWithChannelMsg(getClientByFd(fds[i].fd), buffer);
                }
            }
		}
	}	
	info("[>DONE] Go online", CLR_YLW);
}

void Server::shutDown()
{
	info("[START] Shut down", CLR_YLW);
	broadcastMessage("!!!Server shutting down in 42 seconds!!!");
	sleep(42);
	// TODO: SLEEP is C! Use C++ sleep_for
	info("[>DONE] Shut down", CLR_GRN);
}

// Private Member Functions
// -----------------------------------------------------------------------------
void Server::parseArgs(const std::string &port, const std::string &password) throw (ServerException)
{
	u_int16_t	portInt;
	std::istringstream iss(port);

	if (!(iss >> portInt))
    	throw ServerException("Invalid port number\n\t>>Port is not the IRC port (194) or in the range 1024-65535!");

	// 194 is the default port for IRC
	// https://en.wikipedia.org/wiki/Port_(computer_networking)
	// The ports up to 49151 are not as strictly controlled
	// The ports from 49152 to 65535 are called dynamic ports
	if (portInt != 194 && (portInt < 1024 || portInt > 65535))
		throw ServerException("Port is not the IRC port (194) or in the range 1024-65535!");
	_port = portInt;
	info ("port accepted: " + port, CLR_YLW);
	// TODO: Check if password is valid
	_password = password;
}

std::vector<pollfd> Server::getFdsAsVector() const
{
	std::vector<pollfd> fds;
	pollfd fd;
	fd.fd = _socket;
	// POLLIN: There is data to read
	fd.events = POLLIN;
	fds.push_back(fd);
	for (std::list<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		fd.fd = it->getSocketFd();
		fd.events = POLLIN;
		fds.push_back(fd);
	}
	if(fds.size() == 0)
		throw ServerException("No socket file descriptors found");
	return fds;
}

void                    	Server::processNewClient(Client client)
{
	// // Messages has to be
	// // NICK nickname
	// // USER username hostname servername :realname
	// // PING :server
	// // PONG :server
	// while(iss 
	
	_clients.push_back(client);
}

Client 						*Server::getClientByFd(int fd)
{
	for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getSocketFd() == fd)
			return &(*it);
	}
	return NULL;
}

void Server::broadcastMessage(const std::string &message) const
{
	info("[START] Broadcast message", CLR_YLW);
	std::string ircMessage = "TODO:!" + message + "TODO:!";
	
	// Send it to all clients
	for (std::list<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		// TODO: Create the ircMessage in right format! aka adding the user's name
		it->sendMessage(ircMessage);
	}

	// Send it to all channels
	for (std::list<Channel>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		it->sendMessage(ircMessage);
	}
	info("[>DONE] Broadcast message", CLR_GRN);
}

// check the structure of a message, oif it alwazs has like 3 components
// TODO:CHECK FOR client NULL Pointer!!!!
void Server::dealWithChannelMsg(Client *client, const std::string &ircMessage)
{
	// We deal with all thoe messages
	// INVITE ash #testtest
	// MODE #testtest i
	// KICK #testtest anshovah_
	//  MODE #testtest l 12
	// JOIN #newChannel
	// TOPIC #testtest :        hello             world        42

	// we need to extract the channel name and the coomand
	// if channel doest exist create it
	
	// after parsing this logic needs to be implemented:
	
	std::istringstream iss(ircMessage);
	std::string token;
	std::string cmd;
	std::string channelName;
	std::string arg1;
	std::string arg2;
	bool		createdNewChannel = false;
	
	while (iss >> token)
	{
		if (cmd.empty())
			cmd = token;
		else if (token[0] == '#')
			channelName = token;
		else if (arg1.empty() && token[0] != ':')
			arg1 = token;
		else if (arg2.empty() && token[0] != ':')
			arg2 = token;
		else if (token[0] == ':')
		{	
			arg1 = ircMessage.substr(ircMessage.find(':') + 1);
			break;
		}
	}

	if (cmd.empty() || channelName.empty())
	{
		info("Invalid message", CLR_RED);
		return;
	}
	Channel *channel = NULL;
	for (std::list<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (it->getName() == channelName)
		{
			channel = &(*it);
			break;
		}
	}
	if (!channel && cmd[0] != 'J')
	{
		_channels.push_back(Channel(channelName, client));
		channel = &(_channels.back());
		createdNewChannel = true;
	}
	else
	{
		info("Channel not found", CLR_RED);
		return;
	}
	
	switch (cmd[0])
	{
		case 'I':
			// INVITE
			// invite(client, channelName, arg1);
			break;
		case 'M':
			// MODE
			// mode(client, channelName, arg1, arg2);
			break;
		case 'K':
			// KICK
			// kick(client, channelName, arg1, arg2);
			break;
		case 'J': // JOIN
		{
			// Server				Client			Channel
			// if channel exist
			// 	client.addChannel()	addChannel()
			// 				 {_channels.pushBack...}	
			// 	channel.addClient()				addClient()
			// 						         {_clients.pushBack...}	
			// else
			// 	_channels.pushback(new Channel(name, client)			
			if (!createdNewChannel)
			{
				client->joinChannel(channel);
				channel->addClient(client);
			}
		}
			
			break;
		case 'T':
			// TOPIC
			// topic(client, channelName, arg1);
			break;
		case 'P': // PART
		{
				// client.leabvechannel
				// channel.removeclient
				// if !channel.isActive
				// 	_channel.remove(this)
				// 		->this will call the destructor of the channel
				// 			->destructor of channel will call alluser.leaveChannel
			client->leaveChannel(channel);
			channel->removeClient(client);
			if (!channel->isActive())
				_channels.remove(*channel);
		}
			break;
		default:
			info("Invalid message", CLR_RED);
			break;
	}
	// switch for first letter of cmd as char
	
/* 	JOIN ON SERVER
Server				Client			Channel
if channel exist
	client.addChannel()	addChannel()
				 {_channels.pushBack...}	
	channel.addClient()				addClient()
						         {_clients.pushBack...}	
else
	_channels.pushback(new Channel(name, client)
				
				
				
PART ON SERVER
	client.leabvechannel
	channel.removeclient
	if !channel.isActive
		_channel.remove(this)
			->this will call the destructor of the channel
				->destructor of channel will call alluser.leaveChannel


MODE MODE i
	call fucntion toggleInvite or so...
 */
}


// Signal handling for exit
// -----------------------------------------------------------------------------
volatile sig_atomic_t Server::_keepRunning = 1;

void Server::sigIntHandler(int sig)
{
	if(sig != SIGINT)
		return;
    _keepRunning = 0;  // Set flag to false to break the loop
    info("Shutdown signal received, terminating server..." , CLR_RED);
	// TODO: Implement the shutdown logic
}

void Server::setupSignalHandling()
{
    signal(SIGINT, Server::sigIntHandler);
}

// Server Exception Class Implementation
// -----------------------------------------------------------------------------

ServerException::ServerException(const std::string &message) : _message(message)
{
	// Nothing to do
}

ServerException::~ServerException() throw()
{
	// Nothing to do
}

const char *ServerException::what(void) const throw()
{
    return (_message.c_str());
}
