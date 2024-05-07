/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:46 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/07 23:40:06 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

// Constructor
// -----------------------------------------------------------------------------
Channel::Channel(const std::string &name, Client *client) : 
	_name(name), _topic(""), _key(""), _limit(0), _inviteOnly(false), _topicProtected(false)
{
    Logger::log("Channel CREATED: " + _name);
	logChanel();
	if (!client)
	{
		Logger::log("ERROR Trying to create a channel without a valid client pointer!");
		return;
	}
}

void 	Channel::iniChannel(Client *client)
{
	client->addChannel(this);
    this->addClient(client);
	this->addOperator(client);

	// SEND JOIN MESSAGE FOR THE CLIENT THAT CREATED THE CHANNEL
	client->sendMessage(":" + client->getUniqueName() + 
		"!" + client->getUsername() + "@localhost" +
		" JOIN " + _name + " * :realname");
    Logger::log("Channel INIT: " + _name);
	logChanel();
}

// Copy Constructor
// -----------------------------------------------------------------------------
Channel::Channel(const Channel &other) : 
	_name(other._name),
	_topic(other._topic),
	_topicChange(other._topicChange),
	_key(other._key),
	_limit(other._limit),
	_inviteOnly(other._inviteOnly),
	_topicProtected(other._topicProtected)
{
	Logger::log("Channel COPIED: " + _name);
	for(std::list<Client *>::const_iterator it = other._clients.begin(); it != other._clients.end(); ++it)
		_clients.push_back(*it);
	for(std::list<Client *>::const_iterator it = other._operators.begin(); it != other._operators.end(); ++it)
		_operators.push_back(*it);
	for(std::list<Client *>::const_iterator it = other._invitations.begin(); it != other._invitations.end(); ++it)
		_invitations.push_back(*it);
	logChanel();
}
// Destructor
// -----------------------------------------------------------------------------
Channel::~Channel()
{
    std::list<Client *>::iterator it;
	for(it = _invitations.begin(); it != _invitations.end(); ++it)
		(*it)->removeChannel(this);
	for(it = _clients.begin(); it != _clients.end(); ++it)
		(*it)->removeChannel(this);
	for(it = _operators.begin(); it != _operators.end(); ++it)
		(*it)->removeChannel(this);
	_invitations.clear();
    _clients.clear();
	_operators.clear();
	Logger::log("Channel DESTROYED: " + _name);
}

// Equal Overload (for list remove)
// -----------------------------------------------------------------------------
bool	Channel::operator==(const Channel &other) const
{
	return this->_name == other._name;
}

// So the Server can check if the Channel still has an operator
// -----------------------------------------------------------------------------
bool	Channel::isActive() const
{
	if(_operators.size() == 0)
		return false;
	if(_clients.size() == 0)
		return false;
	return true;
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
	if (isClientIsInList(_clients, client) || isClientIsInList(_operators, client))
	{
		// SEND MESSAGE ALREADY IN CHANNEL
		client->sendMessage(ERR_USERONCHANNEL, client->getUniqueName() + " " +  _name + " :is already on channel");
		Logger::log("Client " + client->getUniqueName() + " is already in " + _name);
		return ;
	}
	
	// IS K FLAG?
	if (!_key.empty())
	{
		// CHECK IF PASWD IS PROVIDED AND CORRECT
		if (_key != pswd)
		{
			// 475
			return client->sendMessage(ERR_BADCHANNELKEY, _name + " :Cannot join channel (+k)");
		}
	}
	// IF I FLAG
	if (_inviteOnly)
	{
		// CHECK IF INVITED
		// TODO: check if this sender is invited
		client->sendMessage(ERR_INVITEONLYCHAN, _name + " :Cannot join channel (+i)");
		return ;
	}

	// IF L FLAG
	if (_limit != 0)
	{
		// CHECK IF CHANNEL IS FULL
		if (_clients.size() != 0 && _clients.size() >= _limit)
			return client->sendMessage(ERR_CHANNELISFULL, _name + " :Cannot join channel (+l)");
	}

	// IF WE GOT HERE
	// JOIN CHANNEL
	client->addChannel(this);
	this->addClient(client);

	// CREATE MSG
	std::string msgToSend =
		":" + client->getUniqueName() + 
		"!" + client->getUsername() + "@*" + // + client->getHost()
		" JOIN " + _name + " * :realname";

	// INFORM ALL CHANNEL MEMBERS
	// >> :ash222!anshovah@F456A.75198A.60D2B2.ADA236.IP JOIN #qweqwe * :realname
	this->sendMessageToClients(msgToSend);	
	info ("Client " + client->getUniqueName() + " joined " + _name, CLR_GRN);
}

