/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 16:44:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/03 02:37:58 by anshovah         ###   ########.fr       */
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
	std::istringstream iss(ircMessage);
    std::string token;

	if (!sender)
		throw MessageException("No sender");

    if (ircMessage.empty())
        throw MessageException("Empty message");


		
    while (iss >> token)
	{
		if (_cmd.empty())
			_cmd = token;
		else if (token[0] == '#')
			_channelName = token;
		else if (_arg1.empty() && token[0] != ':')
			_arg1 = token;
		else if (_arg2.empty() && token[0] != ':')
			_arg2 = token;
		else if (token[0] == ':')
		{	
			_arg1 = ircMessage.substr(ircMessage.find(':') + 1);
			break;
		}
	}
    if (_cmd.empty())
        throw MessageException("Invalid message format");
		







std::cout << "CMD --> " << _cmd << " CHANNEL --> " << _channelName << " ARG1 --> " << _arg1 << " ARG2 --> " << _arg2 << "\n";
}

Message::~Message()
{
    // Nothing to do
}

void Message::setReceiver(Client *receiver)
{
	_receiver = receiver;
}

const std::string &Message::getCmd() const
{
    return _cmd;
}

const std::string &Message::getChannelName() const
{
    return _channelName;
}

const std::string &Message::getArg1() const
{
    return _arg1;
}

const std::string &Message::getArg2() const
{
    return _arg2;
}

Client *Message::getSender() const
{
	return _sender;
}

Client *Message::getReceiver() const
{
	return _receiver;
}

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