/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:46 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/03 02:45:19 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

// Constructor
Channel::Channel(const std::string &name, Client *client) : _name(name)
{
    client->joinChannel(this);
    this->addClient(client);
    this->addOperator(client);
    std::cout << "NEW CHANNEL --> " << name << "\n";
}

// Destructor
Channel::~Channel()
{
    std::list<Client *>::iterator it = _clients.begin();

    while (it != _clients.end())
    {
        (*it)->leaveChannel(this);
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
        // if ((*it)->getNickname() != ircMessage.substr(1, ircMessage.find(' ', 1) - 1))
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

void Channel::removeClient(Client *client)
{
    // TODO: test check if one op can kick another op
    _clients.remove(client);
    _operators.remove(client); // TODO: test if this fails if the client isnt in the list
}

const std::string &Channel::getName() const
{
    return _name;
}

bool					Channel::isActive() const
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
