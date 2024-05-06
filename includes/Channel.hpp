/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 23:23:47 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/07 00:10:50 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <list>
#include "Client.hpp"
#include "utils.hpp"
#include "Message.hpp"	//SHOULD BE REMOVED

class Client;
class Message;

class Channel
{
    public:
		// Constructor and Destructor
        Channel(const std::string &name, Client &client);
        ~Channel();

		// Equal Overload (for list remove)
		bool	operator==(const Channel& other) const;

		// So the Server can check if the Channel still has an operator
		bool	isActive() const;

		// Members & Operators funtionality
		void	joinChannel 	(Client &client, const std::string &pswd);
		void	inviteToChannel	(Client &host, Client &guest);
		void	kickFromChannel	(Client &kicker, Client &kicked);		
		void	partChannel		(Client &client);

		// Modes & Topic funtionality
		void	topicOfChannel(Client &sender, const std::string &topic);
		void	modeOfChannel(/* TODO: */);
		
		// Simple List Management
		void	addClient		(Client &client);
		void	removeClient	(Client &client);
		void	addOperator		(Client &client);
        void	removeOperator	(Client &client);
		void	addInvitation	(Client &client);
		void	removeInvitation(Client &client);

		// Channel Broadcast Message
        void	sendMessageToClients(const std::string &ircMessage, Client *sender = NULL) const;

		// Getters and Setters
		const std::string	&getUniqueName() const;

    private:
		// For the basic channel functionality
		bool					isClientIsInList(std::list<Client *> list, const Client &client) const;

        Channel();									// Default Constructor shouldn't be used
        const std::string		_name;
        std::string				_topic;
        std::string				_topicChange;
        std::string				_key;				// empty string means no password
        size_t					_limit; 			// 0 means unset
        bool					_inviteOnly;
        bool					_topicProtected;
        std::list<Client *>		_clients;
        std::list<Client *>		_operators;
        std::list<Client *>		_invitations;

};

#endif
