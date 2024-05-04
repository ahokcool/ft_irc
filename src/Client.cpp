/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:40 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/04 01:21:15 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "utils.hpp"

std::set<std::string> Client::_nicknames;

Client::Client(const int socketFd) : _socketFd(socketFd)
{
	info ("Created Client Inctance", CLR_BLU);
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

bool	Client::appendBuffer(const char *buffer)
{
	_inputBuffer += std::string(buffer);
	if (_inputBuffer.size() > BUFFER_SIZE && _inputBuffer.find("\r\n") == std::string::npos)
	{
		_inputBuffer.clear();
		return false;
	}
	return true;
}

std::string	Client::getFullMessage()
{
	size_t pos = _inputBuffer.find("\r\n");
	if(pos != std::string::npos)
	{
		std::string _fullMsg;
		_fullMsg = _inputBuffer.substr(0, pos);
		_inputBuffer = _inputBuffer.substr(pos + 2);
		return _fullMsg;
	}
	return std::string("");
}

bool	Client::operator==(const Client &other) const
{
	return _socketFd == other._socketFd;
}





 #include <string.h>
 #include <errno.h>

// the last argument specifies the flags that control the behavior of the send operation. 
// When set to 0, it means that no special flags are being used, and the send operation should proceed with default behavior.
void Client::sendMessage(const std::string &ircMessage) const
{
    std::string msgN = ircMessage + "\r\n";
    ssize_t bytesSent = send(_socketFd, msgN.c_str(), msgN.length(), 0);
if (bytesSent == -1) {
    std::cerr << "send failed: " << strerror(errno) << "\n";
} else {
    std::cout << "Bytes sent: " << bytesSent << "\n"; }
}

void Client::joinChannel(Channel *channel)
{
    _channels.push_back(channel);
    //  Now talking on #newwww
    //  astein (alex@F456A.75198A.60D2B2.ADA236.IP) has joined
    // info() TODO:
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

void    Client::setNickname(const std::string &nickname) throw(NickNameException)
{
    info ("Reserving Nickname: ", CLR_BLU);
	info (nickname, CLR_BLU);
	if (Client::_nicknames.find(nickname) != Client::_nicknames.end())
	{
		throw NickNameException("Nickname in used!"); // '" + nickname + "' already in use");
        //TODO: send a message to inform client that he need another nickname
	}
	Client::_nicknames.insert(nickname);
    _nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
    info("Set username " + username, CLR_GRN);
    _username = username;
}

void Client::setFullname(const std::string &fullname)
{
    _fullname = fullname;
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

const std::string &Client::getFullname() const
{
    return _fullname;
}

const std::string &Client::getHostname() const
{
    return _hostname;
}

// Static Function


// void	Client::reserveNickName(const std::string &nickname) throw(NickNameException)
// {
// 	info ("Reserving Nickname: ", CLR_BLU);
// 	info ("\t->Nickname:\t" 	+ nickname, CLR_BLU);
// 	if (Client::_nicknames.find(nickname) != Client::_nicknames.end())
// 	{
// 		throw NickNameException("Nickname in use!"); // '" + nickname + "' already in use");
// 	}
// 	Client::_nicknames.insert(nickname);
// }

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