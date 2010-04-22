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
		// IOR
		// NAME
		// AVAHI_TYPE
		// AVAHI_PORT
	};

	void configure(const struct Configuration& configuration);

	void start();

	void stop();
	
	// return ior or alternate identifier
	// std::string findService(SearchPattern searchPattern);

	// the callback function will be used to store the entries, so this will be handled within this library. Just account for that 
	// std::map<Service, ServiceDescription> 

};


}}

#endif
