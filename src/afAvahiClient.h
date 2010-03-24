/*
 * afAvahiClient.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFAVAHICLIENT_H_
#define AFAVAHICLIENT_H_

#define DEFAULT_POLL afSimplePoll

class afAvahiClient;

#include <avahi-client/client.h>
#include <sigc++/sigc++.h>

#include <string>

#include "afPoll.h"
#include "afSimplePoll.h"
#include "afThreadPoll.h"
//#include "afServiceBrowser.h"


typedef sigc::slot<void,
	afAvahiClient*,
	AvahiClientState,
	void*> afAvahiClientStateChanged;

class afAvahiClient {
private:
	AvahiClient *client;
	afPoll *poll;

public:
	afAvahiClient();
	afAvahiClient(afPoll *poll, AvahiClientFlags flags);
	virtual ~afAvahiClient();

	sigc::signal<void, afAvahiClient*, AvahiClientState, void*> AvahiClientStateChanged;

//	afServiceBrowser newServiceBrowser(
//			std::string type);
//	afServiceBrowser newServiceBrowser(
//			AvahiIfIndex interface, /**< In most cases pass AVAHI_IF_UNSPEC here */
//			AvahiProtocol protocol, /**< In most cases pass AVAHI_PROTO_UNSPEC here */
//			std::string type,  /**< A service type such as "_http._tcp" */
//			std::string domain, /**< A domain to browse in. In most cases you want to pass NULL here for the default domain (usually ".local") */
//			AvahiLookupFlags flags,
//			void*);

	void dispatch();
	void stop();

	AvahiClient* getAvahiClient();

};

#endif /* AFAVAHICLIENT_H_ */
