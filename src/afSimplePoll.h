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

class afSimplePoll: public afPoll {
private:
	AvahiSimplePoll *poll;
public:
	afSimplePoll();
	virtual ~afSimplePoll();

	void dispatch();
	void stop();
	const AvahiPoll* getAvahiPoll();
};

#endif /* AFSIMPLEPOLL_H_ */
