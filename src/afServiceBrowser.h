/*
 * afServiceBrowser.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFSERVICEBROWSER_H_
#define AFSERVICEBROWSER_H_

namespace dfki {
namespace communication {

struct ResolveData;
class afServiceBrowser;

}
}

#include <sigc++/sigc++.h>
#include <avahi-client/client.h>
#include "afRemoteService.h"
#include "afAvahiClient.h"
#include "afList.h"
#include "afService.h"
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <semaphore.h>

#include "ServiceDescription.h"

namespace dfki {
namespace communication {

/**
 * @brief
 * struct used to count failed attempts of service resolution
 */
struct ResolveData {
	afServiceBrowser *sb;
	int count;
};


/**
 * a class encapsulating the service browser object of avahi
 * it also keeps track of all currently active services
 * TODO service browser can browse only services of specific type. Implement a service type browser
 */
class afServiceBrowser {
private:
	/** related avahi client instance */
	afAvahiClient* client;
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

	afList<afRemoteService> services;

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
	sigc::signal<void, afRemoteService> afServiceAdded;

	/** signal for removal of a service */
	sigc::signal<void,
		afRemoteService> afServiceRemoved;

	sem_t service_added_sem;

	sem_t service_removed_sem;

public:

	/**
	 * the browser will keep the service resolver object upon this amount of failed attempts
	 */
	int serviceResolveTryCount;

	afServiceBrowser(
			afAvahiClient *client,
			std::string type);
	afServiceBrowser(
			afAvahiClient *client,
			AvahiIfIndex interface, /**< In most cases pass AVAHI_IF_UNSPEC here */
			AvahiProtocol protocol, /**< In most cases pass AVAHI_PROTO_UNSPEC here */
			std::string type,  /**< A service type such as "_http._tcp" */
			std::string domain, /**< A domain to browse in. In most cases you want to pass NULL here for the default domain (usually ".local") */
			AvahiLookupFlags flags,
			void*);
	virtual ~afServiceBrowser();

	/** generic signal for a browser event */
//	sigc::signal<void,
//		afServiceBrowser*,
//	    AvahiBrowserEvent,
//		afRemoteService*,
//		void*> afServiceBrowserSignal;


	void serviceAddedConnect(const sigc::slot<void, afRemoteService>& slot_) {
		sem_wait(&service_added_sem);
		afServiceAdded.connect(slot_);
		sem_post(&service_added_sem);
	}

	void serviceAddedEmit(afRemoteService rms) {
		sem_wait(&service_added_sem);
		afServiceAdded.emit(rms);
		sem_post(&service_added_sem);
	}


	void serviceRemovedConnect(const sigc::slot<void, afRemoteService>& slot_) {
		sem_wait(&service_removed_sem);
		afServiceRemoved.connect(slot_);
		sem_post(&service_removed_sem);
	}

	void serviceRemovedEmit(afRemoteService rms) {
		sem_wait(&service_removed_sem);
		afServiceRemoved.emit(rms);
		sem_post(&service_removed_sem);
	}

    afList<afRemoteService> getServices()
    {
    	sem_wait(&services_sem);
    	afList<afRemoteService> tlist = services;
    	sem_post(&services_sem);
        return tlist;
    }
    
    //to be used only by the static callbacks. TODO: how to avoid this to be public
    afList<afRemoteService> *getInternalServices()
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

    afAvahiClient* getClient()
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

}
}


#endif /* AFSERVICEBROWSER_H_ */
