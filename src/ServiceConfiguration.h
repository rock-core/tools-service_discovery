#ifndef __SERVICEDISCOVERY_SERVICECONFIGURATION_H_
#define __SERVICEDISCOVERY_SERVICECONFIGURATION_H_

#include <stdint.h>
#include <cmath>
#include <string>
#include <vector>
#include "ServiceDescription.h"

namespace dfki { namespace communication {

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
	friend class ServicePublisher;
	friend class ServiceDiscovery;

	private:
		// Avahi type
		std::string type_;

		static const uint16_t defaultPort_;

		uint16_t port_;
		uint16_t ttl_;
		bool publishMode_;
		
	protected: 
		

	public:
		ServiceConfiguration();	

		ServiceConfiguration(const std::string& name, const std::string& type, uint16_t port);

		ServiceConfiguration(const std::string& name, const std::string& type);

		virtual ~ServiceConfiguration();

		std::string getType() const;
		
		void setType(const std::string& type);
		/**
		* Get all raw description fields
		*/
		ServiceDescription getServiceDescription() const;

		uint16_t getPort() const;
		
		void setPort(uint16_t port);	

		void setTTL(uint16_t ttl);

		uint16_t getTTL() const;

		void setPublishMode(bool mode);

		bool getPublishMode() const;

};


}}

#endif // __SERVICEDISCOVERY_SERVICECONFIGURATION_H_



