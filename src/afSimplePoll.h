/*
 * afSimplePoll.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFSIMPLEPOLL_H_
#define AFSIMPLEPOLL_H_

#include "afPoll.h"
#include <avahi-common/simple-watch.h>
#include <iostream>

namespace rock {
namespace communication {


/**
 * @brief
 * a simple event loop poll for avahi
 */
class afSimplePoll: public afPoll {
private:
	AvahiSimplePoll *poll;
public:
	afSimplePoll();
	virtual ~afSimplePoll();
	
	/**
	 * after dispatching program enters in main event loop. For running an event loop in separate thread use afThreadPoll
	 */
	void dispatch();
	/**
	 *	exit main event loop. For afSimpleThread this must be done asynchronously
	 */
	void stop();
	const AvahiPoll* getAvahiPoll();
};


}
}

#endif /* AFSIMPLEPOLL_H_ */
