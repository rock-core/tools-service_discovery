#ifndef RIMRES_SERVICEDISCOVERY_CORE_H_
#define RIMRES_SERVICEDISCOVERY_CORE_H_

namespace dfki { namespace communication {

class ServiceDiscovery
{

public: 
	ServiceDiscovery();
	~ServiceDiscovery();

	struct Configuration // : default configuration here
	{
		// required configuration properties
		// NAME
		// AVAHI_TYPE
		// AVAHI_PORT
		// pointer to callback function 
		
		
	};

	void configure(const struct Configuration& configuration);

	void start();

	void stop();

};


}}

#endif
