/*
 * client.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include <iostream>
#include <cassert>

#include <service_discovery/client.h>

namespace servicediscovery { 

static LoggingWrapper logger("Client");

AvahiClient* Client::getAvahiClient() {
	return client;
}

Client::Client() {
	Poll *p;
	p = new DEFAULT_POLL();
	locallyAllocated = true;
	poll = p;
	int error;
	//the following creation of avahi client will fail if the avahi daemon is not available, 
	//this can be changed by setting a AVAHI_CLIENT_NO_FAIL as a flag
	client = avahi_client_new(poll->getAvahiPoll(), (AvahiClientFlags) 0, NULL, NULL, &error);
	//if creation of client is not immediately successful throw error 
	if (!client) {
		logger.log(FATAL, "Failed to create client: %s" , avahi_strerror(error));
        throw 0; //TODO: do sth else
	}
}

Client::Client(Poll *poll, AvahiClientFlags flags)
{
	this->poll = poll;
	locallyAllocated = false;
	int error;
	client = avahi_client_new(poll->getAvahiPoll(), flags, NULL, NULL, &error);
	//if creation of client is not immediately successful throw error 
	if (!client) {
		logger.log(FATAL, "Failed to create client: %s" , avahi_strerror(error));
        throw 0; //TODO: do sth else
	}
}

Client::~Client() {
	logger.log(DEBUG, "Destructing client");
	if (client) {
		avahi_client_free(client);
                client = NULL;
	}
	if (poll && locallyAllocated) {
		delete poll;
	}
}

void Client::dispatch() {
	logger.log(DEBUG, "Dispatching client");
	poll->dispatch();
}

void Client::stop() {
	logger.log(DEBUG, "Stopping client");
	poll->stop();
}

} // end namespace servicediscovery
