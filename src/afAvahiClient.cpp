/*
 * afAvahiClient.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afAvahiClient.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace dfki::communication;

AvahiClient* afAvahiClient::getAvahiClient() {
	return client;
}

afAvahiClient::afAvahiClient() {
	afPoll *p;
	p = new DEFAULT_POLL();
	locallyAllocated = true;
	poll = p;
	int error;
	//the following creation of avahi client will fail if the avahi daemon is not available, 
	//this can be changed by setting a AVAHI_CLIENT_NO_FAIL as a flag
	client = avahi_client_new(poll->getAvahiPoll(), (AvahiClientFlags) 0, NULL, NULL, &error);
	//if creation of client is not immediately successful throw error 
	if (!client) {
		cerr << "Failed to create client: " << avahi_strerror(error) << endl;
        throw 0; //TODO: do sth else
	}
}

afAvahiClient::afAvahiClient(afPoll *poll, AvahiClientFlags flags)
{
	this->poll = poll;
	locallyAllocated = false;
	int error;
	client = avahi_client_new(poll->getAvahiPoll(), flags, NULL, NULL, &error);
	//if creation of client is not immediately successful throw error 
	if (!client) {
		cerr << "Failed to create client: " << avahi_strerror(error) << endl;
        throw 0; //TODO: do sth else
	}
}

afAvahiClient::~afAvahiClient() {
	if (client) {
//		cout << "..DELETING CLIENT" << endl;
		avahi_client_free(client);
	}
	if (poll && locallyAllocated) {
		delete poll;
	}
}

void afAvahiClient::dispatch() {
	poll->dispatch();
}

void afAvahiClient::stop() {
	cout << "STOPING" << endl;
	poll->stop();
	cout << "STOPED" << endl;
}
