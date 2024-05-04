/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 16:44:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/04 01:34:06 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "utils.hpp"

Message::Message()
{
    // Nothing to do
}

Message::Message(Client *sender, const std::string &ircMessage) throw(MessageException) : 
	_sender(sender), _receiver(NULL)
{
	if (!sender)
		throw MessageException("No sender");
    if (ircMessage.empty())
		throw MessageException("Empty message");

	parseMessage(ircMessage);
	Logger::log("Message parsed:\n\tCMD -->\t" + _cmd +
								"\n\tCHANNEL -->\t" + _channelName +
								"\n\tARG1 -->\t" + _args[0] + 
								"\n\tARG2 -->\t" + _args[1] + 
								"\n\tARG3 -->\t" + _args[2] + 
								"\n\tCOLON -->\t" + _colon);
}

Message::~Message()
{
    // Nothing to do
}

void Message::setReceiver(Client *receiver)
{
	_receiver = receiver;
}

Client *Message::getSender() const
{
	return _sender;
}

Client *Message::getReceiver() const
{
	return _receiver;
}

const std::string &Message::getCmd() const
{
    return _cmd;
}

const std::string &Message::getChannelName() const
{
    return _channelName;
}

const std::string &Message::getColon() const
{
	return _colon;
}

const std::string &Message::getArg(size_t index) const
{
	if (index > 2)
		return _args[2];
    return _args[index];
}


/*
	NICK	nickname
	USER	username * * :full name
	INVITE jojojo #TEST1
	TODO: copilot the other cases
*/
void Message::parseMessage(const std::string &ircMessage) throw(MessageException)
{
	std::istringstream iss(ircMessage);
    std::string token;

    while (iss >> token)
	{
		if (_cmd.empty())
			_cmd = token;
		else if (token[0] == '#')
			_channelName = token;
		else if (_args[0].empty() && token[0] != ':')
			_args[0] = token;
		else if (_args[1].empty() && token[0] != ':')
			_args[1] = token;
		else if (_args[2].empty() && token[0] != ':')
			_args[2] = token;
		else if (token[0] == ':')
		{	
			_colon = ircMessage.substr(ircMessage.find(':') + 1);
			break;
		}
	}
    if (_cmd.empty())
        throw MessageException("Empty IRC command!");
}

// EXCEPTION CLASS
// -----------------------------------------------------------------------------
MessageException::MessageException(const std::string &message) : _message(message)
{
    // Nothing to do
}

MessageException::~MessageException() throw()
{
    // Nothing to do
}

const char	*MessageException::what() const throw()
{
    return _message.c_str();
}