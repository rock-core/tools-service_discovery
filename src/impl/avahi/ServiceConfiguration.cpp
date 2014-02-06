#include "ServiceConfiguration.hpp"
#include <base/Logging.hpp>

namespace servicediscovery { 
namespace avahi {

ServiceConfiguration::ServiceConfiguration(const std::string& name, const std::string& type, uint16_t port, uint16_t ttlInMS, bool publish)
    : ServiceDescription(name)
{
	this->setType(type);
	this->setPort(port);
	this->setTTL(ttlInMS);
	this->setPublishMode(publish);
}

ServiceConfiguration::ServiceConfiguration() : ServiceDescription()
{
	this->setName("undefined-service");
	this->setPort(0);
	this->setTTL(0);
	this->setPublishMode(false);
}

ServiceConfiguration::ServiceConfiguration(const ServiceDescription& description)
    : ServiceDescription(description)
{
	this->setPort(0);
        this->setTTL(0);
        this->setPublishMode(true);
}

ServiceConfiguration::~ServiceConfiguration()
{
}

uint16_t ServiceConfiguration::getPort() const
{
	return port_;
}

void ServiceConfiguration::setPort(uint16_t port)
{
	port_ = port;
}

void ServiceConfiguration::setTTL(uint16_t ttl)
{
	ttl_ = ttl;
}

uint16_t ServiceConfiguration::getTTL() const
{
	return ttl_;
}

void ServiceConfiguration::setPublishMode(bool mode)
{
	publishMode_ = mode;
}

bool ServiceConfiguration::getPublishMode() const
{
	return publishMode_;
}

std::string ServiceConfiguration::toString()
{
    char buffer[2024];
    sprintf(buffer, "Name: %s, type: %s, protocol: %s, domain: %s, interface: %d, port: %d, ttl: %d, publishmode: %d", getName().c_str(), getType().c_str(), getProtocolString().c_str(), getDomain().c_str(), getInterfaceIndex(), getPort(), getTTL(), getPublishMode());
    return std::string(buffer);
}

} // end namespace avahi
} // end namespace servicediscovery
