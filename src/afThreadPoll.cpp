/*
 * afThreadPoll.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afThreadPoll.h"


namespace rock { namespace communication {


afThreadPoll::afThreadPoll() {
	poll = avahi_threaded_poll_new();
}

afThreadPoll::~afThreadPoll() {
	if (poll) {
		avahi_threaded_poll_free(poll);
	}
}

void afThreadPoll::stop()
{
	avahi_threaded_poll_stop(poll);
}

void afThreadPoll::dispatch()
{
	avahi_threaded_poll_start(poll);
}

const AvahiPoll* afThreadPoll::getAvahiPoll() {
	return avahi_threaded_poll_get(poll);
}


}}

