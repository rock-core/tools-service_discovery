#include "ServiceEvent.h"

namespace dfki { namespace communication {

ServiceEvent::ServiceEvent(const afRemoteService& service)
{
	serviceDescription_.setName(service.getName());
	serviceDescription_.setType(service.getType());
	serviceDescription_.setProtocol(service.getProtocolString());
	serviceDescription_.setDomain(service.getDomain());
	serviceDescription_.setRawDescriptions(service.getStringList());
	serviceDescription_.setInterfaceIndex(service.getInterface());
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