void	Channel::inviteToChannel(Client *host, Client *guest)
{
	// IF HOST IS NOT IN CHANNEL
	if (!isClientIsInList(_clients, host))
	{
		host->sendMessage(ERR_NOTONCHANNEL, _name + " :You're not on that channel");
		return ;
	}

	// IF GUEST ALREADY IN CHANNEL
	if (isClientIsInList(_operators, guest) || isClientIsInList(_clients, guest))
	{
		host->sendMessage(ERR_USERONCHANNEL, guest->getUniqueName() + " " +  _name + " :is already on channel");
		return ;
	}
	
	// IF INVITE ONLY (I FLAG)
	if (_inviteOnly)
	{
		if (!isClientIsInList(_operators, host))
		{
			host->sendMessage(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
			return ;
		}
	}
	
	// Add guest to the invitation list
	this->addInvitation(guest);

	// SEND INVITE
	// host :Aurora.AfterNET.Org 341 astein astein__ #test3
	host->sendMessage(RPL_INVITING, host->getUniqueName() + " " +
		guest->getUniqueName() + " " + _name);
	
	// guest :astein!alex@F456A.75198A.60D2B2.ADA236.IP INVITE astein__ #test3
	guest->sendMessage(":" + host->getUniqueName() + "!" + host->getUsername() +
		"@localhost" + " INVITE " + guest->getUniqueName() + " " + _name);

	info ("Invite sent to " + guest->getUniqueName() + " by " + host->getUniqueName(), CLR_GRN);
}

void	Channel::kickFromChannel(Client *kicker, Client *kicked)
{
	// IF CLIENT IS NOT IN CHANNEL
	if (!isClientIsInList(_operators, kicked) && !isClientIsInList(_clients, kicked))
	{
		kicked->sendMessage(ERR_USERNOTINCHANNEL, kicked->getUniqueName() + " " + _name + " :They aren't on that channel");
		return ;
	}
	
	// IF CLIENT IS NOT OPERATOR
	if (!isClientIsInList(_operators, kicked))
	{
		kicked->sendMessage(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
		return ;
	}

	// FIRST SEND THE MSG THEN KICK SO THAT THE
	// sendMessageToClients() will INLUDE THE KICKED GUY
	// MSG:
	// :astein!alex@F456A.75198A.60D2B2.ADA236.IP KICK #test3 astein__ :astein
	std::string msg = ":" + kicker->getUniqueName() + "!" + kicker->getUsername() + "@localhost" +
		" KICK " + _name + " " + kicked->getUniqueName() + " :" + kicker->getUniqueName();
	this->sendMessageToClients(msg);

	this->removeOperator(kicked);
	this->removeClient(kicked);
	kicked->removeChannel(this);
	info ("Client " + kicked->getUniqueName() + " kicked from " + _name + " by " + kicker->getUniqueName(), CLR_RED);
}

void	Channel::partChannel(Client *client)
{
	// IF CLIENT IS NOT IN CHANNEL
	if (!isClientIsInList(_operators, client) && !isClientIsInList(_clients, client))
	{
		client->sendMessage(ERR_NOTONCHANNEL, _name + " :You're not on that channel");
		return ;
	}

	// FIRST SEND THE MSG THEN PART SO THAT THE
	// sendMessageToClients() will INLUDE THE LEAVING GUY
	// MSG:
	// :astein_!alex@F456A.75198A.60D2B2.ADA236.IP PART #test :Leaving
	std::string msg = ":" + client->getUniqueName() + "!" + client->getUsername() + "@localhost" +
		" PART " + _name + " :Leaving";
	this->sendMessageToClients(msg);
	this->removeOperator(client);
	this->removeClient(client);
	client->removeChannel(this);
	info ("Client " + client->getUniqueName() + " left " + _name, CLR_ORN);
}

// Modes & Topic funtionality
// -----------------------------------------------------------------------------
void	Channel::topicOfChannel(Client *sender, const std::string &topic)
{
	// IF SENDER IS NOT IN CHANNEL
	if (!isClientIsInList(_operators, sender) && !isClientIsInList(_clients, sender))
	{
		sender->sendMessage(ERR_NOTONCHANNEL, _name + " :You're not on that channel");
		return ;
	}
	
	// IF NO TOPIC IS PROVIDED
	if (topic.empty())
	{
		sender->sendMessage(
			":localhost " + std::string(RPL_TOPIC) +
			" " + sender->getUniqueName() +
			" " + _name + " :" + _topic);
		sender->sendMessage(
			":localhost " + std::string(RPL_TOPICADDITIONAL) +
			" " + sender->getUniqueName() +
			" " + _topicChange);
		return ;
	}
	
	// IF TOPIC IS PROTECTED (FLAG T)
	if (_topicProtected)
	{
		if (!isClientIsInList(_operators, sender))
		{
			sender->sendMessage(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
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
		" TOPIC " + _name + " :" + _topic;

	sendMessageToClients(confirmMsg);
	sender->sendMessage(confirmMsg);

	info ("Topic changed to: " + _topic + " by " + sender->getUniqueName(), CLR_GRN);
	info ("Topic change message: " + _topicChange, CLR_GRN);
}

void	Channel::modeOfChannel(/* TODO: */)
{
	// TODO:
}

// Simple List Management
// -----------------------------------------------------------------------------
void	Channel::addClient		(Client *client)
{
	_clients.push_back(client);
	Logger::log("Channel " + _name + " added client: " + client->getUniqueName());
	removeInvitation(client);
}

void	Channel::removeClient	(Client *client)
{
	_clients.remove(client);
}

void	Channel::addOperator	(Client *client)
{
	_operators.push_back(client);
	Logger::log("Channel " + _name + " added client as operator: " + client->getUniqueName());
}

void	Channel::removeOperator	(Client *client)
{
	_operators.remove(client);
}		

void	Channel::addInvitation	(Client *client)
{
	_invitations.push_back(client);
}

void	Channel::removeInvitation(Client *client)
{
	_invitations.remove(client);
}

// Channel Broadcast Message
// -----------------------------------------------------------------------------
void	Channel::sendMessageToClients(const std::string &ircMessage, Client *sender) const
{
    std::list<Client *>::const_iterator it;

	for (it = _operators.begin(); it != _operators.end(); ++it)
    {
		if (sender && **it != *sender)
        	(*it)->sendMessage(ircMessage);
    }
	for (it = _clients.begin(); it != _clients.end(); ++it)
    {
		if (sender && **it != *sender)
        	(*it)->sendMessage(ircMessage);
    }
}

// Getters and Setters
// -----------------------------------------------------------------------------
const std::string	&Channel::getUniqueName() const
{
    return _name;
}

const std::string Channel::getClientList() const
{
	std::string users = "";

	if(_operators.empty())
		return users;

	for (std::list<Client *>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		users += "@";
		users += (*it)->getUniqueName();
		users += " ";
		Logger::log("appending client list for channel " + _name + ": " + users);
	}
	if(_clients.empty())
		return users;

	for (std::list<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		users += "@";
		users += (*it)->getUniqueName();
		users += " ";
		Logger::log("appending client list for channel " + _name + ": " + users);
	}
	Logger::log("Created client list for channel " + _name + ": " + users);
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
	values << std::setw(15) << std::left << (_name.length() > 14 ? _name.substr(0, 14) + "." : _name.empty() ? "(NULL)" : _name)
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
bool	Channel::isClientIsInList(std::list<Client *> list, const Client *client) const
{
	if (list.empty() || !client)
		return false;
	std::list<Client *>::const_iterator it;

	for (it = list.begin(); it != list.end(); ++it)
	{
		if (*client == **it)
			return true;
	}
	return false;
}
