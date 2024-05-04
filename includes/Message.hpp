/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 16:37:13 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/04 03:40:26 by anshovah         ###   ########.fr       */
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

class Message
{
    public:
        Message(Client &sender, const std::string &ircMessage);
        void parseMessage(const std::string &ircMessage);
        ~Message();

        void setReceiver(Client *receiver);
        void setChannel(Channel *channel);
        Client &getSender() const;
        Client *getReceiver() const;
        Channel *getChannel() const;
        const std::string &getCmd() const;
        const std::string &getChannelName() const;
        const std::string &getColon() const;
        const std::string &getArg(size_t index) const;

    private:
        Message();

        Client          &_sender;
        Client          *_receiver; //TODO: do we  need this?
        Channel         *_channel;
        std::string     _cmd;
        std::string     _channelName;
        std::string     _colon;
        std::string     _args[3];
};

#endif