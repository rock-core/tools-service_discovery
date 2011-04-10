#ifndef __SERVICE_DISCOVERY_SERVICEPUBLISHER_H_
#define __SERVICE_DISCOVERY_SERVICEPUBLISHER_H_

#include <stdio.h>
#include <vector>

#include <service_discovery/local_service.h>
 
namespace servicediscovery {

class ServicePublisher : public LocalService
{

	ServiceConfiguration serviceConfiguration_;

protected: 
	const ServiceConfiguration& getServiceConfiguration() const;

public:
	ServicePublisher(Client* client, const ServiceConfiguration& sc);

	~ServicePublisher();

	bool operator==(const ServicePublisher&);


};

}
#endif

