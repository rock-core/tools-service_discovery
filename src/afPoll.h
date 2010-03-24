/*
 * afPoll.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFPOLL_H_
#define AFPOLL_H_

#include <avahi-common/watch.h>
#include <avahi-common/error.h>

class afPoll {
public:
	afPoll();
	virtual ~afPoll();

	virtual void dispatch() = 0;
	virtual void stop() = 0;
	virtual const AvahiPoll* getAvahiPoll() = 0;

};

#endif /* AFPOLL_H_ */
