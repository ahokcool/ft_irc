#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <list>
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
};

#endif