/*
 * service_base.cpp
 *
 *  Created on: Mar 19, 2010
 *      Author: darko
 */

#include "service_base.h"

namespace servicediscovery {

ServiceBase::ServiceBase(
			Client *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain
			) : client_(0), configuration_() {

	client_ = client;
	configuration_.setInterfaceIndex(interf);
	configuration_.setProtocol(prot);
	configuration_.setName(name);
	configuration_.setType(type);
	configuration_.setDomain(domain);
}


ServiceBase::~ServiceBase() {
	// TODO Auto-generated destructor stub
}

bool ServiceBase::operator==(const ServiceBase& comp) {
        // Compare services based on their configuration
        return configuration_ == comp.getConfiguration();
}


void ServiceBase::setConfiguration(const ServiceConfiguration& config)
{
    configuration_ = config;
}

ServiceConfiguration ServiceBase::getConfiguration() const
{
    return configuration_;
}

Client* ServiceBase::getClient()
{
    assert(client_);
    return client_;
}

} // end namespace servicediscovery
