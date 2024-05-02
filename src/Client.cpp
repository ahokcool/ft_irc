/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:40 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/02 00:33:55 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(const int socketFd, const std::string &nickname) : _socketFd(socketFd), _nickname(nickname)
{
    _username = _nickname + " is too lazy to set a username";
    _hostname = "default";
}

Client::~Client()
{
    std::list<Channel *>::iterator it = _channels.begin();

    while (it != _channels.end())
    {
        (*it)->removeClient(this);
        it++;
    }
    _channels.clear();
}

// the last argument specifies the flags that control the behavior of the send operation. 
// When set to 0, it means that no special flags are being used, and the send operation should proceed with default behavior.
void Client::sendMessage(const std::string &ircMessage) const
{
    send(_socketFd, ircMessage.c_str(), ircMessage.length(), 0);
}

void Client::joinChannel(Channel *channel)
{
    _channels.push_back(channel);
}

void Client::leaveChannel(Channel *channel)
{
    _channels.remove(channel);
}

void Client::setUsername(const std::string &username)
{
    _username = username;
}

void Client::setHostname(const std::string &hostname)
{
    _hostname = hostname;
}

const std::string &Client::getNickname() const
{
    return _nickname;
}

const std::string &Client::getUsername() const
{
    return _username;
}

const std::string &Client::getHostname() const
{
    return _hostname;
}

