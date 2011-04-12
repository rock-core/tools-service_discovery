/*
 * thread_poll.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef _SERVICE_DISCOVERY_THREADPOLL_H_
#define _SERVICE_DISCOVERY_THREADPOLL_H_

#include <service_discovery/poll.h>
#include <avahi-common/thread-watch.h>

namespace servicediscovery {

/**
 * @brief
 * a threaded poll for avahi. Upon dispatching creates a separate thread and executes main event loop in this thread
 */
class ThreadPoll: public Poll {
private:
	AvahiThreadedPoll *poll;
public:
	ThreadPoll();

	virtual ~ThreadPoll();

	/**
	 * run main loop
	 */
	void dispatch();
	/**
	 * exit main loop
	 */
	void stop();

	const AvahiPoll* getAvahiPoll();

	AvahiThreadedPoll* getAvahiThreadedPoll();
};


} // end servicediscovery

#endif /* _SERVICE_DISCOVERY_THREADPOLL_H_ */

