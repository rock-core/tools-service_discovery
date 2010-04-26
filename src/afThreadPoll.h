/*
 * afThreadPoll.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFTHREADPOLL_H_
#define AFTHREADPOLL_H_

#include "afPoll.h"
#include <avahi-common/thread-watch.h>

/**
 * @brief
 * a threaded poll for avahi. Upon dispatching creates a separate thread and executes main event loop in this thread
 */
class afThreadPoll: public afPoll {
private:
	AvahiThreadedPoll *poll;
public:
	afThreadPoll();
	virtual ~afThreadPoll();

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

#endif /* AFTHREADPOLL_H_ */

