/*
 * afRemoteService.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFREMOTESERVICE_H_
#define AFREMOTESERVICE_H_

namespace dfki {
namespace communication {

class afRemoteService;
class afServiceBrowser;
struct ResolveData;

}
}

#include <sigc++/sigc++.h>
#include "afService.h"
#include <avahi-client/lookup.h>
#include <iostream>
#include <semaphore.h>


//typedef sigc::slot<void,
//	afRemoteService*,
//	AvahiResolverEvent,
//	AvahiLookupResultFlags,
//	void*> afAvahiRemoteServiceSlot;

namespace dfki {
namespace communication {

/**
 * @brief
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
	
	/**
	 * signal  
	 */
	sigc::signal<void,
		afRemoteService> *afRemoteServiceSignal;

	/**
	 * semaphore for the afRemoteServiceSignal object
	 */
	sem_t afRMS_sem;
	 
public:

	//the data used in the service browser. pointer also used here so that the memory is freed along with the service resolver
	ResolveData* resolveData;

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
			AvahiServiceResolver *sr,
			sigc::signal<void,
		afRemoteService> *afrms

	);
	virtual ~afRemoteService();

	bool serviceSignalConnect(const sigc::slot<void, afRemoteService>& slot_) {
		if (!afRemoteServiceSignal) {
			return false;
		}
		sem_wait(&afRMS_sem);
		afRemoteServiceSignal->connect(slot_);
		sem_post(&afRMS_sem);		
		return true;
	}
	
	//to be used only by the service browser. TODO: avoid public use
	void emitSignal() {
		if (afRemoteServiceSignal) {
			sem_wait(&afRMS_sem);
			afRemoteServiceSignal->emit(*this);
			sem_post(&afRMS_sem);
		}
	}
	//to be used only by the service browser. TODO: avoid public use
	void freeSignal() {
		if (afRemoteServiceSignal) {
			delete afRemoteServiceSignal;
		}
	}
	
//	sigc::signal<void, afRemoteService> * getSignal() {
//		return afRemoteServiceSignal;
//	}
	
	
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

}
}

#include "afServiceBrowser.h"

#endif /* AFREMOTESERVICE_H_ */
