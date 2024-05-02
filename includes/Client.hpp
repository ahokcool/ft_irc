/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:33 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/02 22:36:55 by astein           ###   ########.fr       */
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
        Client(const int socketFd, const std::string &nickname) throw(NickNameException);
        ~Client();

        void                    sendMessage(const std::string &ircMessage) const;

        void                    joinChannel(Channel *channel);
        void                    leaveChannel(Channel *channel);
        void                    getInvited(Channel *channel);
        void                    getKicked(Channel *channel);
        
        void					setUsername(const std::string &username);
        void                    setHostname(const std::string &hostname);
		int						getSocketFd() const;
        const std::string		&getNickname() const;
        const std::string		&getUsername() const;
        const std::string		&getHostname() const;

		static void				reserveNickName(const std::string &nickname) throw(NickNameException);
    
    private:
        Client();
        const int               		_socketFd;
    	  std::string			       		_nickname;
        std::string             		_username;
        std::string             		_hostname;
        std::list<Channel *>    		_channels;
        std::list<Channel *>    _invitations;

		// List of all nicknames that are already in use
		  static std::set<std::string> 	_nicknames;
};

#endif