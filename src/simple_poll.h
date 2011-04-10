/*
 * simple_poll.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef _SERVICE_DISCOVERY_SIMPLEPOLL_H_
#define _SERVICE_DISCOVERY_SIMPLEPOLL_H_

#include <service_discovery/poll.h>
#include <avahi-common/simple-watch.h>
#include <iostream>

namespace servicediscovery {

/**
 * @brief
 * a simple event loop poll for avahi
 */
class SimplePoll: public Poll {

private:
	AvahiSimplePoll *poll;
public:
	SimplePoll();
	virtual ~SimplePoll();
	
	/**
	 * ter dispatching program enters in main event loop. For running an event loop in separate thread use ThreadPoll
	 */
	void dispatch();

	/**
	 * exit main event loop. For SimpleThread this must be done asynchronously
	 */
	void stop();

	const AvahiPoll* getAvahiPoll();
};

}

#endif /* _SERVICE_DISCOVERY_SIMPLEPOLL_H_ */
