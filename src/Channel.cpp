/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:46 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/08 17:11:07 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

// Constructor
// -----------------------------------------------------------------------------
Channel::Channel(const std::string &name) : 
	_channelName(name), _topic(""), _key(""), _limit(0), _inviteOnly(false), _topicProtected(false)
{
    Logger::log("Channel CREATED: " + _channelName);
	logChanel();
}

void 	Channel::iniChannel(Client *client)
{
	client->addChannel(this);
    this->addClient(client, STATE_O);

	// SEND JOIN MESSAGE FOR THE CLIENT THAT CREATED THE CHANNEL
	client->sendMessage(":" + client->getUniqueName() + 
		"!" + client->getUsername() + "@localhost" +
		" JOIN " + _channelName + " * :realname");
    Logger::log("Channel INIT: " + _channelName);
	logChanel();
}

// Copy Constructor
// -----------------------------------------------------------------------------
Channel::Channel(const Channel &other) : 
	_channelName(other._channelName),
	_topic(other._topic),
	_topicChange(other._topicChange),
	_key(other._key),
	_limit(other._limit),
	_inviteOnly(other._inviteOnly),
	_topicProtected(other._topicProtected)
{
	Logger::log("Channel COPIED: " + _channelName);
	std::map<Client *, int>::const_iterator it;
	for(it = other._clients.begin(); it != other._clients.end(); ++it)
		_clients.insert(*it);
	logChanel();
}

// Destructor
// -----------------------------------------------------------------------------
Channel::~Channel()
{
	std::map<Client *, int>::const_iterator it;
	for(it = _clients.begin(); it != _clients.end(); ++it)
		it->first->removeChannel(this);
	_clients.clear();
	Logger::log("Channel DESTROYED: " + _channelName);
}

// Equal Overload (for list remove)
// -----------------------------------------------------------------------------
bool	Channel::operator==(const Channel &other) const
{
	return this->_channelName == other._channelName;
}

// So the Server can check if the Channel still has an operator
// -----------------------------------------------------------------------------
bool	Channel::isActive() const
{
	std::map<Client *, int>::const_iterator it;
	for(it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second == STATE_O)
			return true;
	return false;
}

// Members & Operators funtionality
// -----------------------------------------------------------------------------
/*
	This takes a message which HAS to contain:
		- sender
		- key (optional)

	Checks if the channel accepst the client
		Checks flags and stuff
*/
void	Channel::joinChannel(Client *client, const std::string &pswd)
{
	if (!client)
		return ;

	// IF ALREADY IN CHANNEL
	if (getClientState(client) > STATE_I)
	{
		// SEND MESSAGE ALREADY IN CHANNEL
		client->sendMessage(ERR_USERONCHANNEL, client->getUniqueName() + " " +  _channelName + " :is already on channel");
		Logger::log("Client " + client->getUniqueName() + " is already in " + _channelName);
		return ;
	}
	
	// IS K FLAG?
	if (!_key.empty())
	{
		// CHECK IF PASWD IS PROVIDED AND CORRECT
		if (_key != pswd)
		{
			// 475
			return client->sendMessage(ERR_BADCHANNELKEY, _channelName + " :Cannot join channel (+k)");
		}
	}
	
	// IF I FLAG
	if (_inviteOnly)
	{
		// CHECK IF INVITED
		if (getClientState(client) < STATE_I)
		{
			client->sendMessage(ERR_INVITEONLYCHAN, _channelName + " :Cannot join channel (+i)");
			return ;
		}
	}
	
	// IF L FLAG
	if (_limit != 0)
	{
		// CHECK IF CHANNEL IS FULL
		size_t noOfClients = 0;
		std::map<Client *, int>::const_iterator it;
		for(it = _clients.begin(); it != _clients.end(); ++it)
			if (it->second > STATE_I)
				noOfClients++;
		if (noOfClients >= _limit)
			return client->sendMessage(ERR_CHANNELISFULL, _channelName + " :Cannot join channel (+l)");
	}

	// IF WE GOT HERE
	// JOIN CHANNEL
	client->addChannel(this);
	this->addClient(client, STATE_C);

	// CREATE MSG
	std::string msgToSend =
		":" + client->getUniqueName() + 
		"!" + client->getUsername() + "@*" + // + client->getHost()
		" JOIN " + _channelName + " * :realname";

	// INFORM ALL CHANNEL MEMBERS
	// >> :ash222!anshovah@F456A.75198A.60D2B2.ADA236.IP JOIN #qweqwe * :realname
	this->sendMessageToClients(msgToSend);	
	Logger::log("Client " + client->getUniqueName() + " joined " + _channelName);
}

