/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:11 by astein            #+#    #+#             */
/*   Updated: 2024/05/08 22:03:22 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// -----------------------------------------------------------------------------
// Construction / Destruction
// -----------------------------------------------------------------------------
Server::Server(const std::string &port, const std::string &password) : _serverIP("localhost")
{
	// Initialize the list of allowed cmds
	_cmds["NICK"] = &Server::nick;
    _cmds["USER"] = &Server::user;
    _cmds["WHOIS"] = &Server::whois;
    _cmds["PRIVMSG"] = &Server::privmsg;
    _cmds["JOIN"] = &Server::join;
    _cmds["INVITE"] = &Server::invite;
    _cmds["TOPIC"] = &Server::topic;
    _cmds["MODE"] = &Server::mode;
    _cmds["KICK"] = &Server::kick;
    _cmds["PART"] = &Server::part;
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
			_clients.push_back(Client(new_socket));
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
				if (!cur_client)
					continue ; // should never happen by arcitechture
					
                int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                if (valread <= 0)
				{
					// Some read error happend
					// The server doesn't bother to much and just deletes this client
					info("Client" + cur_client->getUniqueName() + " disconnected", CLR_RED);
					Logger::log("Client" + cur_client->getUniqueName() + " disconnected");
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);		// erase the client fd from the fd vector
					_clients.remove(*cur_client);	// erase the client from the client list
                    --i; // Adjust loop counter since we removed an element
                }
				else
				{
                    buffer[valread] = '\0';
					// Since the buffer could only be a part of a msg we
					// 1. append it to the client buffer
					if (!cur_client->appendBuffer(buffer))
					{
						cur_client->sendMessage("Message was to long and will be deleted");
						// The msg was to long
						// The full messages will be deleted and the client will be informed
					}
					// 2. get the full msg(s) from the client buffer
					std::string fullMsg;
					while (!(fullMsg = cur_client->getFullMessage()).empty())
					{
						// 3. process the msg(s)
						Logger::log("start processing msg from " + cur_client->getUniqueName() + " -> " + fullMsg);
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

void	Server::broadcastMessage(const std::string &msg) const
{
	info("[START] Broadcast msg", CLR_YLW);
	std::string ircMessage = "TODO:!" + msg + "TODO:!";
	
	// Send it to all clients
	for (std::list<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		// TODO: Create the ircMessage in right format! aka adding the user's name
		it->sendMessage(ircMessage);
	}

	// Send it to all channels
	for (std::list<Channel>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		it->sendMessageToClients(ircMessage);
	}
	info("[>DONE] Broadcast msg", CLR_GRN);
}

// -----------------------------------------------------------------------------
// Processing the Messages
// -----------------------------------------------------------------------------
void	Server::processMessage(Client *sender, const std::string &ircMessage)
{
	// Parse the IRC Message
	Message     msg(sender, ircMessage);
	
	//Execute IRC Message
	//	1. Check if CLIENT is loggedin
	if (!isLoggedIn(&msg))
		return ;
	
	//	2. Execute normal commands
	//		3.1. Find the channel if there is  channelname in the msg
	msg.setChannel(getInstanceByName(_channels, msg.getChannelName()));
	
	// 		3.2 Process the msg aka call the right function
	chooseCommand(&msg);
}

bool	Server::isLoggedIn(Message *msg)
{
	if (!msg->getSender()->getUniqueName().empty() && !msg->getSender()->getUsername().empty())
		return true;

	//	1. Check if NICK is set
	if (msg->getCmd() == "NICK")
		nick(msg);
	else if (msg->getCmd() == "USER")
		user(msg);
	else
		msg->getSender()->sendMessage(ERR_NOTREGISTERED, ":You have not registered");
	return false;
}

void	Server::chooseCommand(Message *msg)
{
	std::string cmd = msg->getCmd();
	if (cmd.empty())
		return ; // this should not happen by design

	for (std::map<std::string, CommandFunction>::iterator it = _cmds.begin(); it != _cmds.end(); ++it)
	{
		if (cmd == it->first)
		{
			(this->*(it->second))(msg);
			return ;
		}
	}
	// :10.11.3.6 421 anshovah_ PRIMSG :Unknown command
	msg->getSender()->sendMessage(ERR_UNKNOWNCOMMAND, msg->getCmd() + " :Unknown command");
}

// /NICK
void	Server::nick(Message *msg)
{	
	std::string oldNickname = msg->getSender()->getUniqueName();
	std::string newNickname = msg->getArg(0);
	bool 		isFirstNick = oldNickname.empty();

	if (oldNickname.empty())
		oldNickname = newNickname;

	if (newNickname.empty())
		msg->getSender()->sendMessage(ERR_NONICKNAMEGIVEN, ":No nickname given");
	else if (!isNameAvailable(_clients, newNickname))
		msg->getSender()->sendMessage(ERR_NICKNAMEINUSE, oldNickname + " " + newNickname + " :Nickname is already in use");
	else
	{
		msg->getSender()->setUniqueName(newNickname);
		std::string ircMessage = 
			":" + oldNickname + "!" +
			msg->getSender()->getUsername() +
			"@localhost NICK :" +
			newNickname;
		msg->getSender()->sendMessage(ircMessage);

		// CHECK IF NEED tO SEND A WELCOME MSG NOW
		if (isFirstNick && !msg->getSender()->getUsername().empty())
		{
			//:luna.AfterNET.Org 001 ash_ :Welcome to the FINISHERS' IRC Network, ash_
			msg->getSender()->sendMessage(RPL_WELCOME, msg->getSender()->getUniqueName() + " :Welcome to FINISHERS' IRC Network, " + msg->getSender()->getUniqueName());
		}
	}
}

void	Server::user(Message *msg)
{
	std::string oldUsername = msg->getSender()->getUsername();
	std::string newUsername = msg->getArg(0);
	
	// CAN'T RE-REGISTER!
	if(!oldUsername.empty())
	{
		msg->getSender()->sendMessage(ERR_ALREADYREGISTRED, msg->getSender()->getUniqueName() + " :You may not reregister");
		return ;
	}

	if (!msg->getArg(0).empty() && !msg->getArg(1).empty() &&
		!msg->getArg(2).empty() && !msg->getColon().empty())
	{
		msg->getSender()->setUsername(msg->getArg(0));
		msg->getSender()->setFullname(msg->getColon());
		// CHECK IF NEED tO SEND A WELCOME MSG NOW
		if (oldUsername.empty() && !msg->getSender()->getUniqueName().empty())
			msg->getSender()->sendMessage(RPL_WELCOME, msg->getSender()->getUniqueName() + " :Welcome to " + std::string(PROMT) + ", " + msg->getSender()->getUniqueName());
	}
	else
		msg->getSender()->sendMessage(ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
}

void	Server::whois	(Message *msg)
{
	if (msg->getArg(0).empty())
	{
		msg->getSender()->sendMessage(ERR_NONICKNAMEGIVEN, ":No nickname given");
		return ;
	}
	Client *whoIsClient = getClientByNick(msg->getArg(0));
	if (!whoIsClient)
	{
		msg->getSender()->sendMessage(ERR_NOSUCHNICK, msg->getArg(0) + " :No such nick/channel");
		return ;
	}

	whoIsClient->sendWhoIsMsg(msg->getSender());
}

/* 
	receiver not found				ERR_NOSUCHNICK
		inform sender about it
	send msg to receiver
 */
