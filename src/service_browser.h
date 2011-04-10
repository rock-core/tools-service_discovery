/*
 * service_browser.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef _SERVICE_DISCOVERY_SERVICEBROWSER_H_
#define _SERVICE_DISCOVERY_SERVICEBROWSER_H_

namespace servicediscovery {

struct ResolveData;
class ServiceBrowser;

}

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <semaphore.h>
#include <sigc++/sigc++.h>
#include <avahi-client/client.h>
#include <service_discovery/remote_service.h>
#include <service_discovery/client.h>
#include <service_discovery/list.h>
#include <service_discovery/service.h>

namespace servicediscovery {

/**
 * @brief
 * struct used to count failed attempts of service resolution
 */
struct ResolveData {
	ServiceBrowser *sb;
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

	sem_t services_sem;

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
	sigc::signal<void,
		RemoteService> ServiceRemoved;

	sem_t service_added_sem;

	sem_t service_removed_sem;

public:

	/**
	 * the browser will keep the service resolver object upon this amount of failed attempts
	 */
	int serviceResolveTryCount;
	
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

	/** generic signal for a browser event */
//	sigc::signal<void,
//		ServiceBrowser*,
//	    AvahiBrowserEvent,
//		RemoteService*,
//		void*> ServiceBrowserSignal;


	void serviceAddedConnect(const sigc::slot<void, RemoteService>& slot_) {
		sem_wait(&service_added_sem);
		ServiceAdded.connect(slot_);
		sem_post(&service_added_sem);
	}

	void serviceAddedEmit(RemoteService rms) {
		sem_wait(&service_added_sem);
		ServiceAdded.emit(rms);
		sem_post(&service_added_sem);
	}


	void serviceRemovedConnect(const sigc::slot<void, RemoteService>& slot_) {
		sem_wait(&service_removed_sem);
		ServiceRemoved.connect(slot_);
		sem_post(&service_removed_sem);
	}

	void serviceRemovedEmit(RemoteService rms) {
		sem_wait(&service_removed_sem);
		ServiceRemoved.emit(rms);
		sem_post(&service_removed_sem);
	}

    List<RemoteService> getServices()
    {
    	sem_wait(&services_sem);
    	List<RemoteService> tlist = services;
    	sem_post(&services_sem);
        return tlist;
    }
    
    //to be used only by the static callbacks. TODO: how to avoid this to be public
    List<RemoteService> *getInternalServices()
    {
    	return &services;
    }
    //to be used only by the static callbacks. TODO: how to avoid this to be public
    sem_t* getServicesSem()
    {
    	return &services_sem;
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

} // end namespace servicediscovery

#endif /* _SERVICE_DISCOVERYSERVICEBROWSER_H_ */
