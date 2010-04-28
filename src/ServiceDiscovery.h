#ifndef RIMRES_SERVICEDISCOVERY_CORE_H_
#define RIMRES_SERVICEDISCOVERY_CORE_H_

#include <string>
#include "afAvahiClient.h"
#include "afServiceBrowser.h"
#include "OrocosComponentService.h"

namespace dfki { namespace communication {


enum SDException {
	already_started,
	not_configured
};

/**
 * @brief
 * A wrapper class for the framework
 */
class ServiceDiscovery
{

public: 
	ServiceDiscovery();
	~ServiceDiscovery();

	struct Configuration // : default configuration here
	{
	
	
		Configuration(std::string IOR, std::string name, std::string avahi_type) : avahi_port(12000), ttl(0) {
			this->IOR = IOR;
			this->name = name;
			this->avahi_type = avahi_type;
		}
		
		std::string IOR;
		std::string name;

		std::list<std::string> stringlist;
		
		uint32_t ttl;
		
		//should these be constants?
		std::string avahi_type;
		uint16_t avahi_port; 

	};

	void configure(const struct Configuration& configuration);

	void start();

	void stop();
	
	// return ior or alternate identifier
	OrocosComponentRemoteService* findService(std::string pattern);

private:

	bool started;

	afAvahiClient* client;
	afServiceBrowser* browser;
	OrocosComponentLocalService* localserv;

	bool configured;
	struct Configuration conf;

};

}}

#endif
