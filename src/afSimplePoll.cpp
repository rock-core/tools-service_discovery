/*
 * afSimplePoll.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afSimplePoll.h"
#include <iostream>
afSimplePoll::afSimplePoll() {
	poll = avahi_simple_poll_new();
}

afSimplePoll::~afSimplePoll() {
	if (poll) {
		avahi_simple_poll_free(poll);
	}
}

void afSimplePoll::dispatch() {
	avahi_simple_poll_loop(poll);
}

void afSimplePoll::stop() {
	avahi_simple_poll_quit(poll);
}

const AvahiPoll* afSimplePoll::getAvahiPoll() {
	return avahi_simple_poll_get(poll);
}
