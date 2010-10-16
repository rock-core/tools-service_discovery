#ifndef __SERVICEDISCOVERY_SERVICEEVENT_H_
#define __SERVICEDISCOVERY_SERVICEEVENT_H_

#include "afRemoteService.h"
#include "ServiceDescription.h"

namespace dfki { namespace communication {

/**
* \class ServiceEvent
* \brief Event associated with appearance or disappearance of a service
*/
class ServiceEvent 
{
	ServiceDescription serviceDescription_;

	public:
		/**
		 * Constructor for a service event 
		 * \param service remote service object
                 */
		ServiceEvent(const afRemoteService& service);
		
		/**
                 * Constructor for a service event 
		 * \param description ServiceDescription
                 */
		ServiceEvent(const ServiceDescription& description);

		virtual ~ServiceEvent();

		/**
		 * Retrieve service description from service event
		 */
		const ServiceDescription& getServiceDescription() const;

		bool operator==(const ServiceEvent&) const;

};

}}
#endif // __SERVICEDISCOVERY_SERVICEEVENT_H_
