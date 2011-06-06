/*
 * service_base.h
 *
 *  Created on: Mar 19, 2010
 *      Author: darko
 */

#ifndef _SERVICE_DISCOVERY_SERVICEBASE_H_
#define _SERVICE_DISCOVERY_SERVICEBASE_H_


#include <avahi-client/client.h>
#include <string>
#include <iostream>
#include <service_discovery/client.h>
#include <service_discovery/service_configuration.h>

namespace servicediscovery {

/**
 * @brief
 * an abstract base service class used to represent the basic info of a service as received from a service browser callback
 */
class ServiceBase {

private:
	/**
	 * related avahi client instance
	 */
	Client *client_;
        ServiceConfiguration configuration_;

public:
	ServiceBase(
			Client *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain
			);

        std::string getName() const
        {
                return configuration_.getName();
        }

	virtual ~ServiceBase();

	bool operator==(const ServiceBase&);

        void setConfiguration(const ServiceConfiguration& config);

        ServiceConfiguration getConfiguration() const;

        Client* getClient();
};

} // end namespace servicediscovery

#endif /* _SERVICE_DISCOVERY_SERVICEBASE_H_ */
