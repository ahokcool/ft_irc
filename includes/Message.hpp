/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 16:37:13 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/07 17:25:38 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include "Client.hpp"
#include "Channel.hpp"
#include "Logger.hpp"

class Client;
class Channel;

class Message
{
    public:
		// Constructors and Destructor
        Message(Client &sender, const std::string &ircMessage);
        void parseMessage(const std::string &ircMessage);
        ~Message();

		// Getters
        Client 				*getSender()			const;
        Client 				*getReceiver()			const;
        Channel 			*getChannel()			const;
        const std::string 	&getCmd()				const;
        const std::string 	&getChannelName()		const;
        const std::string 	&getColon()				const;
        const std::string 	&getArg(size_t index)	const;

		// Setters
        void				setReceiver(Client *receiver);
        void				setChannel(Channel *channel);

    private:
        Message();

        Client		    	*_sender;
        Client          	*_receiver;
        Channel         	*_channel;
        std::string     	_cmd;
        std::string     	_channelName;
        std::string     	_colon;
        std::string     	_args[3];
};

#endif