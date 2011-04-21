#include "service_configuration.h"
#include <base/logging.h>

namespace servicediscovery { 

const uint16_t ServiceConfiguration::defaultPort_ = 12000; 

ServiceConfiguration::ServiceConfiguration(const std::string& name, const std::string& type) : ServiceDescription()
{
	this->setName(name);
	this->setType(type);
	this->setPort(defaultPort_);
	this->setTTL(0);
	this->setPublishMode(true);
}

ServiceConfiguration::ServiceConfiguration(const std::string& name, const std::string& type, uint16_t port) : ServiceDescription()
{
	this->setName(name);
	this->setType(type);
	this->setPort(port);
	this->setTTL(0);
	this->setPublishMode(true);
}



ServiceConfiguration::ServiceConfiguration() : ServiceDescription()
{
	this->setName("undefined-service");
	this->setPort(0);
	this->setTTL(0);
	this->setPublishMode(false);


}

ServiceConfiguration::~ServiceConfiguration()
{
}

std::string ServiceConfiguration::getType() const
{
	return type_;
}

void ServiceConfiguration::setType(const std::string& type)
{
	type_ = type;
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

} // end namespace servicediscovery

