#ifndef __SERVICEDISCOVERY_SERVICEPUBLISHER_H_
#define __SERVICEDISCOVERY_SERVICEPUBLISHER_H_

#include <stdio.h>
#include <vector>

#include "afLocalService.h"
#include "ServiceConfiguration.h"

namespace dfki { namespace communication {

class ServicePublisher : public afLocalService
{

	ServiceConfiguration serviceConfiguration_;

protected: 
	const ServiceConfiguration& getServiceConfiguration() const;

public:
	ServicePublisher(afAvahiClient* client, const ServiceConfiguration& sc);

	~ServicePublisher();

	bool operator==(const ServicePublisher&);


};

}}
#endif