void	Server::privmsg(Message *msg)
{
	std::string recipientNick 	= msg->getArg(0);
	std::string channelName 	= msg->getChannelName();

	// IF CHANNEL AND RECEIPENT BOTH ARE GIVEN DON'T DO SHIT
	if (!channelName.empty() && !recipientNick.empty())
		return ;

	// IF CHANNEL AND RECEIPENT BOTH ARE NOT GIVEN
	if (channelName.empty() && recipientNick.empty())
	{
		msg->getSender()->sendMessage(ERR_NORECIPIENT, ":No recipient given (PRIVMSG)");
		return ;
	}

	// NO TEXT TO SEND
	if (msg->getColon().empty())
	{
		msg->getSender()->sendMessage(ERR_NOTEXTTOSEND, ":No text to send");
		return ;
	}

	// CASE RECIPIENT
	if (!recipientNick.empty())
	{
		msg->setReceiver(getInstanceByName(_clients, recipientNick));
		if (!msg->getReceiver())
		{
			msg->getSender()->sendMessage(ERR_NOSUCHNICK, recipientNick + " :No such nick");
			return ;
		}
		std::string ircMessage = 
			":" + msg->getSender()->getUniqueName() + "!" +
			msg->getSender()->getUsername() +
			"@localhost PRIVMSG " +
			recipientNick +
			" :" +
			msg->getColon();
		msg->getReceiver()->sendMessage(ircMessage);
		return ;
	}

	// CASE CHANNEl
	if (!channelName.empty())
	{
		if (!msg->getChannel())
		{
			msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, channelName + " :No such channel");
			return ;
		}
		std::string ircMessage = 
			":" + msg->getSender()->getUniqueName() + "!" +
			msg->getSender()->getUsername() +
			"@localhost PRIVMSG " +
			channelName +
			" :" +
			msg->getColon();
		msg->getChannel()->sendMessageToClients(ircMessage, msg->getSender());
	}
}

