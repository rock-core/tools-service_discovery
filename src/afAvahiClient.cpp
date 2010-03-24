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

AvahiClient* afAvahiClient::getAvahiClient() {
	return client;
}

afAvahiClient::afAvahiClient() {
	afPoll *p;
	p = new DEFAULT_POLL();
	poll = p;
	int error;
	client = avahi_client_new(poll->getAvahiPoll(), (AvahiClientFlags) 0, NULL, NULL, &error);
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
	delete poll;
}

void afAvahiClient::dispatch() {
	poll->dispatch();
}

void afAvahiClient::stop() {
	poll->stop();
}
