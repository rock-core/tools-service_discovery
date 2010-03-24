/*
 * afServiceBrowser.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFSERVICEBROWSER_H_
#define AFSERVICEBROWSER_H_

class afServiceBrowser;

#include <sigc++/sigc++.h>
#include <avahi-client/client.h>
#include "afRemoteService.h"
#include "afAvahiClient.h"
#include "afList.h"

class afServiceBrowser {
private:
	afAvahiClient* client;
	std::string type;
	AvahiIfIndex interface;
	AvahiProtocol protocol;
	std::string domain;
	AvahiLookupFlags flags;
	void *data;

	AvahiServiceBrowser *browser;

	void bootstrap();

	afList<afRemoteService *> services;

	const char* getInType() {
		return type.c_str();
	}

	const char* getInDomain() {
		if (domain == "") {
			return NULL; //if empty string then the default value NULL (.local) domain is used
		} else {
			return domain.c_str();
		}
	}

public:
	afServiceBrowser(
			afAvahiClient *client,
			std::string type);
	afServiceBrowser(
			afAvahiClient *client,
			AvahiIfIndex interface, /**< In most cases pass AVAHI_IF_UNSPEC here */
			AvahiProtocol protocol, /**< In most cases pass AVAHI_PROTO_UNSPEC here */
			std::string type,  /**< A service type such as "_http._tcp" */
			std::string domain, /**< A domain to browse in. In most cases you want to pass NULL here for the default domain (usually ".local") */
			AvahiLookupFlags flags,
			void*);
	virtual ~afServiceBrowser();

	sigc::signal<void,
		afServiceBrowser*,
	    AvahiBrowserEvent,
		afRemoteService*,
		void*> afServiceBrowserSignal;

	sigc::signal<void, afRemoteService*> afServiceAdded;

//	sigc::signal<void,
//		afRemoteService*> afServiceRemoved;

    afList<afRemoteService*> *getServices()
    {
        return &services;
    }

    AvahiServiceBrowser* getAvahiBrowser()
	{
    	return browser;
	}

    afAvahiClient* getClient()
	{
    	return client;
	}


    static void browseCallback(AvahiServiceBrowser *sb, AvahiIfIndex interface, AvahiProtocol protocol,
                                  AvahiBrowserEvent event, const char *name, const char *type, const char *domain,
                                  AvahiLookupResultFlags flags, void *userdata);

    static void resolveCallback(AvahiServiceResolver *sr, AvahiIfIndex interface, AvahiProtocol protocol,
                            AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host,
                            const AvahiAddress *address, uint16_t port, AvahiStringList *list, AvahiLookupResultFlags flags, void *userdata);

};

#endif /* AFSERVICEBROWSER_H_ */
