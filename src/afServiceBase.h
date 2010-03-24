/*
 * afServiceBase.h
 *
 *  Created on: Mar 19, 2010
 *      Author: darko
 */

#ifndef AFSERVICEBASE_H_
#define AFSERVICEBASE_H_

#include <avahi-client/client.h>
#include <string>
#include "afAvahiClient.h"

class afServiceBase {
public:
private:
	afAvahiClient *client;
	AvahiIfIndex interface;
	AvahiProtocol protocol;
	std::string name;
	std::string type;
	std::string domain;
public:

	afServiceBase(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain
			);
	virtual ~afServiceBase();

	bool operator==(afServiceBase);

    std::string getDomain() const
    {
        return domain;
    }

    const char* getDomainChar()
	{
    	if (domain.compare("") == 0) {
    		return NULL;
    	} else {
    		return domain.c_str();
    	}
	}


    AvahiIfIndex getInterface() const
    {
        return interface;
    }

    std::string getName() const
    {
        return name;
    }

    AvahiProtocol getProtocol() const
    {
        return protocol;
    }

    std::string getProtocolString() const
    {
        const char *proto = avahi_proto_to_string(protocol);
        return std::string(proto);
    }

    std::string getType() const
    {
        return type;
    }

    afAvahiClient* getClient()
    {
    	return client;
    }
};

#endif /* AFSERVICEBASE_H_ */
