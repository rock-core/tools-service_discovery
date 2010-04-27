#ifndef RIMRES_SERVICEDISCOVERY_CORE_H_
#define RIMRES_SERVICEDISCOVERY_CORE_H_

#include <string>
#include "afAvahiClient.h"
#include "afServiceBrowser.h"
#include "OrocosComponentService.h"

namespace dfki { namespace communication {


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

	struct Configuration conf;

};

}}

#endif
