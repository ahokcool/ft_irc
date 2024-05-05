/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:46 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/05 01:11:21 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

// Constructor
Channel::Channel(const std::string &name, Client *client) : _name(name), _clients()
{
    client->addChannel(this);
    this->addClient(client);
    this->addOperator(client);
    info("NEW CHANNEL " + name + " CREATED", CLR_GRN);
}

// Destructor
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

// Equal Overload for list remove
bool Channel::operator==(const Channel &other) const
{
	return this->_name == other._name;
}

void Channel::sendMessage(const std::string &ircMessage) const
{
    std::list<Client *>::const_iterator it = _clients.begin();

    while (it != _clients.end())
    {
        // if ((*it)->getUniqueName() != ircMessage.substr(1, ircMessage.find(' ', 1) - 1))
        //TODO: not send a message to itself
        (*it)->sendMessage(ircMessage);
        it++;
    }
}

void Channel::addClient(Client *client)
{
    _clients.push_back(client);
}

void Channel::addOperator(Client *client)
{
	_operators.push_back(client);
}

// SETTERS
void	Channel::setTopic(const std::string &param)
{
	_topic = param;
}

void	Channel::setKey(const std::string &param)
{
	_key = param;
}

void	Channel::setUserLimit(const size_t param)
{
	_limit = param;
}

void	Channel::setInviteOnly(const bool param)
{
	_inviteOnly = param;
}

void	Channel::setTopicProtected(const bool param)
{
	_topicProtected = param;
}

// GETTERS
const std::string		&Channel::getTopic() const
{
	return _topic;
}

const std::string		&Channel::getKey() const
{
	return _key;
}

size_t 			Channel::getUserLimit() const
{
	return _limit;
}

bool 				Channel::getInviteOnly() const
{
	return _inviteOnly;
}

bool 				Channel::getTopicProtected() const
{
	return _topicProtected;
}


void	Channel::removeClient(Client *client)
{
    // TODO: test check if one op can kick another op
    _clients.remove(client);
    _operators.remove(client); // TODO: test if this fails if the client isnt in the list
	// TODO: send a message to the channel?
}

const std::string &Channel::getUniqueName() const
{
    return _name;
}

bool	Channel::isClientInChannel(const Client &client) const
{
	std::list<Client *>::const_iterator it;

	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (&client == *it)
			return true;
	}
	return false;
}

bool	Channel::isActive() const
{
	if(_operators.size() == 0)
		return false;
	if(_clients.size() == 0)
		return false;
	return true;
}

// Message

// B - broadcast
// P - privat
// C - channel

// Server                            			
// B somebody joins
// B general info/broadcast(shut down, fuck you)
// P login

// Channel
// C somebody joins
// C general info(shut down, fuck you)

// Client
// P messages from other clients
