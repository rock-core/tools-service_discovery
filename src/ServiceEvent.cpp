#include "ServiceEvent.h"

namespace dfki { namespace communication {

ServiceEvent::ServiceEvent(const afRemoteService& service)
{
	serviceDescription_.setName(service.getName());
	serviceDescription_.setRawDescriptions(service.getStringList());
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



}}
