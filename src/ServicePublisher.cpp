#include "ServicePublisher.h"

namespace dfki { namespace communication {

ServicePublisher::ServicePublisher(afAvahiClient* client, const ServiceConfiguration& sc) : afLocalService(client, sc.getName(), sc.getType(), (uint16_t) sc.getPort(), sc.getServiceDescription().getRawDescriptions(), (uint32_t) sc.getTTL(), sc.getPublishMode())
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
	if(this->getName() != other.getName())
	{
		return false;
	}
	
	if(serviceConfiguration_.getServiceDescription() != other.getServiceConfiguration().getServiceDescription())
	{
		return false;
	}

	return true;
}


}}

