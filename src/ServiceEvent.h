#ifndef __SERVICEDISCOVERY_SERVICEEVENT_H_
#define __SERVICEDISCOVERY_SERVICEEVENT_H_

#include "afRemoteService.h"
#include "ServiceDescription.h"

namespace dfki { namespace communication {

class ServiceEvent 
{
	ServiceDescription serviceDescription_;

	public:
		ServiceEvent(const afRemoteService& service);
		
		ServiceEvent(const ServiceDescription& description);

		virtual ~ServiceEvent();

		const ServiceDescription& getServiceDescription() const;

		bool operator==(const ServiceEvent&) const;

};

}}
#endif // __SERVICEDISCOVERY_SERVICEEVENT_H_
