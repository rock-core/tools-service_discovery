/*
 * client.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef _SERVICE_DISCOVERY_AVAHICLIENT_H_
#define _SERVICE_DISCOVERY_AVAHICLIENT_H_

#define DEFAULT_POLL ThreadPoll

namespace servicediscovery {

class Client;
		
}

#include <avahi-client/client.h>
#include <sigc++/sigc++.h>

#include <string>
#include <iostream>
#include <cassert>

#include <service_discovery/simple_poll.h>
#include <service_discovery/thread_poll.h>
#include <service_discovery/logging.h>

namespace servicediscovery {

/**
 * @brief
 * Client represent the core class since every avahi program must set up a client that will be connected to the avahi-daemon
 */
class Client {
private:
	/**
	 * the correspondent Client instance from the avahi C api
	 */
	AvahiClient *client;
	/**
	 * every avahi client program must enter in a poll loop to speak to the avahi-daemon and receive asynchronous messages
	 */
	Poll *poll;
	
	bool locallyAllocated;

public:
	/**
	 * default constructor with default Poll and no flags
	 */
	Client();
	/**
	 * constructor with custom poll and flags
	 */
	Client(Poll *poll, AvahiClientFlags flags);
	virtual ~Client();

	/**
	 * signal for asynchronous message on a client state change.
	 * currently not functional TODO implement this
	 */
	sigc::signal<void, Client*, AvahiClientState, void*> ClientStateChanged;

	/**
	 * enter main loop
	 */
	void dispatch();
	/**
	 * exit main loop
	 */
	void stop();
	
	Poll* getPoll() {return poll;}

	AvahiClient* getAvahiClient();

};

} // end namespace servicediscovery

#endif /* _SERVICE_DISCOVERY_AVAHICLIENT_H_ */
