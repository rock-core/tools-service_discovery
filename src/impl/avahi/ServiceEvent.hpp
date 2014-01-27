#ifndef __SERVICEDISCOVERY_SERVICEEVENT_H_
#define __SERVICEDISCOVERY_SERVICEEVENT_H_

#include <service_discovery/impl/avahi/RemoteService.hpp>

namespace servicediscovery { 
namespace avahi {

/**
* \class ServiceEvent
* \brief Event associated with appearance or disappearance of a service
*/
class ServiceEvent 
{
	ServiceConfiguration serviceConfiguration_;

	public:
		/**
		 * Constructor for a service event 
		 * \param service remote service object
                 */
		ServiceEvent(const RemoteService& service);
		
		/**
                 * Constructor for a service event 
		 * \param description ServiceConfig
                 */
		ServiceEvent(const ServiceConfiguration& config);

		virtual ~ServiceEvent();

		/**
		 * Retrieve service description from service event
		 */
		const ServiceConfiguration& getServiceConfiguration() const;

		bool operator==(const ServiceEvent&) const;

};

} // end namespace avahi
} // end namespace servicediscovery
#endif // __SERVICEDISCOVERY_SERVICEEVENT_H_
