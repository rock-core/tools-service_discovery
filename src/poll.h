/*
 * poll.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef _SERVICE_DISCOVERY_POLL_H_
#define _SERVICE_DISCOVERY_POLL_H_

#include <avahi-common/watch.h>
#include <avahi-common/error.h>

namespace servicediscovery {

/**
 * @brief
 * base class for avahi poll
 * possible polls are:
 *		- a simple poll
 *		- a threaded poll which when dispatched goes into a separated poll
 *		- a poll which is integrated with the qt main loop
 */
class Poll {
public:
	Poll() {}
	virtual ~Poll() {}

	/** run the loop */
	virtual void dispatch() = 0;
	/** exit the loop */
	virtual void stop() = 0;
	/** return the correspondent avahi poll */
	virtual const AvahiPoll* getAvahiPoll() = 0;

};

} // end namespace servicediscovery

#endif /* _SERVICE_DISCOVERY_POLL_H_ */