void	Server::join(Message *msg)
{
	std::string channelName = msg->getChannelName();
	// JOIN #<channel>
	
	// WITHOUT ARGS
	if (channelName.empty())
	{
		// NO HASHTAG
		if (!msg->getArg(0).empty())
			msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, "JOIN :Channel name has to start with '#'");
		else
			msg->getSender()->sendMessage(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameter");
		return ;
	}
	
	// CHECK IF CHANNEL EXISTS
	if(!msg->getChannel())
	{
		if(!createNewChannel(msg))
			return ; //Smth wrong we could find the channel but we also could create it
		std::cout << msg->getSender()->getChannelList() << std::endl;
		return ;
	}
	
	// LET THE CHANNEL DESIDE IF THE CLIENT CAN JOIN
	msg->getChannel()->joinChannel(msg->getSender(), msg->getArg(0));
	std::cout << msg->getSender()->getChannelList() << std::endl;
}

void	Server::invite(Message *msg)
{
	// INVITE <nick> <channel>
	std::string guestNick 		= msg->getArg(0);
	std::string channelName 	= msg->getChannelName();

	// IF GUESTNICK IS NOT GIVEN DON'T DO SHIT
	if (guestNick.empty())
		return ;

	// IF THE GUEST IS NOT ON THE SERVER
	msg->setReceiver(getInstanceByName(_clients, guestNick));
	if (!msg->getReceiver())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHNICK, guestNick + " :No such nick");
		return ;
	}

	// IF CHANNEL NAME IS NOT PROVIDED 
	if (channelName.empty())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, std::string(PROMT) + " :No such channel");
		return ;
	}

	// IF CHANNEL DOES NOT EXIST
	if (!msg->getChannel())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, channelName + " :No such channel");
		return ;
	}
		
	// CALL THE CHANNEL FUNCTION invite() AND LET IT DECIDE WHAT TO DO
	msg->getChannel()->inviteToChannel(msg->getSender(), msg->getReceiver());
}

void	Server::topic(Message *msg)
{
	// TOPIC #<channelName> :<topic>
	// TOPIC #<channelName>
	
	if (!msg->getChannel())
		msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, msg->getChannelName() + " :No such channel");
	msg->getChannel()->topicOfChannel(msg->getSender(), msg->getColon());
}

