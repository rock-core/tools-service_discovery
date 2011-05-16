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

#include <avahi-common/error.h>
#include <avahi-common/thread-watch.h>
#include <base/logging.h>
#include <base/singleton.h>

namespace servicediscovery {

/**
 * @brief
 * Client represent the core class since every avahi program must set up a client that will be connected to the avahi-daemon
 */
class Client : public base::Singleton<Client> 
{
        friend class base::Singleton<Client>;

protected:
	/**
	 * Default constructor with default Poll and no flags
	 */
	Client();
private:
	/**
	 * the correspondent Client instance from the avahi C api
	 */
	static AvahiClient* msAvahiClient;

	/**
	 * Every avahi client program must enter in a poll loop to speak to the avahi-daemon and receive asynchronous messages
         * we will use a single poll instance
	 */
        static AvahiThreadedPoll* msPoll; 
	
        static void stateUpdateCallback(AvahiClient* client, AvahiClientState state, void* userdata);


public:
	virtual ~Client();

        static AvahiClient* getAvahiClient();

        static void lock();

        void unlock();


};

} // end namespace servicediscovery

#endif /* _SERVICE_DISCOVERY_AVAHICLIENT_H_ */