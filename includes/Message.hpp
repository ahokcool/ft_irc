/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anshovah <anshovah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 16:37:13 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/02 20:36:49 by anshovah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include "Client.hpp"

// Standart exception class for Message
class MessageException : public std::exception
{
	public:
		MessageException(const std::string &message);
		~MessageException() 			throw();
		const char	*what() const	throw();

	private:
		std::string _message;
};

class Message
{
    public:
        Message(Client *sender, const std::string &ircMessage) throw(MessageException);
        ~Message();

        void setReceiver(Client *receiver);
        Client *getSender() const;
        Client *getReceiver() const;
        const std::string &getCmd() const;
        const std::string &getChannelName() const;
        const std::string &getArg1() const;
        const std::string &getArg2() const;

    private:
        Client      *_sender;
        Client      *_receiver; //TODO: do we  need this?
        std::string _cmd;
        std::string _channelName;
        std::string _arg1;
        std::string _arg2;

        Message();
};

/*
    1. copy code form the other thingy |
    2. put the logic int hte param cinstryuctor | 
    3. private the default one | 
    4. create an instance if Message in the function in server without new |
    5. implement message expection like the server one |
    6. the constuctor should throw it if fucks up in consturctor | 
    7. implement logic of PRIVMSG in switch ||
    8. implement private class funcions for each switch case in Server.cpp |
*/

#endif