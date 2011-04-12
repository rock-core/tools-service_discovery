/*
 * thread_poll.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "thread_poll.h"


namespace servicediscovery {


ThreadPoll::ThreadPoll() : poll(0) {
	poll = avahi_threaded_poll_new();
}

ThreadPoll::~ThreadPoll() {
	if (poll) {
		avahi_threaded_poll_free(poll);
	}
}

void ThreadPoll::stop()
{
	avahi_threaded_poll_stop(poll);
}

void ThreadPoll::dispatch()
{
	avahi_threaded_poll_start(poll);
}

const AvahiPoll* ThreadPoll::getAvahiPoll() {
	return avahi_threaded_poll_get(poll);
}

AvahiThreadedPoll* ThreadPoll::getAvahiThreadedPoll() {
        return poll;
}

} // end namepace servicediscovery

