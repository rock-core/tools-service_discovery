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

	protected:
		std::string IOR;

		void extractIOR(std::list<std::string> &list);
		void insertIOR(std::string IOR, std::list<std::string> &list);

	public:
		
		OrocosComponentServiceBase() : IOR("") {}
	
		virtual ~OrocosComponentServiceBase() {}
	
		std::string getIOR() {return IOR;}
		
		bool operator==(OrocosComponentServiceBase);
	
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
	) : afLocalService(client, name, type ,port, list, ttl, false) {

		//insert IOR into the string list
		insertIOR(IOR, stringlist);
		
		//update the avahi string list
		AvahiStringList* ntxt = getTxt(stringlist);
		setTxt(ntxt);
		
		if (publish) {
			this->publish();
		}
		
	}
	virtual ~OrocosComponentLocalService() {}
	
};

class OrocosComponentRemoteService : public OrocosComponentServiceBase , public afRemoteService {


	public:
	OrocosComponentRemoteService(afRemoteService &serv) :  afRemoteService(serv) {
		
		
		extractIOR(stringlist);

		AvahiStringList* ntxt = getTxt(stringlist);
		setTxt(ntxt);
		
		
	}
	virtual ~OrocosComponentRemoteService() {}

	bool operator==(const OrocosComponentRemoteService&);
	
};
	
}}

#endif /* __OROCOS_COMPONENT_SERVICE_H__ */

