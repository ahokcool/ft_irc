/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:11 by astein            #+#    #+#             */
/*   Updated: 2024/05/04 01:29:32 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// -----------------------------------------------------------------------------
// Construction / Destruction
// -----------------------------------------------------------------------------
Server::Server(const std::string &port, const std::string &password)
{
	parseArgs(port, password);
}

Server::~Server()
{
	// TODO: Close all sockets
}

void Server::parseArgs(const std::string &port, const std::string &password)
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

// -----------------------------------------------------------------------------
// Server Methods
// -----------------------------------------------------------------------------
void	Server::initNetwork()
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

	// Use fcntl to set the socket to non-blocking
	// https://pubs.opengroup.org/onlinepubs/009695399/functions/fcntl.html
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
		throw ServerException("Fcntl failed\n\t" +	std::string(strerror(errno)));

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

void	Server::goOnline()
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
		{
			if (!_keepRunning)
			{
				shutDown();
				return ;
			}
			throw ServerException("Poll failed\n\t" + std::string(strerror(errno)));
		}
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
			// Use fcntl to set the socket to non-blocking
			// https://pubs.opengroup.org/onlinepubs/009695399/functions/fcntl.html
			if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
				throw ServerException("Fcntl failed\n\t" +	std::string(strerror(errno)));
			addClient(Client(new_socket));
        }

		// TODO: in client check for to long msgs
		// Read from clients
		char buffer[BUFFER_SIZE+1];	// +1 for the null terminator
		Client *cur_client;
		for (size_t i = 1; i < fds.size(); ++i)
		{
            if (fds[i].revents & POLLIN)
			{
				cur_client = getClientByFd(fds[i].fd);
                int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                if (valread <= 0)
				{
					// Some read error happend
					// The server doesn't bother to much and just deletes this client
					info("Client" + cur_client->getNickname() + " disconnected", CLR_RED);
					Logger::log("Client" + cur_client->getNickname() + " disconnected");
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);		// erase the client fd from the fd vector
					removeClient(cur_client);		// erase the client from the client lisr
                    --i; // Adjust loop counter since we removed an element
                }
				else
				{
                    buffer[valread] = '\0';
					// Since the buffer could only be a part of a message we
					// 1. append it to the client buffer
					if (!cur_client->appendBuffer(buffer))
					{
						cur_client->sendMessage("Message was to long and will be deleted");
						// The message was to long
						// The full messages will be deleted and the client will be informed
					}
					// 2. get the full message(s) from the client buffer
					std::string fullMsg;
					while (!(fullMsg = cur_client->getFullMessage()).empty())
					{
						// 3. process the message(s)
						Logger::log("start processing message from " + cur_client->getNickname() + " -> " + fullMsg);
						processMessage(cur_client, fullMsg);
					}
                }
            }
		}
	}	
	info("[>DONE] Go online", CLR_YLW);
}

void	Server::shutDown()
{
	info("[START] Shut down", CLR_YLW);
	broadcastMessage("!!!Server shutting down in 42 seconds!!!\r\n");
	sleep(1);
	// TODO: SLEEP is C! Use C++ sleep_for
	info("[>DONE] Shut down", CLR_GRN);
}

std::vector<pollfd>	Server::getFdsAsVector() const
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

void	Server::broadcastMessage(const std::string &message) const
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

// -----------------------------------------------------------------------------
// Processing the Messages
// -----------------------------------------------------------------------------
void	Server::processMessage(Client *client, const std::string &ircMessage)
{
	try 
	{
		// Parse the IRC Message
		Message     msg(client, ircMessage);

		//Execute IRC Message
		//	1. Check if CLIENT is loggedin
		if (!isLoggedIn(msg))
			return ;
		
		//	2. Execute normal commands
		//		3.1. Find the channel if there is  channelname in the msg
		Channel *channel = getChannelByName(msg.getChannelName());
		
		// 		3.2 Process the msg aka call the right function
		chooseCommand(msg, channel);
	}
	catch (MessageException &e)
	{
		info(e.what(), CLR_RED);
		return;
	}
}

bool	Server::isLoggedIn(Message &msg)
{
	//	1. Check if NICK is set
	if (msg.getSender()->getNickname().empty())
	{
		if (msg.getCmd() == "NICK" && !msg.getArg(0).empty())
		try 
		{
			msg.getSender()->setNickname(msg.getArg(0));
		}
		catch (NickNameException &nne)
		{
			info(nne.what(), CLR_RED);
			msg.getSender()->sendMessage(nne.what());
			return false;
		}
		else
			msg.getSender()->sendMessage("Select unique nickname first!");
		return false;
	}

	//	2. Check if USER is set
	if (msg.getSender()->getUsername().empty())
	{
		if (msg.getCmd() == "USER" && !msg.getArg(0).empty() && !msg.getArg(1).empty() &&
			!msg.getArg(2).empty() && !msg.getColon().empty())
		{
			msg.getSender()->setUsername(msg.getArg(0));
			msg.getSender()->setFullname(msg.getColon());
		}
		else
			msg.getSender()->sendMessage("Select username first!");
		return false;
	}
	return true;
}

void	Server::chooseCommand(Message &msg, Channel *cnl)
{
	switch (msg.getCmd()[0])
	{
		case 'I':
			// INVITE <client> #<channel>
			invite(msg, cnl);
			break;

		case 'M':
			// MODE #<cnlName> flag
			mode(msg, cnl);
			break;

		case 'K':
			// KICK #<cnlName> <client>
			kick(msg, cnl);
			break;

		case 'J':
			// JOIN #<cnlName>
			join(msg, cnl);
			break;
			
		case 'T':
			// TOPIC #<cnlName> :<topic>
			topic(msg, cnl);
			break;

		case 'P':
		{
			if (msg.getCmd()[1] == 'R') // PRIVMSG
			{
				// PRIVMSG <recepient> <msg>
				privmsg(msg);
			}
			else // PART
			{
				// PART #<cnlName>
				part(msg, cnl);
			}
		}
			break;

		default:
			info("Invalid msg", CLR_RED);
			msg.getSender()->sendMessage("Command is not supported");
			break;
	}
}

void	Server::privmsg(Message &message)
{
	// PRIVMSG <recepient> <message>
	// 1. need to find the recepient
	message.setReceiver(getClientByNick(message.getArg(0)));
	info("privmsg", CLR_BLU);

	if (!message.getSender() || !message.getReceiver())
	{
		info("Sender/Recepient not found", CLR_RED);
		// TODO: send a message to the sender that the recepient was not found
		return;
	}
	info("privmsg2", CLR_BLU);

	// 2. send the message to the recepient
	std::string ircMessage = 
		":" + message.getSender()->getNickname() + "!" +
		message.getSender()->getUsername() +
		"@HELLOWORLD PRIVMSG " +
		message.getReceiver()->getNickname() +
		" :" +
		message.getArg(1) + "\r\n";
	// std::cout << "HERE\n" << ircMessage << "\n";
	// :anshovah_!anshovah@F456A.75198A.60D2B2.ADA236.IP PRIVMSG astein :joao is lazyao
	message.getReceiver()->sendMessage(ircMessage);
}

void	Server::join(Message &message, Channel *channel)
{
	if (!channel)
		channel = createNewChannel(message);
	if (!channel)
	{
		info ("STRANGE CASE: COULD NOT FIND NOR CREATE CHANNEL", CLR_RED);
		message.getSender()->sendMessage("Could not create channel"); 
		return ;
	}
	message.getSender()->joinChannel(channel);
	channel->addClient(message.getSender());
	info(message.getSender()->getNickname() + " JOINED Channel " + channel->getName(), CLR_GRN);
	//TODO: send messages to the channel that the client has joined
}

void	Server::invite(Message &message, Channel *channel)
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

void	Server::topic(Message &message,  Channel *channel)
{
	(void)message;
	(void)channel;
	// TOPIC #<channelName> :<topic>
	
}

void	Server::mode(Message &message,  Channel *channel)
{
	(void)message;
	(void)channel;
	// MODE #<channelName> flag
}

void	Server::kick(Message &message, Channel *channel)
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

void	Server::part(Message &message, Channel *channel)
{
	message.getSender()->leaveChannel(channel);
	channel->removeClient(message.getSender());
	if (!channel->isActive())
		_channels.remove(*channel);
}

// -----------------------------------------------------------------------------
// Client Methods
// -----------------------------------------------------------------------------
void	Server::addClient(Client client)
{
	_clients.push_back(client);
}

void	Server::removeClient(Client *client)
{
	if (!client)
		return;
	_clients.remove(*client);
	// TODO: CHECK if this calls the client destructor
	// There also the username should be freed again!
	// Print an info that the user was deleted!
}

Client	*Server::getClientByFd(int fd)
{
	for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getSocketFd() == fd)
			return &(*it);
	}
	return NULL;
}

Client	*Server::getClientByNick(const std::string &nickname)
{
	for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getNickname() == nickname)
			return &(*it);
	}
	return NULL;
}

// -----------------------------------------------------------------------------
// Channel Methods
// -----------------------------------------------------------------------------
void	Server::addChannel(Channel &channel)
{
	(void)channel;
	//TODO:
}

void	Server::removeChannel(Channel *channel)
{
	(void)channel;
	//TODO:
}

Channel	*Server::getChannelByName(const std::string &channelName)
{
	if (channelName.empty())
		return NULL;

	// If we have a channel name find
	// Try to find the channel
	for (std::list<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (it->getName() == channelName)
			return &(*it);
	}
	return NULL;
}

Channel	*Server::createNewChannel(Message &msg)
{
	if (getChannelByName(msg.getChannelName()))
		return NULL;
	// Couldn't find the channel -> create it
	_channels.push_back(Channel(msg.getChannelName(), msg.getSender()));
	return &(_channels.back());
}

// -----------------------------------------------------------------------------
// Static Signal handling (for exit with CTRL C)
// -----------------------------------------------------------------------------
volatile sig_atomic_t	Server::_keepRunning = 1;

void	Server::setupSignalHandling()
{
    signal(SIGINT, Server::sigIntHandler);
}

void	Server::sigIntHandler(int sig)
{
	if(sig != SIGINT)
		return;
    _keepRunning = 0;  // Set flag to false to break the loop
    std::cout << CLR_RED << "Shutdown signal received, terminating server..." << CLR_RST << std::endl;
}

// -----------------------------------------------------------------------------
// Standard exception class for server
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
