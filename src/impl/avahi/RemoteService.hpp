#ifndef _SERVICE_DISCOVERYREMOTESERVICE_H_
#define _SERVICE_DISCOVERYREMOTESERVICE_H_

namespace servicediscovery {
namespace avahi {

class RemoteService;
class ServiceBrowser;
struct ResolveData;

}}

#include <sigc++/sigc++.h>
#include <avahi-client/lookup.h>
#include <iostream>
#include <service_discovery/impl/avahi/Service.hpp>

//typedef sigc::slot<void,
//	RemoteService*,
//	AvahiResolverEvent,
//	AvahiLookupResultFlags,
//	void*> AvahiRemoteServiceSlot;

namespace servicediscovery {
namespace avahi {

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
        );

        virtual ~RemoteService();

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

} // end namespace avahi
} // end namespace servicediscovery
#endif /* _SERVICE_DISCOVERYREMOTESERVICE_H_ */
