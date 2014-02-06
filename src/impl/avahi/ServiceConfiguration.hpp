#ifndef _SERVICE_DISCOVERY_SERVICECONFIGURATION_H_
#define _SERVICE_DISCOVERY_SERVICECONFIGURATION_H_

#include <stdint.h>
#include <cmath>
#include <string>
#include <vector>
#include <service_discovery/impl/avahi/ServiceDescription.hpp>

namespace servicediscovery { 
namespace avahi {

class ServicePublisher;
class ServiceDiscovery;

/**
* @class ServiceConfiguration
* @brief Description and parameterization of a service
*
*/
class ServiceConfiguration : public ServiceDescription
{
	// To access the normal constructor
	friend class ServiceDiscovery;

	private:
		uint16_t port_;
		uint16_t ttl_;
		bool publishMode_;
		
	public:
		ServiceConfiguration();	

                /**
                 * \param name Name of service
                 * \param type Type of service such as _foo_type._tcp
                 * \param port Communication port for avahi
                 * \param ttlInMS Time-to-live for records
                 * \param publish Flag to control the publishing mode, if service shall be published set to true
                 */
		ServiceConfiguration(const std::string& name, const std::string& type, uint16_t port = 12000, uint16_t ttlInMS = 0, bool publish = true);

		ServiceConfiguration(const ServiceDescription& description);

		virtual ~ServiceConfiguration();

		uint16_t getPort() const;
		
		void setPort(uint16_t port);	

		void setTTL(uint16_t ttl);

		uint16_t getTTL() const;

		void setPublishMode(bool mode);

		bool getPublishMode() const;
                
                /**
                * General description of the service
                */
                std::string toString();
};


} // end namespace avahi
} // end namespace servicediscovery
#endif // _SERVICE_DISCOVERY_SERVICECONFIGURATION_H_
