/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:11 by astein            #+#    #+#             */
/*   Updated: 2024/05/02 02:50:13 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Constructor
Server::Server(const std::string &port, const std::string &password) throw(std::exception)
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
void Server::initNetwork() throw(std::exception)
{
	info("[START] Init network", CLR_YLW);

	
	info("[>DONE] Init network", CLR_GRN);
}

void Server::goOnline()
{
	info("[START] Go online", CLR_YLW);
	while (_keepRunning)
	{
		// TODO: DELETE LATER
		pause();
	}
	info("[>DONE] Go online", CLR_GRN);
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
void Server::parseArgs(const std::string &port, const std::string &password)
	throw(std::exception)
{
	int		portInt;
	
	try
	{
		info("Port is not a number!", CLR_RED);
		portInt = std::atoi(port.c_str());
	}
	catch (std::exception &e)
	{
		throw std::exception();
	}

	// 194 is the default port for IRC
	// https://en.wikipedia.org/wiki/Port_(computer_networking)
	// The ports up to 49151 are not as strictly controlled
	// The ports from 49152 to 65535 are called dynamic ports
	if (portInt != 194 && (portInt < 1024 || portInt > 65535))
	{
		info("Port is not the IRC port (194) or in the range 1024-65535!", CLR_RED);
		throw std::exception();
	}
	_port = port;
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
				client.addChannel(channel);
				channel.addClient(client);
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
			client.leaveChannel(channel);
			channel.removeClient(client);
			if (!channel.isActive())
				_channels.remove(channel);
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
    _keepRunning = 0;  // Set flag to false to break the loop
    std::cout << "Shutdown signal received, terminating server..." << std::endl;
	// TODO: Implement the shutdown logic
}

void Server::setupSignalHandling()
{
    signal(SIGINT, Server::sigIntHandler);
}