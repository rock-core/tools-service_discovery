/*
 * afRemoteService.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFREMOTESERVICE_H_
#define AFREMOTESERVICE_H_

class afRemoteService;

#include <sigc++/sigc++.h>
#include "afService.h"
#include "afServiceBrowser.h"
#include <avahi-client/lookup.h>

//typedef sigc::slot<void,
//	afRemoteService*,
//	AvahiResolverEvent,
//	AvahiLookupResultFlags,
//	void*> afAvahiRemoteServiceSlot;


/**
 * class for handling service that are found and resolved by the service browsers
 */
class afRemoteService: public afService {
protected:
	/**
	 * a service browser instance connected to this remote service
	 */
	afServiceBrowser *browser;
	/**
	 * host name of the remote machine where the service is present
	 */
	std::string host_name;
	/**
	 * address of the remote machine where the service is present
	 */
	AvahiAddress address;

	/**
	 * avahi service resolver object
	 */
	AvahiServiceResolver *sr;
public:

	afRemoteService(
			afServiceBrowser *browser,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain,
			std::list<std::string> list,
			uint16_t port,
			std::string host_name,
			AvahiAddress address,
			AvahiServiceResolver *sr
	);
	virtual ~afRemoteService();

	//signal for avahi resolver event. TODO this is not implemented yet, and needs to be encapsulated
//	sigc::signal<void,
//		afRemoteService*,
//		AvahiResolverEvent,
//		AvahiLookupResultFlags,
//		void*> afAvahiRemoteServiceSignal;
	sigc::signal<void,
		afRemoteService*> afRemoteServiceSignal;

    AvahiAddress getAddress() const
    {
        return address;
    }

    afServiceBrowser *getBrowser() const
    {
        return browser;
    }

    std::string getHostName() const
    {
        return host_name;
    }

    AvahiServiceResolver* getServiceResolver()
	{
    	return sr;
	}

    void setServiceResolver(AvahiServiceResolver* nsr)
    {
    	if (sr)
    		avahi_service_resolver_free(sr);
    }

    std::string getAddressString() {
        char a[AVAHI_ADDRESS_STR_MAX];
        avahi_address_snprint(a, sizeof(a), &address);
        std::string addr(a);
		return addr;
    }

    bool operator==(afRemoteService serv);
};

#endif /* AFREMOTESERVICE_H_ */
