/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 16:44:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/04 05:27:03 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "utils.hpp"

Message::Message(Client &sender, const std::string &ircMessage) : _sender(sender), _receiver(NULL), _channel(NULL)
{
	// by architechture IRC message can not be empty
	parseMessage(ircMessage);
	Logger::log("Message parsed:\n\tCMD -->\t\t" 		+ _cmd +
								"\n\tCHANNEL -->\t" 	+ _channelName +
								"\n\tARG0 -->\t\t" 		+ _args[0] + 
								"\n\tARG1 -->\t\t" 		+ _args[1] + 
								"\n\tARG2 -->\t\t" 		+ _args[2] + 
								"\n\tCOLON -->\t\t" 	+ _colon);
}

Message::~Message()
{
    // Nothing to do
}

void Message::setReceiver(Client *receiver)
{
	_receiver = receiver;
}

void Message::setChannel(Channel *channel)
{
	_channel = channel;
}

Client &Message::getSender() const
{
	return _sender;
}

Client *Message::getReceiver() const
{
	return _receiver;
}

Channel *Message::getChannel() const
{
	return _channel;
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
void Message::parseMessage(const std::string &ircMessage)
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
}
