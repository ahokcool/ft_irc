/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:40 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/07 17:21:05 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "utils.hpp"

// Constructors and Destructor
// -----------------------------------------------------------------------------
Client::Client(const int socketFd) : _socketFd(socketFd)
{
	info ("Created Client Inctance", CLR_BLU);
}

// Copy Constructor
Client::Client(const Client &other) : 
	_inputBuffer(other._inputBuffer),
	_socketFd(other._socketFd),
	_nickname(other._nickname),
	_username(other._username),
	_fullname(other._fullname),
	_hostname(other._hostname)
{
	info ("COPY CONSTRUCTOR!!!", CLR_BLU);
	for(std::list<Channel *>::const_iterator it = other._channels.begin(); it != other._channels.end(); ++it)
		_channels.push_back(*it);
}

// Destructor
Client::~Client()
{
	std::list<Channel *>::iterator it = _channels.begin();

	while (it != _channels.end())
	{
		(*it)->kickFromChannel(*this, *this); // TODO: this is fucked up
		it++;
	}
	_channels.clear();
	// Client::_nicknames.erase(_nickname);
	info("Client destroyed", CLR_RED);
}

// Equal Overload (for list remove)
// -----------------------------------------------------------------------------
bool	Client::operator==(const Client &other) const
{
	return _socketFd == other._socketFd;
}

// Simple List Management
// -----------------------------------------------------------------------------
void Client::addChannel(Channel *channel)
{
	if(!channel)
		return;
	_channels.push_back(channel);
	info("INSIDE CLIENT: Added Channel: " + channel->getUniqueName(), CLR_GRN);
	std::cout << "INSIDE CLIENT: Channel List Size: "<< _channels.size() << std::endl;
}

void Client::removeChannel(Channel *channel)
{
	if(!channel)
		return;
	_channels.remove(channel);
}

// Read message from client to buffer
// -----------------------------------------------------------------------------
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

// Get the full message from the buffer
// -----------------------------------------------------------------------------
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

// Send message to client
// -----------------------------------------------------------------------------
void Client::sendMessage(const std::string &ircMessage) const
{
	if (ircMessage.empty())
		return ;
	std::string msg = ircMessage;
	if(msg[msg.size() - 1] != '\n')
		msg += "\r\n";
	ssize_t bytesSent = send(_socketFd, msg.c_str(), msg.length(), 0);
	// LOGGER
	if (!msg.empty() && msg[msg.length() - 1] == '\n')
		msg.erase(msg.length() - 1);
	Logger::log("Message sent:\tMSG -->\t\t" 		+ msg);
	if (bytesSent == -1)
		Logger::log("\t ERROR -->\n\t" + std::string(strerror(errno)));
}

void	Client::sendMessage(const std::string &code, const std::string &message) const
{
	std::string ircMessage = 
		":localhost " + code + " " + _nickname + " " + message + "\n";
	sendMessage(ircMessage);
}

void Client::sendWhoIsMsg(Client &reciever) const
{
	// localhost 311 <nick> <user> <host> * :<real name>
	reciever.sendMessage(
		":localhost " + std::string(RPL_WHOISUSER) + " " +
		_nickname + " " +
		_username + " " +
		_hostname + " * :" +
		_fullname);

	// localhost 319 <nick> :{[@|+]<channel><space>}
	std::string channels;
	
	std::cout << "Channels List size: " << _channels.size() << std::endl;

	
	Logger::log("Channels of: " + _nickname + " :" + channels);
	for (std::list<Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
		channels += "@" + (*it)->getUniqueName() + " ";
	if(!channels.empty())
		reciever.sendMessage(
			":localhost " + std::string(RPL_WHOISCHANNELS) + " " +
			_nickname + " :" +
			channels);

	// localhost 318 <nick> :End of /WHOIS list.
	reciever.sendMessage(
		":localhost " + std::string(RPL_ENDOFWHOIS) + " " +
		_nickname + " :End of /WHOIS list.");
}

// Setters
// -----------------------------------------------------------------------------
void    Client::setUniqueName(const std::string &nickname)
{
	info("set nickname " + nickname, CLR_GRN);
	_nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
	info("set username " + username, CLR_GRN);
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

// Getters
// -----------------------------------------------------------------------------
int Client::getSocketFd() const
{
	return _socketFd;
}

const std::string &Client::getUniqueName() const
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

// Exception
// -----------------------------------------------------------------------------
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