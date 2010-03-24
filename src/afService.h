/*
 * afService.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFSERVICE_H_
#define AFSERVICE_H_

class afService;
class afAvahiClient;

#include <avahi-client/lookup.h>
#include <string>
#include <list>

#include "afServiceBase.h"

class afService : public afServiceBase {
private:
	uint16_t port;
	AvahiStringList *txt;
	std::list<std::string> stringlist;
public:

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

	bool operator==(afService);

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

#endif /* AFSERVICE_H_ */
