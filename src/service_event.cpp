#include "service_event.h"

namespace servicediscovery {

ServiceEvent::ServiceEvent(const RemoteService& service)
{
	serviceDescription_ = service.getConfiguration();
}

ServiceEvent::ServiceEvent(const ServiceDescription& description)
{
	serviceDescription_ = description;
}

ServiceEvent::~ServiceEvent()
{
}

const ServiceDescription& ServiceEvent::getServiceDescription() const
{
	return serviceDescription_;
}

bool ServiceEvent::operator==(const ServiceEvent& other) const
{
	if( this->getServiceDescription() == other.getServiceDescription())
		return true;
	
	else false;
}



}
