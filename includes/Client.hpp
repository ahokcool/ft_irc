/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:33 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/10 00:55:22 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include "Channel.hpp"
#include "codes.hpp"

class Channel;

class NickNameException : public std::exception
{
	public:
		NickNameException(const std::string &message);
		~NickNameException() 		throw();
		const char	*what() const	throw();

	private:
		std::string _message;
};

class Client
{   
    public:
		// Constructors and Destructor
        Client(const int socketFd);
		Client(const Client &other);
        ~Client();

		// Equal Overload (for list remove)
		bool					operator==(const Client &other) const;
		bool					operator!=(const Client &other) const;

		// Simple List Management
		void                    addChannel(Channel *channel);
        void                    removeChannel(Channel *channel);		

		// Read message from client to buffer
		bool					appendBuffer(const char *buffer);

		// Get the full message from the buffer
		std::string				getFullMessage();

		// Send message to client
        void                    sendMessage(const std::string &ircMessage) const;
        void                    sendMessage(const std::string &code, const std::string &message) const;
        void 					sendWhoIsMsg(Client *reciever) const;
		
		// Setters
        void					setUniqueName(const std::string &nickname);
        void					setUsername(const std::string &username);
        void					setFullname(const std::string &fullname);
        void                    setHostname(const std::string &hostname);

		// Getters
		int						getSocketFd()		const;
        const std::string		&getUniqueName()	const;
        const std::string		&getUsername()		const;
        const std::string		&getFullname()		const;
        const std::string		&getHostname()		const;
		const std::string		getChannelList()	const;

		// LOG
		void					logClient() const;

    private:
        Client();
		std::string				_inputBuffer;
        int						_socketFd;
        std::string			   	_nickname;
        std::string         	_username;	// Can only be changed when connecting to server!
        std::string				_fullname;	// Can only be changed when connecting to server!
        std::string         	_hostname;	// Can only be changed when connecting to server!
        std::list<Channel *>	_channels;
};

#endif