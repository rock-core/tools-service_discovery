/*
 * \file client.cpp
 * \author darko.makreshanski@dfki.de
 * \author thomas.roehr@dfki.de
 */

#include <service_discovery/client.h>
#include <base/logging.h>
#include <stdexcept>

namespace servicediscovery { 

AvahiThreadedPoll* Client::msPoll = 0;
AvahiClient* Client::msAvahiClient = 0;

Client::Client() {

        msPoll = avahi_threaded_poll_new();

	int error;
	//the following creation of avahi msAvahiClient will fail if the avahi daemon is not available, 
	//this can be changed by setting a AVAHI_CLIENT_NO_FAIL as a flag
	msAvahiClient = avahi_client_new( avahi_threaded_poll_get(msPoll), (AvahiClientFlags) 0, &Client::stateUpdateCallback, NULL, &error);

	// If creation of msAvahiClient is not immediately successful throw error 
	if (!msAvahiClient) {
	    LOG_FATAL("Failed to create client: %s" , avahi_strerror(error));
            throw std::runtime_error("Failed to create avahi client");
	}

        avahi_threaded_poll_start(msPoll);
}

AvahiClient* Client::getAvahiClient()
{
    return msAvahiClient;
}

Client::~Client() {
        avahi_threaded_poll_stop(msPoll);
	avahi_client_free(msAvahiClient);
        avahi_threaded_poll_free(msPoll);
}

void Client::lock() {
        avahi_threaded_poll_lock(msPoll);
}

void Client::unlock() {
        avahi_threaded_poll_unlock(msPoll);
}

void Client::stateUpdateCallback(AvahiClient* avahiClient, AvahiClientState state, void* userdata) {
    LOG_DEBUG(" Clientstate: %d\n", state);
}

} // end namespace servicediscovery
