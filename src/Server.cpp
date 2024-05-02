/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:11 by astein            #+#    #+#             */
/*   Updated: 2024/05/02 16:07:23 by astein           ###   ########.fr       */
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
}

// Public Member Functions
// -----------------------------------------------------------------------------
void Server::initNetwork() throw(ServerException)
{
	info("[START] Init network", CLR_YLW);

	// This struct is holding the adress information about the socket
	// It's part of the socket.h library <netinet/in.h>
    struct sockaddr_in address;
    
    // Create a master socket for the server
	// AF_INET:			to use IPv4 (AF = Adress Family) (AF_INET6 for IPv6)
	// SOCK_STREAM:		TCP two way connection using a stream of bytes
	// 0:				Protocol (0 = default aka TCP/IP)
	// https://pubs.opengroup.org/onlinepubs/009604499/functions/socket.html
    if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw ServerException("Socket creation failed");

    // Set master socket to allow multiple connections,
    // this is just a good habit, it will work without this
    int opt = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
		throw ServerException("TODO");

    // Type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);
	// TODO: SET THE PASSWORD

    // Bind the socket to localhost port _port
    if (bind(_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw ServerException("TODO");


    // Try to specify maximum of 3 pending connections for the master socket
    if (listen(_socket, 3) < 0)
		throw ServerException("TODO");

	
	info("[>DONE] Init network", CLR_GRN);
}

void Server::goOnline()
{
	info("[START] Go online", CLR_GRN);
	// while (_keepRunning)
	// {
	// 	poll(NULL, 0, 1000);
	// 	pause();
	// }


    // Accept the incoming connection
    // int addrlen = sizeof(address);
    // puts("Waiting for connections ...");

// char buffer[1025];  //data buffer of 1K

	// int new_socket, max_clients = 30 sd;
	
	// int activity, valread;
    // int max_sd;

	//a message
    // const char *message = "ECHO Daemon v1.0 \r\n";


    // while (true) {
    //     // Clear the socket set
    //     FD_ZERO(&FUUUUUUUUCK);

    //     // Add master socket to set
    //     FD_SET(_socket, &FUUUUUUUUCK);
    //     max_sd = _socket;

    //     // Add child sockets to set
    //     for (i = 0; i < max_clients; i++) {
    //         // Socket descriptor
    //         sd = client_socket[i];

    //         // If valid socket descriptor then add to read list
    //         if (sd > 0) {
    //             FD_SET(sd, &FUUUUUUUUCK);
    //         }

    //         // Highest file descriptor number, need it for the select function
    //         if (sd > max_sd) {
    //             max_sd = sd;
    //         }
    //     }

    //     // Wait for an activity on one of the sockets, timeout is NULL,
    //     // so wait indefinitely
    //     activity = select(max_sd + 1, &FUUUUUUUUCK, NULL, NULL, NULL);

    //     if ((activity < 0) && (errno != EINTR)) {
    //         printf("select error");
    //     }

    //     // If something happened on the master socket,
    //     // then its an incoming connection
    //     if (FD_ISSET(_socket, &FUUUUUUUUCK)) {
    //         if ((new_socket = accept(_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
    //             perror("accept");
    //             exit(EXIT_FAILURE);
    //         }

    //         // Inform user of socket number - used in send and receive commands
    //         printf("New connection, socket fd is %d, ip is : %s, port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    //         // Send new connection greeting message
	// 		if (send(new_socket, message, strlen(message), 0) != static_cast<ssize_t>(strlen(message))) 
    // 			perror("send failed");


    //         puts("Welcome message sent successfully");

    //         // Add new socket to array of sockets
    //         for (i = 0; i < max_clients; i++) {
    //             // If position is empty
    //             if (client_socket[i] == 0) {
    //                 client_socket[i] = new_socket;
    //                 printf("Adding to list of sockets as %d\n", i);
    //                 break;
    //             }
    //         }
    //     }

    //     // Else its some IO operation on some other socket
    //     for (i = 0; i < max_clients; i++) {
    //         sd = client_socket[i];

    //         if (FD_ISSET(sd, &FUUUUUUUUCK)) {
    //             // Check if it was for closing, and also read the
    //             // incoming message
    //             if ((valread = read(sd, buffer, 1024)) == 0) {
    //                 // Somebody disconnected, get his details and print
    //                 getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    //                 printf("Host disconnected, ip %s, port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    //                 // Close the socket and mark as 0 in list for reuse
    //                 close(sd);
    //                 client_socket[i] = 0;
    //             }

    //             // Echo back the message that came in
    //             else {
	// 				printf("Message from client: %s\n", buffer);
    //                 buffer[valread] = '\0';
    //                 send(sd, buffer, strlen(buffer), 0);
    //             }
    //         }
    //     }
































		
	
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
    	throw ServerException("Invalid port number");

	// 194 is the default port for IRC
	// https://en.wikipedia.org/wiki/Port_(computer_networking)
	// The ports up to 49151 are not as strictly controlled
	// The ports from 49152 to 65535 are called dynamic ports
	if (portInt != 194 && (portInt < 1024 || portInt > 65535))
		throw ServerException("Port is not the IRC port (194) or in the range 1024-65535!");
	_port = portInt;
	// TODO: Check if password is valid
	_password = password;
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
