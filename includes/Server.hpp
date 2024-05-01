#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <exception>
#include <list>
#include <poll.h>
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
    public:
        Server(const std::string &port, const std::string &password) throw(std::exception);
        ~Server();

        void                    initNetwork() throw(std::exception);
        void                    goOnline();
        void                    shutDown();

    private:
        Server();
        
        void                    parseArgs(const std::string &port, const std::string &password) throw(std::exception);
        // void                    addClient(Client *client);
        // void                    removeClient(Client *client);
        // void                    addChannel(Channel &channel);
        // void                    removeChannel(Channel *channel);
        void                    broadcastMessage(const std::string &ircMessage) const;
        
        std::string             _port;
        std::string             _password;
        std::list<Client>       _clients;
        std::list<Channel>      _channels;
};

#endif