void	Channel::inviteToChannel(Client *host, Client *guest)
{
	// IF HOST IS NOT IN CHANNEL
	if (getClientState(host) < STATE_C)
	{
		host->sendMessage(ERR_NOTONCHANNEL, _channelName + " :You're not on that channel");
		return ;
	}
	
	// IF GUEST ALREADY IN CHANNEL
	if (getClientState(guest) >= STATE_I)
	{
		host->sendMessage(ERR_USERONCHANNEL, guest->getUniqueName() + " " +  _channelName + " :is already on channel");
		return ;
	}
	
	// IF INVITE ONLY (I FLAG)
	if (_inviteOnly)
	{
		if (getClientState(host) < STATE_O)
		{
			host->sendMessage(ERR_CHANOPRIVSNEEDED, _channelName + " :You're not channel operator");
			return ;
		}
	}
	
	// Add guest to the invitation list
	this->addClient(guest, STATE_I);

	// SEND INVITE
	// host :Aurora.AfterNET.Org 341 astein astein__ #test3
	host->sendMessage(RPL_INVITING, host->getUniqueName() + " " +
		guest->getUniqueName() + " " + _channelName);
	
	// guest :astein!alex@F456A.75198A.60D2B2.ADA236.IP INVITE astein__ #test3
	guest->sendMessage(":" + host->getUniqueName() + "!" + host->getUsername() +
		"@localhost" + " INVITE " + guest->getUniqueName() + " " + _channelName);

	Logger::log("Invite sent to " + guest->getUniqueName() + " by " + host->getUniqueName());
}

void	Channel::kickFromChannel(Client *kicker, Client *kicked)
{
	// IF KICKER IS NOT OPERATOR
	if (getClientState(kicker) < STATE_O)
	{
		kicked->sendMessage(ERR_CHANOPRIVSNEEDED, _channelName + " :You're not channel operator");
		return ;
	}
	
	// IF KICKED IS NOT IN CHANNEL
	if (getClientState(kicked) < STATE_C)
	{
		kicked->sendMessage(ERR_USERNOTINCHANNEL, kicked->getUniqueName() + " " + _channelName + " :They aren't on that channel");
		return ;
	}

	// FIRST SEND THE MSG THEN KICK SO THAT THE
	// sendMessageToClients() will INLUDE THE KICKED GUY
	// MSG:
	// :astein!alex@F456A.75198A.60D2B2.ADA236.IP KICK #test3 astein__ :astein
	std::string msg = ":" + kicker->getUniqueName() + "!" + kicker->getUsername() + "@localhost" +
		" KICK " + _channelName + " " + kicked->getUniqueName() + " :" + kicker->getUniqueName();
	this->sendMessageToClients(msg);

	kicked->removeChannel(this);
	this->removeClient(kicked);
	Logger::log("Client " + kicked->getUniqueName() + " kicked from " + _channelName + " by " + kicker->getUniqueName());
}

void	Channel::partChannel(Client *client, const std::string &reason)
{
	// IF CLIENT IS NOT IN CHANNEL
	if (getClientState(client) < STATE_C)
	{
		client->sendMessage(ERR_NOTONCHANNEL, _channelName + " :You're not on that channel");
		return ;
	}

	// FIRST SEND THE MSG THEN PART SO THAT THE
	// sendMessageToClients() will INLUDE THE LEAVING GUY
	// MSG:
	// :astein_!alex@F456A.75198A.60D2B2.ADA236.IP PART #test :Leaving
	std::string r = "Leaving";
	if(!reason.empty())
		r = reason;
	std::string msg = ":" + client->getUniqueName() + "!" + client->getUsername() + "@localhost" +
		" PART " + _channelName + " :" + r;
	this->sendMessageToClients(msg);

	client->removeChannel(this);
	this->removeClient(client);
	Logger::log("Client " + client->getUniqueName() + " left " + _channelName);
}

// Modes & Topic funtionality
// -----------------------------------------------------------------------------
void	Channel::topicOfChannel(Client *sender, const std::string &topic)
{
	// IF SENDER IS NOT IN CHANNEL
	if (getClientState(sender) < STATE_C)
	{
		sender->sendMessage(ERR_NOTONCHANNEL, _channelName + " :You're not on that channel");
		return ;
	}
	
	// IF NO TOPIC IS PROVIDED
	if (topic.empty())
	{
		sender->sendMessage(
			":localhost " + std::string(RPL_TOPIC) +
			" " + sender->getUniqueName() +
			" " + _channelName + " :" + _topic);
		sender->sendMessage(
			":localhost " + std::string(RPL_TOPICADDITIONAL) +
			" " + sender->getUniqueName() +
			" " + _topicChange);
		return ;
	}
	
	// IF TOPIC IS PROTECTED (FLAG T)
	if (_topicProtected)
	{
		if (getClientState(sender) < STATE_O)
		{
			sender->sendMessage(ERR_CHANOPRIVSNEEDED, _channelName + " :You're not channel operator");
			return ;
		}
	}
	// CHANGE TOPIC
	_topic = topic;

	// GET THE TOPIC CHANGED STRING
	std::time_t currentTime = std::time(0);
	// Convert time_t to string using stringstream unix timestamp
	std::stringstream ss;
	ss << currentTime;
	_topicChange = sender->getUniqueName() + "!" + 
			sender->getUsername() + "@localhost" +
			" " + ss.str();
	// SEND TOPIC MESSAGE
	std::string confirmMsg =
		sender->getUniqueName() + "!" +
		sender->getUsername() + "@localhost" +
		" TOPIC " + _channelName + " :" + _topic;

	sendMessageToClients(confirmMsg);
	sender->sendMessage(confirmMsg);

	Logger::log("Topic changed to: " + _topic + " by " + sender->getUniqueName());
	Logger::log("Topic change message: " + _topicChange);
}

