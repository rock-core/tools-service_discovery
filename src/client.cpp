/*
 * mClient.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include <iostream>
#include <cassert>

#include <service_discovery/client.h>
#include <avahi-common/thread-watch.h>

namespace servicediscovery { 

static LoggingWrapper logger("Client");

AvahiClient* Client::getAvahiClient() {
	return mClient;
}

Client::Client() {
	mPoll = new DEFAULT_POLL();
	mLocallyAllocated = true;

	int error;

	//the following creation of avahi mClient will fail if the avahi daemon is not available, 
	//this can be changed by setting a AVAHI_CLIENT_NO_FAIL as a flag
	mClient = avahi_client_new( mPoll->getAvahiPoll(), (AvahiClientFlags) 0, &Client::stateUpdateCallback, NULL, &error);

	//if creation of mClient is not immediately successful throw error 
	if (!mClient) {
	    logger.log(FATAL, "Failed to create client: %s" , avahi_strerror(error));
            throw 0; //TODO: do sth else
	}
}

Client::Client(Poll *poll, AvahiClientFlags flags)
{
	this->mPoll = dynamic_cast<DEFAULT_POLL*>(poll);
        assert(this->mPoll);

	mLocallyAllocated = false;
	int error;

        // poll hast to be already started
        lock();
	mClient = avahi_client_new(mPoll->getAvahiPoll(), flags, NULL, NULL, &error);
        unlock();

	//if creation of mClient is not immediately successful throw error 
	if (!mClient) {
		logger.log(FATAL, "Failed to create mClient: %s" , avahi_strerror(error));
        throw 0; //TODO: do sth else
	}
}

Client::~Client() {
	logger.log(DEBUG, "Destructing mClient");
	if (mClient) {
		avahi_client_free(mClient);
                mClient = NULL;
	}
	if (mPoll && mLocallyAllocated) {
		delete mPoll;
	}
}

void Client::dispatch() {
	logger.log(DEBUG, "Dispatching mClient");
	mPoll->dispatch();
}

void Client::stop() {
	logger.log(DEBUG, "Stopping mClient");
	mPoll->stop();
} 


Poll* Client::getPoll() {
    return mPoll;
}

void Client::lock() {
        #ifdef DEFAULT_THREAD_POLL
        logger.log(INFO, "Client lock");
        AvahiThreadedPoll* poll = mPoll->getAvahiThreadedPoll();
        assert(poll);
        avahi_threaded_poll_lock(poll);
        #endif
}

void Client::unlock() {
        #ifdef DEFAULT_THREAD_POLL
        logger.log(INFO, "Client unlock");
        AvahiThreadedPoll* poll = mPoll->getAvahiThreadedPoll();
        assert(poll);
        avahi_threaded_poll_unlock(poll);
        #endif
}

void Client::stateUpdateCallback(AvahiClient* mClient, AvahiClientState state, void* userdata) {
//    logger.log(INFO," Clientstate: %d\n", state);
}

} // end namespace servicediscovery
