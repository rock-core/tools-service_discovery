#ifndef _SERVICE_DISCOVERY_SERVICEBROWSER_H_
#define _SERVICE_DISCOVERY_SERVICEBROWSER_H_

namespace servicediscovery {
namespace avahi {

struct ResolveData;
class ServiceBrowser;

}}

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <boost/thread.hpp>
#include <sigc++/sigc++.h>
#include <avahi-client/client.h>
#include <service_discovery/impl/avahi/RemoteService.hpp>
#include <service_discovery/impl/avahi/Client.hpp>
#include <service_discovery/utils/List.hpp>
#include <service_discovery/impl/avahi/Service.hpp>

namespace servicediscovery {
namespace avahi {

/**
 * @brief
 * struct used to count failed attempts of service resolution
 */
struct ResolveData {
        ServiceBrowser* serviceBrowser;
        int count;
};


/**
 * a class encapsulating the service browser object of avahi
 * it also keeps track of all currently active services
 * TODO service browser can browse only services of specific type. Implement a service type browser
 */
class ServiceBrowser {
private:
	/** related client instance */
	Client* client;
	/** type of services that are browsed */
	std::string type;
	/** interface on which services are browsed */
	AvahiIfIndex interface;
	/** protocol on which services are browsed (ipv4 or ipv6 or both) */
	AvahiProtocol protocol;
	/** domain on which services are browsed. Most of the times it should be "" -> NULL -> ".local" */
	std::string domain;
	/** flags */
	AvahiLookupFlags flags;
	
	void *data;

	AvahiServiceBrowser *browser;

	void bootstrap();

	List<RemoteService> services;

	boost::mutex mServicesMutex;

	const char* getInType() {
		return type.c_str();
	}

	const char* getInDomain() {
		if (domain == "") {
			return NULL; //if empty string then the default value NULL (.local) domain is used
		} else {
			return domain.c_str();
		}
	}


	/** signal for addition of a service */
	sigc::signal<void, RemoteService> ServiceAdded;

	/** signal for removal of a service */
	sigc::signal<void, RemoteService> ServiceRemoved;

	/** signal for updating a service */
	sigc::signal<void, RemoteService> ServiceUpdated;

	boost::mutex mServiceAddedMutex;
	boost::mutex mServiceRemovedMutex;
	boost::mutex mServiceUpdateMutex;

public:

	/**
	 * the browser will keep the service resolver object upon this amount of failed attempts
	 */
	const int static MAX_RESOLVER_TRIES = 5;
	
	/**
	 * Class to browse existing services given a avahi client and a type to browse
	 * \param client
	 * \param type
         */
	ServiceBrowser(
			Client *client,
			std::string type);
	/**
	 * Browser for existing services, given specific avahi definitions
	 * \param client Reference to avahi client
	 * \param interface AvahiIfIndex interface, // In most cases pass AVAHI_IF_UNSPEC here 
	 * \param protocol AvahiProtocol protocol, // In most cases pass AVAHI_PROTO_UNSPEC here
	 * \param type  A service type such as "_http._tcp" 
	 * \param domain A domain to browse in. In most cases you want to pass NULL here for the default domain (usually ".local")
	 * \param flags
	 * \param data
         */
	ServiceBrowser(
			Client* client,
			AvahiIfIndex interface, // In most cases pass AVAHI_IF_UNSPEC here 
			AvahiProtocol protocol, // In most cases pass AVAHI_PROTO_UNSPEC here
			std::string type,  // A service type such as "_http._tcp" 
			std::string domain, // A domain to browse in. In most cases you want to pass NULL here for the default domain (usually ".local")
			AvahiLookupFlags flags,
			void* data);

	virtual ~ServiceBrowser();

        /**
         * Cleanup internal instances of avahi_browers and resolvers
         * (do not call if you are using avahi_client_free)
         */
        void cleanup();

	/** generic signal for a browser event */
//	sigc::signal<void,
//		ServiceBrowser*,
//	    AvahiBrowserEvent,
//		RemoteService*,
//		void*> ServiceBrowserSignal;


	void serviceAddedConnect(const sigc::slot<void, RemoteService>& slot_) {
		boost::unique_lock<boost::mutex> lock(mServiceAddedMutex);
		ServiceAdded.connect(slot_);
	}

	void serviceAddedEmit(RemoteService rms) {
		boost::unique_lock<boost::mutex> lock(mServiceAddedMutex);
		ServiceAdded.emit(rms);
	}


	void serviceRemovedConnect(const sigc::slot<void, RemoteService>& slot_) {
		boost::unique_lock<boost::mutex> lock(mServiceRemovedMutex);
		ServiceRemoved.connect(slot_);
	}

	void serviceRemovedEmit(RemoteService rms) {
		boost::unique_lock<boost::mutex> lock(mServiceRemovedMutex);
		ServiceRemoved.emit(rms);
	}

        void serviceUpdatedConnect(const sigc::slot<void, RemoteService>& slot_) {
                boost::unique_lock<boost::mutex> lock(mServiceUpdateMutex);
                ServiceUpdated.connect(slot_);
        }

        void serviceUpdatedEmit(RemoteService rms) {
                boost::unique_lock<boost::mutex> lock(mServiceUpdateMutex);
                ServiceUpdated.emit(rms);
        }

    List<RemoteService> getServices()
    {
        boost::unique_lock<boost::mutex> lock(mServicesMutex);
        List<RemoteService> tlist = services;
        return tlist;
    }
    
    //to be used only by the static callbacks. TODO: how to avoid this to be public
    List<RemoteService> *getInternalServices()
    {
        return &services;
    }
    //to be used only by the static callbacks. TODO: how to avoid this to be public
    boost::mutex& getServicesMutex()
    {
        return mServicesMutex;
    }

    

    AvahiServiceBrowser* getAvahiBrowser()
    {
        return browser;
    }

    Client* getClient()
    {
        return client;
    }


    static void browseCallback(AvahiServiceBrowser *sb, AvahiIfIndex interface, AvahiProtocol protocol,
                                  AvahiBrowserEvent event, const char *name, const char *type, const char *domain,
                                  AvahiLookupResultFlags flags, void *userdata);

    static void resolveCallback(AvahiServiceResolver *sr, AvahiIfIndex interface, AvahiProtocol protocol,
                            AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host,
                            const AvahiAddress *address, uint16_t port, AvahiStringList *list, AvahiLookupResultFlags flags, void *userdata);

};

} // end namespace avahi
} // end namespace servicediscovery
#endif /* _SERVICE_DISCOVERYSERVICEBROWSER_H_ */
