#ifndef __OROCOS_COMPONENT_SERVICE_H__
#define __OROCOS_COMPONENT_SERVICE_H__

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <sstream>

#include "afService.h"
#include "afLocalService.h"
#include "afRemoteService.h"

namespace dfki { namespace communication {

enum OCSException {
	ior_incomplete,
	ior_not_in_list
};

class OrocosComponentServiceBase {

	private:
		std::string IOR;

	public:
	
		OrocosComponentServiceBase(std::list<std::string> &list);
		OrocosComponentServiceBase(std::string IOR, std::list<std::string> &list);
	
		virtual ~OrocosComponentServiceBase() {}
	
		std::string getIOR() {return IOR;}
	
};	

class OrocosComponentLocalService : public OrocosComponentServiceBase , public afLocalService{
	
	public:	
	OrocosComponentLocalService(
			afAvahiClient *client,
			std::string name,
			std::string type,
			uint16_t port,
			std::string IOR,
			std::list<std::string> list,
			uint32_t ttl=0,
			bool publish=true
	) : OrocosComponentServiceBase(IOR, list), afLocalService(client, name, type ,port, list, ttl, publish) {}
	virtual ~OrocosComponentLocalService() {}
	
};

class OrocosComponentRemoteService : public OrocosComponentServiceBase , public afRemoteService {

	public:
	OrocosComponentRemoteService(
			afServiceBrowser *browser,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain,
			std::list<std::string> list,
			uint16_t port,
			std::string host_name,
			AvahiAddress address,
			AvahiServiceResolver *sr
	) : OrocosComponentServiceBase(list), afRemoteService(browser, interf, prot, name, type, domain ,list, port, host_name, address, sr) {}
	virtual ~OrocosComponentRemoteService() {}
	
};
	
}}

#endif /* __OROCOS_COMPONENT_SERVICE_H__ */

