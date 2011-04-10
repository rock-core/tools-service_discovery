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
	
	/**
	 * upon dispatching you must make sure to synchronize access to the poll and other related objects, thus lock and unlock are provided
	 */
	void lock() {avahi_threaded_poll_lock(poll);}

	/**
	 * counterpart to lock
	 */
	void unlock() {avahi_threaded_poll_unlock(poll);}
	
};


} // end servicediscovery

#endif /* _SERVICE_DISCOVERY_THREADPOLL_H_ */

