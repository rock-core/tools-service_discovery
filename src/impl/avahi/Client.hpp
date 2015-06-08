#ifndef _SERVICE_DISCOVERY_AVAHICLIENT_H_
#define _SERVICE_DISCOVERY_AVAHICLIENT_H_

#define DEFAULT_THREAD_POLL

#ifdef DEFAULT_THREAD_POLL
#define DEFAULT_POLL ThreadPoll
#else
#define DEFAULT_POLL SimplePoll
#endif

#include <avahi-client/client.h>
#include <sigc++/sigc++.h>

#include <string>
#include <iostream>
#include <cassert>
#include <vector>

#include <avahi-common/error.h>
#include <avahi-common/thread-watch.h>
#include <base/Singleton.hpp>

namespace servicediscovery {
namespace avahi {

/**
 * @class ClientObserver
 * ClientObserver interface, which is used by the client in order publish
 * the events required to handle interrupted communication with the avahi-daemon
 *
 */
class ClientObserver
{
public:
    // Client events
    enum EventType { UNKNOWN, DISCONNECTED, RECOVERED };

    // Event structure
    struct Event 
    {
        EventType type;
    };

    virtual ~ClientObserver() {}

    /**
     * Update the observer
     */
    virtual void update(const Event& event) = 0;

    /**
     * Stop any activity that requires service browers etc. and
     * clean them up
     */
    virtual void stop() = 0;
};

/**
 * @brief
 * Client represent a wrapper to access the avahi_client (a singleton).
 * Every avahi program must set up a client and all communicate via a single avahi-client instance with the avahi-daemon
 */
class Client : public base::Singleton<Client> 
{
        friend class base::Singleton<Client>;
        friend class UniqueClientLock;


protected:
	/**
	 * Default constructor with default Poll and no flags
	 */
	Client();

private:
        // Internal modes in order to handle reconnection to the avahi-daemon
        enum Mode { INIT, NORMAL, RECOVERY };

	/**
	 * the correspondent Client instance from the avahi C api
	 */
	static AvahiClient* msAvahiClient;

	/**
	 * Every avahi client program must enter in a poll loop to speak to the avahi-daemon and receive asynchronous messages
         * A single poll instance is used
	 */
        static AvahiThreadedPoll* msPoll; 

        /**
         * List of attached observers, which require notification
         * about changes of the client state
         */
        std::vector<ClientObserver*> msObservers;

        /**
         * Client mode to allow for recovery
         */
        Mode mMode;
	
        /**
         * Callback when state is updated
         */
        static void stateUpdateCallback(AvahiClient* client, AvahiClientState state, void* userdata);

        /**
         * Start client
         * \param runningDaemonRequired By default a running daemon is required for the client start
         */
        void start(bool runningDaemonRequired);

        /**
         * Cleanup client
         */
        void cleanup();

public:
	virtual ~Client();

        /**
         * Get the avahi client instance 
         * \return pointer to the AvahiClient instance
         **/
        static AvahiClient* getAvahiClient();

        /**
         * Get the avahi client state
         * \return AvahiClientState
         */
        static AvahiClientState getAvahiClientState();

        /**
         * Register observer
         */
        void addObserver(ClientObserver* clientObserver);

        /**
         * Remove observer
         */
        void removeObserver(ClientObserver* clientObserver);

protected:
        static void lock();

        static void unlock();
};

/**
 * Lock the client 
 * will release the lock on deconstruction
 */
class UniqueClientLock
{
    public: 
        /**
         * Lock the avahi client
         */
        UniqueClientLock();

        /**
         * Release lock 
         */
        ~UniqueClientLock();
};

} // end namespace avahi
} // end namespace servicediscovery
#endif /* _SERVICE_DISCOVERY_AVAHICLIENT_H_ */
