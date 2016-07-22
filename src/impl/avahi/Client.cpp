#include "Client.hpp"
#include <base-logging/Logging.hpp>
#include <stdexcept>
#include <algorithm>
#include <dbus/dbus.h>

namespace servicediscovery { 
namespace avahi {

AvahiThreadedPoll* Client::msPoll = 0;
AvahiClient* Client::msAvahiClient = 0;

Client::Client()
    : mMode(INIT) {
	// Making sure dbus is operating in a thread safe manner
        dbus_threads_init_default();
        bool runningDaemonRequired = true;
        start(runningDaemonRequired);
}

AvahiClient* Client::getAvahiClient() {
    return msAvahiClient;
}

AvahiClientState Client::getAvahiClientState() {
    UniqueClientLock lock;
    return avahi_client_get_state(msAvahiClient);
}

Client::~Client() {
}

void Client::lock() {
        avahi_threaded_poll_lock(msPoll);
}

void Client::unlock() {
        avahi_threaded_poll_unlock(msPoll);
}

void Client::cleanup() {
	avahi_threaded_poll_quit(msPoll);
        avahi_client_free(msAvahiClient);
}

void Client::start(bool runningDaemonRequired) {
        msPoll = avahi_threaded_poll_new();
	int error;

	//the creation of avahi client will fail if the avahi daemon is not available, 
	//this can be changed using the AVAHI_CLIENT_NO_FAIL flag
        AvahiClientFlags flags = (AvahiClientFlags) 0;
        if(!runningDaemonRequired)
        {
            flags = (AvahiClientFlags) AVAHI_CLIENT_NO_FAIL;
        } 
	msAvahiClient = avahi_client_new( avahi_threaded_poll_get(msPoll), flags, &Client::stateUpdateCallback, this, &error);

	// If creation of msAvahiClient is not immediately successful throw error 
	if (!msAvahiClient) {
            char buffer[512];
            snprintf(buffer,512, "Failed to create client: %s" , avahi_strerror(error));
            LOG_FATAL("%s", buffer);
            throw std::runtime_error(buffer);
	} else {
            LOG_DEBUG("Created client");
        }
        avahi_threaded_poll_start(msPoll);
}

void Client::stateUpdateCallback(AvahiClient* avahiClient, AvahiClientState state, void* userdata) {

    Client* client = static_cast<Client*>(userdata);
    switch(state)
    {
        case AVAHI_CLIENT_FAILURE:
        	if(avahi_client_errno(msAvahiClient) == AVAHI_ERR_DISCONNECTED) 
                {
    			client->mMode = RECOVERY;
    			LOG_WARN("Client has been disconnected -- switching to recovery mode");
        		std::vector<ClientObserver*>::iterator it = client->msObservers.begin();
        		for(; it != client->msObservers.end(); ++it)
        		{
        		    ClientObserver::Event event;
        		    event.type = ClientObserver::DISCONNECTED;
        		    (*it)->update(event);
        		}
        		client->cleanup();

                        bool runningDaemonRequired = false;
        		client->start(runningDaemonRequired);
        	} else {
        		client->mMode = NORMAL;
        	        LOG_WARN("Client failure: '%s'", avahi_strerror(avahi_client_errno(msAvahiClient)));
        	}
                break;
        case AVAHI_CLIENT_S_RUNNING:
        	if(client->mMode == RECOVERY)
        	{
        		std::vector<ClientObserver*>::iterator it = client->msObservers.begin();
        		for(; it != client->msObservers.end(); ++it)
        		{
        		    ClientObserver::Event event;
        		    event.type = ClientObserver::RECOVERED;
        		    (*it)->update(event);
        		}
                        LOG_INFO("Client recovery has been initiated -- switching back to normal mode");
        	}
        	client->mMode = NORMAL;
        	break;
        case AVAHI_CLIENT_S_REGISTERING:
        	break;
        case AVAHI_CLIENT_S_COLLISION:
        	break;

        case AVAHI_CLIENT_CONNECTING:
                break;

        default: 
            LOG_DEBUG(" Clientstate: %d\n", state);
            break;
    }
}

void Client::addObserver(ClientObserver* clientObserver) {
    std::vector<ClientObserver*>::iterator it;
    it = std::find(msObservers.begin(), msObservers.end(), clientObserver);
    if(it == msObservers.end())
    {
        msObservers.push_back(clientObserver);
    }
}

void Client::removeObserver(ClientObserver* clientObserver) {
    std::vector<ClientObserver*>::iterator it;
    it = std::find(msObservers.begin(), msObservers.end(), clientObserver);
    if(it != msObservers.end())
    {
        msObservers.erase(it);
    }
}

UniqueClientLock::UniqueClientLock() {
    Client::lock();
}

UniqueClientLock::~UniqueClientLock() {
    Client::unlock();
}

} // end namespace avahi
} // end namespace servicediscovery
