#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <list>
#include "Client.hpp"

class Client;

class Channel
{
    public:
        Channel(const std::string &name, Client *client);
        ~Channel();

        void                    sendMessage(const std::string &ircMessage) const;
        void                    addClient(Client *client);
        void                    removeClient(Client *client); // delete empty channel
        const std::string       &getName() const;
    //TODO: if all the ops left the channel, kick all the clients and delete the channel
    private:
        Channel();
        const std::string       _name;
        std::string             _topic;
        std::string             _password; // empty string means no password
        size_t                  _userLimit; // 0 means unset
        bool                    _inviteOnly;
        bool                    _topicProtected;
        std::list<Client *>     _clients;
        std::list<Client *>     _operators;
};

#endif