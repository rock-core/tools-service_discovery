/*
 * simple_poll.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "simple_poll.h"

namespace servicediscovery {

SimplePoll::SimplePoll() {
	poll = avahi_simple_poll_new();
}

SimplePoll::~SimplePoll() {
	if (poll) {
		avahi_simple_poll_free(poll);
	}
}

void SimplePoll::dispatch() {
	avahi_simple_poll_loop(poll);
}

void SimplePoll::stop() {
	avahi_simple_poll_quit(poll);
}

const AvahiPoll* SimplePoll::getAvahiPoll() {
	return avahi_simple_poll_get(poll);
}

}
