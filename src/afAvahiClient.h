/*
 * afAvahiClient.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFAVAHICLIENT_H_
#define AFAVAHICLIENT_H_

#define DEFAULT_POLL afThreadPoll

namespace dfki {
namespace communication {

class afAvahiClient;
		
}
}

#include <avahi-client/client.h>
#include <sigc++/sigc++.h>

#include <string>
#include <iostream>
#include <cassert>

#include "afPoll.h"
#include "afSimplePoll.h"
#include "afThreadPoll.h"
//#include "afServiceBrowser.h"

namespace dfki {
namespace communication {

/**
 * @brief
 * The "mother" class
 * Every avahi program must set up a client that will be connected to the avahi-daemon
 */
class afAvahiClient {
private:
	/**
	 * the correspondent AvahiClient instance from the avahi C api
	 */
	AvahiClient *client;
	/**
	 * every avahi client program must enter in a poll loop to speak to the avahi-daemon and receive asynchronous messages
	 */
	afPoll *poll;
	
	bool locallyAllocated;

public:
	/**
	 * default constructor with default Poll and no flags
	 */
	afAvahiClient();
	/**
	 * constructor with custom poll and flags
	 */
	afAvahiClient(afPoll *poll, AvahiClientFlags flags);
	virtual ~afAvahiClient();

	/**
	 * signal for asynchronous message on a client state change.
	 * currently not functional TODO implement this
	 */
	sigc::signal<void, afAvahiClient*, AvahiClientState, void*> AvahiClientStateChanged;

	/**
	 * enter main loop
	 */
	void dispatch();
	/**
	 * exit main loop
	 */
	void stop();
	
	afPoll* getPoll() {return poll;}

	AvahiClient* getAvahiClient();

};


}
}

#endif /* AFAVAHICLIENT_H_ */
