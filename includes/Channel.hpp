/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/13 21:45:00 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <map>
#include "Client.hpp"
#include "Server.hpp"
#include "utils.hpp"

class Client;
class Server;

class Channel
{
    public:
		// Constructor and Destructor
        Channel(const std::string &name, const std::string &topic = "");
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
		void	kickFromChannel	(Client *kicker, Client *kicked, const std::string &reason);		
		void	partChannel		(Client *client, const std::string &reason);

		// Modes & Topic funtionality
		void	topicOfChannel(Client *sender, const std::string &topic);
		void	modeOfChannel(Client *client, const std::string &flag, const std::string &value, Server *server);
		
		// Simple Map Management
		void	removeClient	(Client *client);

		// Channel Broadcast Message
        void	sendMessageToClients(const std::string &ircMessage, Client *sender = NULL) const;
		void 	sendWhoMessage(Client *receiver) const;

		// Getters and Setters
		const std::string	&getUniqueName() const;
		const std::string	getClientList()	const;

		// LOG
		void 				logChanel() const;

    private:
		// Simple Map Management
		void	addClient		(Client *client, int status);	// IF CLIENT ALREADY EXISTS, IT WILL UPDATE THE STATUS
	
		// MSG Functions
		void 	sendTopicMessage(Client *receiver) const;
		void 	sendNamesMessage(Client *receiver) const;

		// For the basic channel functionality
		int					getClientState(const Client *client) const;
		std::string			getChannelFlags();

        Channel();									// Default Constructor shouldn't be used
        const std::string		_channelName;
        std::string				_topic;
        std::string				_topicChange;
        std::string				_key;				// empty string means no password
        int						_limit; 			// 0 means unset
        bool					_inviteOnly;
        bool					_topicProtected;
		
		// A CLIENT CAN ONLY BE AT ONE LIST AT A TIME!
		#define STATE_I	0	// INVITED
		#define STATE_C	1	// CLIENT
		#define STATE_O	2	// OPERATOR
		std::map<Client *, int>	_clients;
};

#endif
