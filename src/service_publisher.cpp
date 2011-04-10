#include "service_publisher.h"

namespace servicediscovery { 

ServicePublisher::ServicePublisher(Client* client, const ServiceConfiguration& sc) : LocalService(client, sc.getName(), sc.getType(), (uint16_t) sc.getPort(), sc.getServiceDescription().getRawDescriptions(), (uint32_t) sc.getTTL(), sc.getPublishMode())
{

}

ServicePublisher::~ServicePublisher()
{

}

const ServiceConfiguration& ServicePublisher::getServiceConfiguration() const
{
	return serviceConfiguration_;
}

bool ServicePublisher::operator==(const ServicePublisher& other)
{
	return serviceConfiguration_.getServiceDescription() == other.getServiceConfiguration().getServiceDescription();
}


} // end namespace servicediscovery

