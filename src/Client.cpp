/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:40 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/02 23:15:00 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "utils.hpp"

std::set<std::string> Client::_nicknames;

Client::Client(const int socketFd, const std::string &nickname) throw(NickNameException) :
	_socketFd(socketFd), _nickname(nickname)
{
	Client::reserveNickName(nickname);	//If this fails we throw an exception
	_nickname = nickname;
    _username = _nickname + " is too lazy to set a username";
    _hostname = "default";
	info ("Created Client: ", CLR_BLU);
	info ("\t->Nickname:\t" 	+ _nickname, CLR_BLU);
	info ("\t->Username:\t" 	+ _username, CLR_BLU);
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
	Client::_nicknames.erase(_nickname);
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

void Client::getInvited(Channel *channel)
{
    _invitations.push_back(channel);
}

void Client::getKicked(Channel *channel)
{
    for (std::list<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        //TODO: find a channel if user's channels and delete it from list
    }
    (void)channel;  
    //TODO: send the message to the client that ha was kicked. also send it to the channel
}

void Client::setUsername(const std::string &username)
{
    _username = username;
}

void Client::setHostname(const std::string &hostname)
{
    _hostname = hostname;
}

int Client::getSocketFd() const
{
	return _socketFd;
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

// Static Function


void	Client::reserveNickName(const std::string &nickname) throw(NickNameException)
{
	info ("Reserving Nickname: ", CLR_BLU);
	info ("\t->Nickname:\t" 	+ nickname, CLR_BLU);
	if (Client::_nicknames.find(nickname) != Client::_nicknames.end())
	{
		throw NickNameException("Nickname in use!"); // '" + nickname + "' already in use");
	}
	Client::_nicknames.insert(nickname);
}

// EXCEPTION
NickNameException::NickNameException(const std::string &message) : _message(message)
{
	// Nothing to do here
}

NickNameException::~NickNameException() throw()
{
	// Nothing to do here
}

const char *NickNameException::what() const throw()
{
	return _message.c_str();
}