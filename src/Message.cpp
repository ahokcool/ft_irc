/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 16:44:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/13 22:02:00 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "utils.hpp"

// Constructor
Message::Message(Client *sender, const std::string &ircMessage) :
	_sender(sender),
	_receiver(NULL),
	_channel(NULL),
	_cmd(""),
	_channelName(""),
	_colon("")
{
	_args[0] = "";
	_args[1] = "";
	_args[2] = "";
	// by architechture IRC message can not be empty
	parseMessage(ircMessage);
	// Log Message
	logMessage();
}

// Parse Message
/*
	NICK	nickname
	USER	username * * :full name
	INVITE jojojo #TEST1
	...
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
			for (int i = _colon.length() - 1; i >= 0; i--)
			{
        		// Check if the character is non-printable or whitespace
        		if (_colon[i] < 32 || std::isspace(static_cast<unsigned char>(_colon[i])))
            		_colon.erase(i, 1); // Remove the character
				else
					break;
			}
			break;
		}
	}
}

// Destructor
Message::~Message()
{
    // Nothing to do
}

// Getters
// -----------------------------------------------------------------------------
Client *Message::getSender() const
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


// Setters
// -----------------------------------------------------------------------------
void Message::setReceiver(Client *receiver)
{
	_receiver = receiver;
}

void Message::setChannel(Channel *channel)
{
	_channel = channel;
}

// LOG
// -----------------------------------------------------------------------------
void Message::logMessage() const
{
	std::ostringstream header, values;

	// Constructing headers
    header << "| " << std::setw(15) << std::left << "CMD"
           << "| " << std::setw(15) << "CHANNEL"
           << "| " << std::setw(15) << "ARG0"
           << "| " << std::setw(15) << "ARG1"
           << "| " << std::setw(15) << "ARG2"
           << "| " << std::setw(15) << "COLON";

    // Constructing values under headers
    values << "| " << std::setw(15) << std::left << (_cmd.length() > 14 ? _cmd.substr(0, 14) + "." : _cmd.empty() ? "(NULL)" : _cmd)
           << "| " << std::setw(15) << (_channelName.length() > 14 ? _channelName.substr(0, 14) + "." : _channelName.empty() ? "(NULL)" : _channelName)
           << "| " << std::setw(15) << (_args[0].length() > 14 ? _args[0].substr(0, 14) + "." : _args[0].empty() ? "(NULL)" : _args[0])
           << "| " << std::setw(15) << (_args[1].length() > 14 ? _args[1].substr(0, 14) + "." : _args[1].empty() ? "(NULL)" : _args[1])
           << "| " << std::setw(15) << (_args[2].length() > 14 ? _args[2].substr(0, 14) + "." : _args[2].empty() ? "(NULL)" : _args[2])
           << "| " << std::setw(15) << (_colon.length() > 14 ? _colon.substr(0, 14) + "." : _colon.empty() ? "(NULL)" : _colon);

    // Combining headers and values into one log entry
    std::ostringstream logEntry;
    logEntry << header.str() << "\n" << values.str();

    // Logging the constructed message
	Logger::log("\n=> START MSG =======================================================================================");
    Logger::log(header.str());
    Logger::log(values.str());
	Logger::log("=> END MSG =========================================================================================\n");
}