/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 22:55:33 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/02 22:37:56 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include "Channel.hpp"

class Channel;

class Client
{   
    public:
        Client(const int socketFd, const std::string &nickname);
        ~Client();

        void                    sendMessage(const std::string &ircMessage) const;

        void                    joinChannel(Channel *channel);
        void                    leaveChannel(Channel *channel);
        void                    getInvited(Channel *channel);
        void                    getKicked(Channel *channel);
        
        void                    setUsername(const std::string &username);
        void                    setHostname(const std::string &hostname);
        const std::string       &getNickname() const;
        const std::string       &getUsername() const;
        const std::string       &getHostname() const;
    
    private:
        Client();
        const int               _socketFd;
        const std::string       _nickname;
        std::string             _username;
        std::string             _hostname;
        std::list<Channel *>    _channels;
        std::list<Channel *>    _invitations;
};

#endif