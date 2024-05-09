/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/09 15:32:26 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <map>
#include "Client.hpp"
#include "utils.hpp"
#include "Message.hpp"	//SHOULD BE REMOVED

class Client;
class Message;

class Channel
{
    public:
		// Constructor and Destructor
        Channel(const std::string &name);
		void 	iniChannel(Client *client);
		Channel(const Channel &other); // Copy Constructor
        ~Channel();

		// Equal Overload (for list remove)
		bool	operator==(const Channel &other) const;

		// So the Server can check if the Channel still has an operator
		bool	isActive() const;

		// Members & Operators funtionality
		void	joinChannel 	(Client *client, const std::string &pswd);
		void	inviteToChannel	(Client *host, Client *guest);
		void	kickFromChannel	(Client *kicker, Client *kicked);		
		void	partChannel		(Client *client, const std::string &reason);

		// Modes & Topic funtionality
		void	topicOfChannel(Client *sender, const std::string &topic);
		void	modeOfChannel(Client *client, const std::string &flag, const std::string &value);
		
		// Simple Map Management
		void	addClient		(Client *client, int status);	// IF CLIENT ALREADY EXISTS, IT WILL UPDATE THE STATUS
		void	removeClient	(Client *client);

		// Channel Broadcast Message
        void	sendMessageToClients(const std::string &ircMessage, Client *sender = NULL) const;

		// Getters and Setters
		const std::string	&getUniqueName() const;
		const std::string	getClientList()	const;

		// LOG
		void 				logChanel() const;

    private:
		// For the basic channel functionality
		int					getClientState(const Client *client) const;
		std::string			getChannelFlags();

        Channel();									// Default Constructor shouldn't be used
        const std::string		_channelName;
        std::string				_topic;
        std::string				_topicChange;
        std::string				_key;				// empty string means no password
        size_t					_limit; 			// 0 means unset
        bool					_inviteOnly;
        bool					_topicProtected;
		
		// A CLIENT CAN ONLY BE AT ONE LIST AT A TIME!
		#define STATE_I	0	// INVITED
		#define STATE_C	1	// CLIENT
		#define STATE_O	2	// OPERATOR
		std::map<Client *, int>	_clients;
};

#endif
