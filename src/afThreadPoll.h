/*
 * afThreadPoll.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFTHREADPOLL_H_
#define AFTHREADPOLL_H_

#include "afPoll.h"

class afThreadPoll: public afPoll {
public:
	afThreadPoll();
	virtual ~afThreadPoll();
};

#endif /* AFTHREADPOLL_H_ */
