/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:11 by astein            #+#    #+#             */
/*   Updated: 2024/05/02 23:14:37 by anshovah         ###   ########.fr       */
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
void Server::processMessage(Client *client, const std::string &ircMessage)
{
	try 
	{
		Message     message(client, ircMessage);
		bool		createdNewChannel = false;
		
		// we now have a valid message 
		Channel *channel = NULL;
		
		// If we have a channel name find / create it
		if (!message.getChannelName().empty())
		{
			// Try to find the channel
			for (std::list<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
			{
				if (it->getName() == message.getChannelName())
				{
					channel = &(*it);
					break;
				}
			}
		
			// Couldn"t find the channel -> create it (if cmd is JOIN)
			if (!channel && message.getCmd()[0] != 'J')
			{
				_channels.push_back(Channel(message.getChannelName(), client));
				channel = &(_channels.back());
				createdNewChannel = true;
			}
			else
			{
				info("Channel not found", CLR_RED);
				return;
			}
		}
		
		// Process the message aka call the right function
		switch (message.getCmd()[0])
		{
			case 'I':
				// INVITE <client> #<channel>
				invite(message, channel);
				break;

			case 'M':
				// MODE #<channelName> flag
				mode(message, channel);
				break;

			case 'K':
				// KICK #<channelName> <client>
				kick(message, channel);
				break;

			case 'J':
				// JOIN #<channelName>
				if (!createdNewChannel)
					join(message, channel);
				break;
				
			case 'T':
				// TOPIC #<channelName> :<topic>
				topic(message, channel);
				break;

			case 'P':
			{
				if (message.getCmd()[1] == 'R') // PRIVMSG
				{
					// PRIVMSG <recepient> <message>
					privmsg(message);
				}
				else // PART
				{
					// PART #<channelName>
					part(message, channel);
				}
			}
				break;

			default:
				info("Invalid message", CLR_RED);
				break;
		}
	}
	catch (MessageException &e)
	{
		info(e.what(), CLR_RED);
		return;
	}
}

void Server::invite(const Message &message, Channel *channel)
{
	// INVITE <client> #<channel>
	// if (!findInList(message.getArg1(), _clients))
	// {
	// 	// info(":No such nick", CLR_RED);
	// 	// TODO: no suck nick
	// 	return;
	// }
	// if (!findInList(message.getChannelName(), _channels))
	// {
	// 	// TODO: no such channel
	// 	return ;
	// }
	message.getReceiver()->getInvited(channel);
	// TODO: send a message that a client was invited to a channel
}

void Server::mode(const Message &message,  Channel *channel)
{
	(void)message;
	(void)channel;
	// MODE #<channelName> flag
}

void Server::kick(const Message &message, Channel *channel)
{
	// KICK #<channelName> <client>
	// if (!findInList(message.getArg1(), _clients))
	// {
	// 	// info(":No such nick", CLR_RED);
	// 	// TODO: no suck nick
	// 	return;
	// }
	// if (!findInList(message.getChannelName(), _channels))
	// {
	// 	// TODO: no such channel
	// 	return ;
	// }
	message.getSender()->getKicked(channel);
}

void Server::join(const Message &message, Channel *channel)
{
	message.getSender()->joinChannel(channel);
	channel->addClient(message.getSender());
	//TODO: send messages to the channel that the client has joined
}

void Server::topic(const Message &message,  Channel *channel)
{
	(void)message;
	(void)channel;
	// TOPIC #<channelName> :<topic>
	
}

Client *Server::findUserByNickname(const std::string &nickname)
{
	for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getNickname() == nickname)
			return &(*it);
	}
	return NULL;
}

void Server::privmsg(const Message &message)
{
	// PRIVMSG <recepient> <message>
	// 1. need to find the recepient
	Client *recepient = findUserByNickname(message.getArg1());

	if (!recepient)
	{
		info("Recepient not found", CLR_RED);
		// TODO: send a message to the sender that the recepient was not found
		return;
	}

	// 2. send the message to the recepient
	std::string ircMessage = "TODO:!" + message.getArg2() + "TODO:!";
	recepient->sendMessage(ircMessage);
}

void Server::part(const Message &message, Channel *channel)
{
	message.getSender()->leaveChannel(channel);
	channel->removeClient(message.getSender());
	if (!channel->isActive())
		_channels.remove(*channel);
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
