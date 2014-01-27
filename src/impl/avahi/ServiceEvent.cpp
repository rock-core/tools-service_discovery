#include "ServiceEvent.hpp"

namespace servicediscovery {
namespace avahi {

ServiceEvent::ServiceEvent(const RemoteService& service)
{
	serviceConfiguration_ = service.getConfiguration();
}

ServiceEvent::ServiceEvent(const ServiceConfiguration& config)
{
	serviceConfiguration_ = config;
}

ServiceEvent::~ServiceEvent()
{
}

const ServiceConfiguration& ServiceEvent::getServiceConfiguration() const
{
	return serviceConfiguration_;
}

bool ServiceEvent::operator==(const ServiceEvent& other) const
{
	if( this->getServiceConfiguration() == other.getServiceConfiguration())
	{
		return true;
	} else {
		return false;
	}
}

} // end namespace avahi
} // end namespace servicediscovery