void	Server::mode(Message *msg)
{
	// MODE #<channelName> flag

	// IF CHANNEL NAME IS NOT PROVIDED
	if (msg->getChannelName().empty())
	{
		msg->getSender()->sendMessage(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
		return ;
	}

	// IF CHANNEL DOES NOT EXIST
	if (!msg->getChannel())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, msg->getChannelName() + " :No such channel");
		return ;
	}

	msg->getChannel()->modeOfChannel(msg->getSender(), msg->getArg(0), msg->getArg(1));
}

void	Server::kick(Message *msg)
{
	// IF CHANNEL NAME IS NOT PROVIDED
	if (msg->getChannelName().empty())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, msg->getChannelName() + " :No such channel");
		return ;
	}

	// IF CHANNEL DOES NOT EXIST
	if (!msg->getChannel())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, msg->getChannelName() + " :No such channel");
		return ;
	}
	
	// IF NO CLIENT NAME IS PROVIDED
	if (!msg->getArg(0).empty())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, msg->getChannelName() + " :No such channel");
		return ;
	}
	
	// IF TO BE KICKED CLIENT IS NOT ON THE SERVER
	if (!msg->getReceiver())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHNICK, msg->getArg(0) + " :No such nick");
		return ;
	}

	// KICK THE CLIENT
	msg->getChannel()->kickFromChannel(msg->getSender(), msg->getReceiver());
}

void	Server::part(Message *msg)
{
	// IF CHANNEL NAME IS NOT PROVIDED
	if (msg->getChannelName().empty())
	{
		msg->getSender()->sendMessage(ERR_NOSUCHCHANNEL, msg->getChannelName() + " :No such channel");
		return ;
	}
	msg->getChannel()->partChannel(msg->getSender(), msg->getColon());

	// IF NO CLIENTS OR OPERATORS LEFT IN CHANNEL -> DELETE CHANNEL
	if (!msg->getChannel()->isActive())
	{
		// DELETE CHANNEL. NO MESSAGE NEEDED
		_channels.remove(*msg->getChannel());
	}	
}

// -----------------------------------------------------------------------------
// Client Methods
// -----------------------------------------------------------------------------
Client	*Server::getClientByFd(int fd)
{
	if (_clients.empty())
	{
		Logger::log("ERROR: Tries to get an client pointer on an empty client server list!");
		return NULL;
	}

	for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getSocketFd() == fd)
		{
			Logger::log("getClientByFd() found client in server list!");
			return &(*it);
		}
	}
	Logger::log("ERROR: Couldnt find client with FD in server client list!");
	return NULL;
}

Client	*Server::getClientByNick(const std::string &nickname)
{
	for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getUniqueName() == nickname)
			return &(*it);
	}
	return NULL;
}

// -----------------------------------------------------------------------------
// Channel Methods
// -----------------------------------------------------------------------------
void	Server::addChannel(const Channel *channel)
{
	_channels.push_back(*channel);
}

void	Server::removeChannel(Channel *channel)
{
	_channels.remove(*channel);
}

// If there is no channel this function
// create it and returns a pointer to the new channel
Channel	*Server::createNewChannel(Message *msg)
{
	Logger::log("Trying to create a new channel: " + msg->getChannelName());
	if (isNameAvailable(_channels, msg->getChannelName()))
	{
		_channels.push_back(Channel(msg->getChannelName()));
		_channels.back().iniChannel(msg->getSender());
		Logger::log("Server created new channel named: " + _channels.back().getUniqueName());
		return &(_channels.back());
	}
	return NULL;
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
ServerException::ServerException(const std::string &msg) : _msg(msg)
{
	// Nothing to do
}

ServerException::~ServerException() throw()
{
	// Nothing to do
}

const char *ServerException::what(void) const throw()
{
    return (_msg.c_str());
}