void	Channel::modeOfChannel(/* TODO: */)
{
	// TODO:
}

// Simple List Management
// -----------------------------------------------------------------------------
// IF CLIENT ALREADY EXISTS, IT WILL UPDATE THE STATUS
void	Channel::addClient		(Client *client, int status)
{
	_clients[client] = status;
	Logger::log("Channel " + _channelName + " added/changed client: " + client->getUniqueName() + " to status " + to_string(status));
}

void	Channel::removeClient	(Client *client)
{
	_clients.erase(client);
}

// Channel Broadcast Message
// -----------------------------------------------------------------------------
// If sender is provided, it will not send the message to the sender
void	Channel::sendMessageToClients(const std::string &ircMessage, Client *sender) const
{
    std::map<Client *, int>::const_iterator it;
	for(it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (sender && *it->first == *sender)
			continue ;
		it->first->sendMessage(ircMessage);
	}
	std::string logMsg ="Channel " + _channelName + " sent message to all clients";
	if(sender)
		logMsg += " except " + sender->getUniqueName();
	logMsg += ": " + ircMessage;
	Logger::log(logMsg);
}

// Getters and Setters
// -----------------------------------------------------------------------------
const std::string	&Channel::getUniqueName() const
{
    return _channelName;
}

const std::string Channel::getClientList() const
{
	std::string users = "";

	std::map<Client *, int>::const_iterator it;
	for(it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second > STATE_I)
		{
			users += "@";
			users += it->first->getUniqueName();
			users += " ";
		}
	}
	Logger::log("Created user (clients & operators) list for channel " + _channelName + ": " + users);
	return users;
}

// LOG
// -----------------------------------------------------------------------------
void Channel::logChanel() const
{
	std::ostringstream header, values;
	
	// Log headers
	header << std::setw(15) << std::left << "NAME"
			<< "| " << std::setw(15) << "TOPIC"
			<< "| " << std::setw(15) << "TOPIC CHANGE"
			<< "| " << std::setw(15) << "KEY"
			<< "| " << std::setw(15) << "LIMIT"
			<< "| " << std::setw(15) << "INVITE ONLY"
			<< "| " << std::setw(15) << "TOPIC PROTECTED"
			<< "| " << std::setw(15) << "CLIENTS" << "\n";

	// Log values
	values << std::setw(15) << std::left << (_channelName.length() > 14 ? _channelName.substr(0, 14) + "." : _channelName.empty() ? "(NULL)" : _channelName)
			<< "| " << std::setw(15) << (_topic.length() > 14 ? _topic.substr(0, 14) + "." : _topic.empty() ? "(NULL)" : _topic)
			<< "| " << std::setw(15) << (_topicChange.length() > 14 ? _topicChange.substr(0, 14) + "." : _topicChange.empty() ? "(NULL)" : _topicChange)
			<< "| " << std::setw(15) << (_key.length() > 14 ? _key.substr(0, 14) + "." : _key.empty() ? "(NULL)" : _key)
			<< "| " << std::setw(15) << (_limit == 0 ? "(unset)" : to_string(_limit))
			<< "| " << std::setw(15) << (_inviteOnly ? "Yes" : "No")
			<< "| " << std::setw(15) << (_topicProtected ? "Yes" : "No")
			<< "| " << getClientList() << std::endl;


        // Simulate logging (You can replace this with actual log calls)
	Logger::log("================ START CHANNEL ================");
    Logger::log(header.str());
    Logger::log(values.str());
	Logger::log("================ END CHANNEL ================");
}

// Private Methods
// -----------------------------------------------------------------------------

// Checks the state of a client
// -----------------------------------------------------------------------------
// 	-1: Client not in the list
// 	 0: Client is only invited
// 	 1: Client is in the list
// 	 2: Client is an operator
int	Channel::getClientState(const Client *client) const
{
	if (_clients.empty() || !client)
		return -1;
	std::map<Client *, int>::const_iterator it;

	for (it = _clients.begin(); it != _clients.end(); ++it)
		// Check if the client is in the list
		if (*client == *it->first)
			return it->second;
	return -1;
}
