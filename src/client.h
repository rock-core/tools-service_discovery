/*
 * client.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef _SERVICE_DISCOVERY_AVAHICLIENT_H_
#define _SERVICE_DISCOVERY_AVAHICLIENT_H_

#define DEFAULT_THREAD_POLL

#ifdef DEFAULT_THREAD_POLL
#define DEFAULT_POLL ThreadPoll
#else
#define DEFAULT_POLL SimplePoll
#endif

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
	AvahiClient* mClient;
	/**
	 * every avahi client program must enter in a poll loop to speak to the avahi-daemon and receive asynchronous messages
	 */
        DEFAULT_POLL* mPoll; 
	
	bool mLocallyAllocated;

        static void stateUpdateCallback(AvahiClient* client, AvahiClientState state, void* userdata);


public:
	/**
	 * Default constructor with default Poll and no flags
	 */
	Client();

	/**
	 * Constructor with custom poll and flags
	 */
	Client(Poll *poll, AvahiClientFlags flags);

	virtual ~Client();

	/**
	 * enter main loop
	 */
	void dispatch();
	/**
	 * exit main loop
	 */
	void stop();
	
	Poll* getPoll();

	AvahiClient* getAvahiClient();

        void lock();

        void unlock();


};

} // end namespace servicediscovery

#endif /* _SERVICE_DISCOVERY_AVAHICLIENT_H_ */
