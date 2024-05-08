/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:40 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/08 18:42:32 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "utils.hpp"

// Constructors and Destructor
// -----------------------------------------------------------------------------
Client::Client(const int socketFd) : _socketFd(socketFd), _hostname("localhost")
{
	Logger::log("CREATED Client Instance with fd: " + to_string(socketFd));
	logClient();
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
	Logger::log("COPIED Client Instance with fd: " + to_string(_socketFd));
	logClient();
	for(std::list<Channel *>::const_iterator it = other._channels.begin(); it != other._channels.end(); ++it)
		_channels.push_back(*it);
}

// Destructor
Client::~Client()
{
	std::list<Channel *>::iterator it = _channels.begin();

	while (it != _channels.end())
	{
		(*it)->kickFromChannel(this, this); // TODO: this is fucked up
		it++;
	}
	_channels.clear();
	Logger::log("DESTRUCTED Client Instance with fd: " + to_string(_socketFd));
	logClient();
}

// Equal Overload (for list remove)
// -----------------------------------------------------------------------------
bool	Client::operator==(const Client &other) const
{
	return _socketFd == other._socketFd;
}

bool	Client::operator!=(const Client &other) const
{
	return _socketFd != other._socketFd;
}

// Simple List Management
// -----------------------------------------------------------------------------
void Client::addChannel(Channel *channel)
{
	if(!channel)
	{
		Logger::log("ERROR Trying to  add a NULL Channel to the client list!");
		return;
	}
	_channels.push_back(channel);
	Logger::log("Client " + _nickname + " joined channel: " + channel->getUniqueName());
	logClient();
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

void Client::sendWhoIsMsg(Client *reciever) const
{
	if (!reciever)
		return;

	// localhost 311 <nick> <nick> <user> <host> * :<real name>
	reciever->sendMessage(
		":localhost " + std::string(RPL_WHOISUSER) + " " +
		_nickname + " " +
		_nickname + " " +
		_username + " " +
		_hostname + " * :" +
		_fullname);

	// localhost 319 <nick> :{[@|+]<channel><space>}
	std::string channels = getChannelList();	
	if(!channels.empty())
		reciever->sendMessage(
			":localhost " + std::string(RPL_WHOISCHANNELS) + " " +
			_nickname + " " +_nickname + " :" +
			channels);

	// localhost 318 <nick> :End of /WHOIS list.
	reciever->sendMessage(
		":localhost " + std::string(RPL_ENDOFWHOIS) + " " +
		_nickname + " " +_nickname + " :End of /WHOIS list.");	

	logClient();
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

const std::string Client::getChannelList() const
{
	std::string channels = "";

	if(_channels.empty())
		return channels;

	for (std::list<Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		channels += "@"; 
		channels += (*it)->getUniqueName();
		channels += " ";
		Logger::log("appeding channel list for client " + _nickname + ": " + channels);
	}
	Logger::log("Created channel list for client " + _nickname + ": " + channels);
	return channels;
}

// LOG
// -----------------------------------------------------------------------------
void Client::logClient() const
{
	std::ostringstream header, values;

	// Constructing headers
	header << std::setw(15) << std::left << "INPUT BUFFER"
			<< "| " << std::setw(15) << "SOCKET FD"
			<< "| " << std::setw(15) << "NICKNAME"
			<< "| " << std::setw(15) << "USERNAME"
			<< "| " << std::setw(15) << "FULLNAME"
			<< "| " << std::setw(15) << "HOSTNAME"
			<< "| " << std::setw(15) << "CHANNELS";

	// Constructing values under headers
	values << std::setw(15) << std::left << (_inputBuffer.length() > 14 ? _inputBuffer.substr(0, 14) + "." : _inputBuffer.empty() ? "(NULL)" : _inputBuffer)
			<< "| " << std::setw(15) << _socketFd
			<< "| " << std::setw(15) << (_nickname.length() > 14 ? _nickname.substr(0, 14) + "." : _nickname.empty() ? "(NULL)" : _nickname)
			<< "| " << std::setw(15) << (_username.length() > 14 ? _username.substr(0, 14) + "." : _username.empty() ? "(NULL)" : _username)
			<< "| " << std::setw(15) << (_fullname.length() > 14 ? _fullname.substr(0, 14) + "." : _fullname.empty() ? "(NULL)" : _fullname)
			<< "| " << std::setw(15) << (_hostname.length() > 14 ? _hostname.substr(0, 14) + "." : _hostname.empty() ? "(NULL)" : _hostname)
			<< "| " << getChannelList() << std::endl;

	// Combining headers and values into one log entry
	std::ostringstream logEntry;
	logEntry << header.str() << "\n" << values.str();

	// Logging the constructed message
	Logger::log("================ START CLIENT ================");
	Logger::log(header.str());
	Logger::log(values.str());
	Logger::log("================ END CLIENT ================");
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