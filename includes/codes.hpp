/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codes.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astein <astein@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 02:01:01 by anshovah          #+#    #+#             */
/*   Updated: 2024/05/11 17:26:16 by astein           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODES_HPP
#define CODES_HPP

// REPLY CODES
#define RPL_WELCOME				"001"	// "<nick> :Welcome to the FINISHERS' IRC Network, <nick>
#define RPL_WHOISUSER			"311"	// "<nick> <user> <host> * :<real name>"
#define RPL_WHOISCHANNELS		"319"	// "<nick> :{[@|+]<channel><space>}"
#define RPL_WHOREPLY			"352"	// "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"
#define RPL_ENDOFWHO			"315"	// "<name> :End of /WHO list"
#define RPL_ENDOFWHOIS			"318"	// "<nick> :End of /WHOIS list"
#define RPL_NOTOPIC				"331"	// "<channel> :No topic is set"
#define RPL_TOPIC				"332"	// "<channel> :<topic>"
#define RPL_TOPICADDITIONAL		"333"	// "<channel> astein!alex@F456A.75198A.60D2B2.ADA236.IP 1714884181"
#define RPL_INVITING			"341"	// "<channel> <nick>"
#define RPL_NOTOPIC				"331"	//"<channel> :No topic is set"
#define RPL_CHANNELMODEIS		"324"	// "<channel> <mode> <mode params>"

// ERROR CODES
#define ERR_UNKNOWNCOMMAND		"421"	// "<command> :Unknown command"
#define ERR_NORECIPIENT			"411"	// ":No recipient given (<command>)"
#define ERR_NOTEXTTOSEND		"412"	// ":No text to send"
#define ERR_NOSUCHNICK			"401"	// "<nickname>	:No such nick/channel"
#define ERR_NONICKNAMEGIVEN		"431"	// ":No nickname given"
#define ERR_NICKNAMEINUSE		"433"	// "<nick> :Nickname is already in use"
#define ERR_NOTREGISTERED		"451"	// ":You have not registered"
#define ERR_NEEDMOREPARAMS		"461"	// "<command> :Not enough parameters"
#define ERR_ALREADYREGISTRED	"462"	// "<nick> :You may not reregister"
#define ERR_NOSUCHCHANNEL		"403"	// "<channel name> :No such channel"
#define ERR_USERONCHANNEL		"443"	// "<user> <channel> :is already on channel"
#define ERR_BADCHANNELKEY		"475"	// "<channel>	:Cannot join channel (+k)"
#define ERR_INVITEONLYCHAN		"473"	// "<channel>	:Cannot join channel (+i)"
#define ERR_CHANNELISFULL		"471"	// "<channel>	:Cannot join channel (+l)"
#define ERR_NOTONCHANNEL		"442"	// "<channel> 	:You're not on that channel"
#define ERR_CHANOPRIVSNEEDED	"482"	// "<channel> 	:You're not channel operator"
#define ERR_USERNOTINCHANNEL	"441"	// "<nick> <channel> :They aren't on that channel
#define ERR_UNKNOWNMODE			"472"	// "<char> :is unknown mode char to me"
#define ERR_KEYSET				"467"	// "<channel>	:Channel key already set"

#endif
