/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:46 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/07 00:17:23 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

// Constructor
// -----------------------------------------------------------------------------
Channel::Channel(const std::string &name, Client &client) : 
	_name(name), _topic(""), _key(""), _limit(0), _inviteOnly(false), _topicProtected(false)
{
    Logger::log("Channel created: " + _name);
    client.addChannel(this);
    _clients.push_back(&client);
	_operators.push_back(&client);

	// SEND JOIN MESSAGE FOR THE CLIENT THAT CREATED THE CHANNEL
	client.sendMessage(":" + client.getUniqueName() + 
		"!" + client.getUsername() + "@localhost" + // + msg.getSender().getHost()
		" JOIN " + _name + " * :realname");
	info ("Channel created: " + _name + " by " + client.getUniqueName(), CLR_GRN);
}

// Destructor
// -----------------------------------------------------------------------------
Channel::~Channel()
{
    std::list<Client *>::iterator it = _clients.begin();

    while (it != _clients.end())
    {
        (*it)->removeChannel(this);
        it++;
    }
    _clients.clear();
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
void	Channel::joinChannel(Client &client, const std::string &pswd)
{
	// IF ALREADY IN CHANNEL
	if (isClientIsInList(_clients, client))
	{
		// SEND MESSAGE ALREADY IN CHANNEL |
		client.sendMessage(ERR_USERONCHANNEL,
			client.getUniqueName() + " " +  _name + " :is already on channel");
		return ;
	}
	
	// IS K FLAG?
	if (!_key.empty())
	{
		// CHECK IF PASWD IS PROVIDED AND CORRECT
		if (_key != pswd)
		{
			// 475
			return client.sendMessage(ERR_BADCHANNELKEY, _name + " :Cannot join channel (+k)");
		}
	}
	// IF I FLAG
	if (_inviteOnly)
	{
		// CHECK IF INVITED
		// TODO: check if this sender is invited
		client.sendMessage(ERR_INVITEONLYCHAN, _name + " :Cannot join channel (+i)");
		return ;
	}

	// IF L FLAG
	if (_limit != 0)
	{
		// CHECK IF CHANNEL IS FULL
		if (_clients.size() != 0 && _clients.size() >= _limit)
			return client.sendMessage(ERR_CHANNELISFULL, _name + " :Cannot join channel (+l)");
	}

	// IF WE GOT HERE
	// JOIN CHANNEL
    _clients.push_back(&client);
	client.addChannel(this);

	// CREATE MSG
	std::string msgToSend =
		":" + client.getUniqueName() + 
		"!" + client.getUsername() + "@*" + // + client.getHost()
		" JOIN " + _name + " * :realname";

	// INFORM ALL CHANNEL MEMBERS
	// >> :ash222!anshovah@F456A.75198A.60D2B2.ADA236.IP JOIN #qweqwe * :realname
	this->sendMessageToClients(msgToSend);	
	info ("Client " + client.getUniqueName() + " joined " + _name, CLR_GRN);
}

void	Channel::inviteToChannel(Client &host, Client &guest)
{
	// IF HOST IS NOT IN CHANNEL
	if (!isClientIsInList(_clients, host))
	{
		host.sendMessage(ERR_NOTONCHANNEL, _name + " :You're not on that channel");
		return ;
	}

	// IF GUEST ALREADY IN CHANNEL
	if (isClientIsInList(_clients, guest))
	{
		host.sendMessage(ERR_USERONCHANNEL, guest.getUniqueName() + " " +  _name + " :is already on channel");
		return ;
	}
	
	// IF INVITE ONLY (I FLAG)
	if (_inviteOnly)
	{
		if (!isClientIsInList(_operators, host))
		{
			host.sendMessage(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
			return ;
		}
	}
	
	// Add guest to the invitation list
	this->addInvitation(guest);

	// SEND INVITE
	// host :Aurora.AfterNET.Org 341 astein astein__ #test3
	host.sendMessage(RPL_INVITING, host.getUniqueName() + " " +
		guest.getUniqueName() + " " + _name);
	
	// guest :astein!alex@F456A.75198A.60D2B2.ADA236.IP INVITE astein__ #test3
	guest.sendMessage(":" + host.getUniqueName() + "!" + host.getUsername() +
		"@localhost" + " INVITE " + guest.getUniqueName() + " " + _name);

	info ("Invite sent to " + guest.getUniqueName() + " by " + host.getUniqueName(), CLR_GRN);
}

void	Channel::kickFromChannel(Client &kicker, Client &kicked)
{
	// IF CLIENT IS NOT IN CHANNEL
	if (!isClientIsInList(_clients, kicked))
	{
		kicked.sendMessage(ERR_USERNOTINCHANNEL, kicked.getUniqueName() + " " + _name + " :They aren't on that channel");
		return ;
	}
	
	// IF CLIENT IS NOT OPERATOR
	if (!isClientIsInList(_operators, kicked))
	{
		kicked.sendMessage(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
		return ;
	}

	// FIRST SEND THE MSG THEN KICK SO THAT THE
	// sendMessageToClients() will INLUDE THE KICKED GUY
	// MSG:
	// :astein!alex@F456A.75198A.60D2B2.ADA236.IP KICK #test3 astein__ :astein
	std::string msg = ":" + kicker.getUniqueName() + "!" + kicker.getUsername() + "@localhost" +
		" KICK " + _name + " " + kicked.getUniqueName() + " :" + kicker.getUniqueName();
	this->sendMessageToClients(msg);

    _clients.remove(&kicked);
    _operators.remove(&kicked); // TODO: test if this fails if the client isnt in the list
	kicked.removeChannel(this);	
	info ("Client " + kicked.getUniqueName() + " kicked from " + _name + " by " + kicker.getUniqueName(), CLR_RED);
}

void	Channel::partChannel(Client &client)
{
	// IF CLIENT IS NOT IN CHANNEL
	if (!isClientIsInList(_clients, client))
	{
		client.sendMessage(ERR_NOTONCHANNEL, _name + " :You're not on that channel");
		return ;
	}

	// FIRST SEND THE MSG THEN PART SO THAT THE
	// sendMessageToClients() will INLUDE THE LEAVING GUY
	// MSG:
	// :astein_!alex@F456A.75198A.60D2B2.ADA236.IP PART #test :Leaving
	std::string msg = ":" + client.getUniqueName() + "!" + client.getUsername() + "@localhost" +
		" PART " + _name + " :Leaving";
	this->sendMessageToClients(msg);
	_clients.remove(&client);
    _operators.remove(&client); // TODO: test if this fails if the client isnt in the list
	client.removeChannel(this);
	info ("Client " + client.getUniqueName() + " left " + _name, CLR_ORN);
}

// Modes & Topic funtionality
// -----------------------------------------------------------------------------
void	Channel::topicOfChannel(Client &sender, const std::string &topic)
{
	// IF SENDER IS NOT IN CHANNEL
	if (!isClientIsInList(_clients, sender))
	{
		sender.sendMessage(ERR_NOTONCHANNEL, _name + " :You're not on that channel");
		return ;
	}
	
	// IF NO TOPIC IS PROVIDED
	if (topic.empty())
	{
		sender.sendMessage(
			":localhost " + std::string(RPL_TOPIC) +
			" " + sender.getUniqueName() +
			" " + _name + " :" + _topic);
		sender.sendMessage(
			":localhost " + std::string(RPL_TOPICADDITIONAL) +
			" " + sender.getUniqueName() +
			" " + _topicChange);
		return ;
	}
	
	// IF TOPIC IS PROTECTED (FLAG T)
	if (_topicProtected)
	{
		if (!isClientIsInList(_operators, sender))
		{
			sender.sendMessage(ERR_CHANOPRIVSNEEDED, _name + " :You're not channel operator");
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
	_topicChange = sender.getUniqueName() + "!" + 
			sender.getUsername() + "@localhost" +
			" " + ss.str();
	// SEND TOPIC MESSAGE
	std::string confirmMsg =
		sender.getUniqueName() + "!" +
		sender.getUsername() + "@localhost" +
		" TOPIC " + _name + " :" + _topic;

	sendMessageToClients(confirmMsg);
	sender.sendMessage(confirmMsg);

	info ("Topic changed to: " + _topic + " by " + sender.getUniqueName(), CLR_GRN);
	info ("Topic change message: " + _topicChange, CLR_GRN);
}

void	Channel::modeOfChannel(/* TODO: */)
{
	// TODO:
}

// Simple List Management
// -----------------------------------------------------------------------------
void	Channel::addClient		(Client &client)
{
	_clients.push_back(&client);
}

void	Channel::removeClient	(Client &client)
{
	_clients.remove(&client);
}

void	Channel::addOperator	(Client &client)
{
	_operators.push_back(&client);
}

void	Channel::removeOperator	(Client &client)
{
	_clients.remove(&client);
}		

void	Channel::addInvitation	(Client &client)
{
	_invitations.push_back(&client);
}

void	Channel::removeInvitation(Client &client)
{
	_invitations.remove(&client);
}

// Channel Broadcast Message
// -----------------------------------------------------------------------------
void	Channel::sendMessageToClients(const std::string &ircMessage, Client *sender) const
{
    std::list<Client *>::const_iterator it = _clients.begin();

	//TODO: change this to a for loop
    while (it != _clients.end())
    {
		if (*it != sender)
        	(*it)->sendMessage(ircMessage);
        it++;
    }
}


// Getters and Setters
// -----------------------------------------------------------------------------
const std::string	&Channel::getUniqueName() const
{
    return _name;
}

// Private Methods
// -----------------------------------------------------------------------------
bool	Channel::isClientIsInList(std::list<Client *> list, const Client &client) const
{
	if (list.empty())
		return false;
	std::list<Client *>::const_iterator it;

	for (it = list.begin(); it != list.end(); ++it)
	{
		if (&client == *it)
			return true;
	}
	return false;
}
