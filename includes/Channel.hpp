/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/05 02:02:43 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <list>
#include "Client.hpp"
#include "utils.hpp"
#include "Message.hpp"

class Client;
class Message;

class Channel
{
    public:
        Channel(const std::string &name, Client &client);
        ~Channel();

		// Equal Overload for list remove
		bool					operator==(const Channel& other) const;
    
		// Client Management
        void                    addClient		(Message &msg);
        void                    addOperator		(Message &msg);
        void                    removeClient	(Client &client);
        void                    removeOperator	(Client &client);


		
		
        void                    sendMessageToClients(const std::string &ircMessage) const;

		// Setters
		void					setTopic(const std::string &param);
		void					setKey(const std::string &param);
		void					setUserLimit(const size_t param);
		void					setInviteOnly(const bool param);
		void					setTopicProtected(const bool param);

		const std::string		&getTopic() const;
		const std::string		&getKey() const;
		size_t 					getUserLimit() const;
		bool 					getInviteOnly() const;
		bool 					getTopicProtected() const;
		
        const std::string       &getUniqueName() const;
    
	//TODO: if all the ops left the channel, kick all the clients and delete the channel
		bool					isActive() const;
    private:
		bool					isClientInChannel(const Client &client) const;

	
        Channel();
        const std::string       _name;
        std::string             _topic;
        std::string             _key; // empty string means no password
        size_t                  _limit; // 0 means unset
        bool                    _inviteOnly;
        bool                    _topicProtected;
        std::list<Client *>     _clients;
        std::list<Client *>     _operators;
};

#endif