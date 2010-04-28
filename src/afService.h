/*
 * afService.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFSERVICE_H_
#define AFSERVICE_H_

#include <avahi-client/lookup.h>
#include <string>
#include <list>
#include <iostream>

namespace dfki { namespace communication {

class afService;
class afAvahiClient;

}}

#include "afServiceBase.h"


namespace dfki {
namespace communication {

class afService : public afServiceBase {
protected:
	/**
	 * port of the service
	 */
	uint16_t port;
	/** private avahistringlist instance */
	AvahiStringList *txt;
	/** the list of string correspondent of avahistringlist */
	std::list<std::string> stringlist;
	
	static AvahiStringList* getTxt(std::list<std::string>);
	
	void setTxt(AvahiStringList* ntxt) {
		if (txt) {
			avahi_string_list_free(txt);
		}
		txt = ntxt;
	}
	
	
public:

	bool dontCheckTXT;

	afService(const afService&);

	afService(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain,
			uint16_t port,
			std::list<std::string> list
			);
	virtual ~afService();

	bool operator==(const afService&);

    uint16_t getPort() const
    {
        return port;
    }

    AvahiStringList *getTxt() const
    {
        return txt;
    }

    std::list<std::string> getStringList() const
	{
    	return stringlist;
	}

};

}
}

#endif /* AFSERVICE_H_ */
