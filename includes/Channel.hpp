/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/05 00:44:11 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <list>
#include "Client.hpp"
#include "utils.hpp"

class Client;

class Channel
{
    public:
        Channel(const std::string &name, Client *client);
        ~Channel();

		// Equal Overload for list remove
		bool					operator==(const Channel& other) const;
    

        void                    sendMessage(const std::string &ircMessage) const;
        void                    addClient(Client *client);
        void                    addOperator(Client *client);

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
		
        void                    removeClient(Client *client); // delete empty channel
        const std::string       &getUniqueName() const;
		bool					isClientInChannel(const Client &client) const;
    
	//TODO: if all the ops left the channel, kick all the clients and delete the channel
		bool					isActive() const;
    private:
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