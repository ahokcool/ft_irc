/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:33 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/04 01:41:06 by anshovah         ###   ########.fr       */
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
        Client(const int socketFd);
        ~Client();

		bool					appendBuffer(const char *buffer);
		std::string				getFullMessage();

		bool					operator==(const Client &other) const;

        void                    sendMessage(const std::string &ircMessage) const;
        void                    sendMessage(const std::string &code, const std::string &message) const;

        void                    joinChannel(Channel *channel);
        void                    leaveChannel(Channel *channel);
        void                    getInvited(Channel *channel);
        void                    getKicked(Channel *channel);
        
        void					setNickname(const std::string &nickname) throw(NickNameException);
        void					setUsername(const std::string &username);
        void					setFullname(const std::string &fullname);
        void                    setHostname(const std::string &hostname);
		int						getSocketFd() const;
        const std::string		&getNickname() const;
        const std::string		&getUsername() const;
        const std::string		&getFullname() const;
        const std::string		&getHostname() const;

    private:
        Client();
		std::string				_inputBuffer;
        int						_socketFd;
        std::string			   	_nickname;
        std::string         	_username;
        std::string				_fullname;
        std::string         	_hostname;
        std::list<Channel *>	_channels;
        std::list<Channel *>	_invitations;

		// List of all nicknames that are already in use
        static std::set<std::string> 	_nicknames;
};

#endif