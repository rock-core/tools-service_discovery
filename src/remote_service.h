/*
 * remote_service.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef _SERVICE_DISCOVERYREMOTESERVICE_H_
#define _SERVICE_DISCOVERYREMOTESERVICE_H_

namespace servicediscovery {

class RemoteService;
class ServiceBrowser;
struct ResolveData;

}

#include <sigc++/sigc++.h>
#include <avahi-client/lookup.h>
#include <iostream>
#include <semaphore.h>

#include <service_discovery/service.h>

//typedef sigc::slot<void,
//	RemoteService*,
//	AvahiResolverEvent,
//	AvahiLookupResultFlags,
//	void*> AvahiRemoteServiceSlot;

namespace servicediscovery {

/**
 * @brief
 * class for handling service that are found and resolved by the service browsers
 */
class RemoteService: public Service {
protected:
	/**
	 * a service browser instance connected to this remote service
	 */
	ServiceBrowser *browser;
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
	//sigc::signal<void, RemoteService> *RemoteServiceSignal;

	/**
	 * semaphore for the RemoteServiceSignal object
	 */
	sem_t RMS_sem;
	 
public:
        //the data used in the service browser. pointer also used here so that the memory is freed along with the service resolver
        ResolveData* resolveData;

        RemoteService(
                ServiceBrowser *browser,
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
        //        sigc::signal<void, RemoteService> *rms
        );

        virtual ~RemoteService();

        /*
        bool serviceSignalConnect(const sigc::slot<void, RemoteService>& slot_) {
            if (!RemoteServiceSignal) {
                return false;
            }
            sem_wait(&RMS_sem);
            RemoteServiceSignal->connect(slot_);
            sem_post(&RMS_sem);		
            return true;
        }

        //to be used only by the service browser. TODO: avoid public use
        void emitSignal() {
            if (RemoteServiceSignal) {
                sem_wait(&RMS_sem);
                RemoteServiceSignal->emit(*this);
                sem_post(&RMS_sem);
            }
        }
        //to be used only by the service browser. TODO: avoid public use
        void freeSignal() {
            if (RemoteServiceSignal) {
                delete RemoteServiceSignal;
            }
        }
        */

        AvahiAddress getAddress() const
        {
            return address;
        }

        ServiceBrowser *getBrowser() const
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

        /**
        * Free the associated service resolver and resolver data
        */
        void freeServiceResolver();

        std::string getAddressString();

        bool operator==(RemoteService serv);
};

} // end namespace servicediscovery

#include <service_discovery/service_browser.h>

#endif /* _SERVICE_DISCOVERYREMOTESERVICE_H_ */
