/*
 * afServiceBrowser.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afServiceBrowser.h"

namespace dfki { namespace communication {

static afLoggingWrapper logger("afServiceBrowser");

void afServiceBrowser::bootstrap() {
	browser = avahi_service_browser_new(
				client->getAvahiClient(),
				interface,
				protocol,
				getInType(),
				getInDomain(),
				flags,
				browseCallback,
				this);
	if (!browser) {
		logger.log(FATAL, "Failed to create avahi service browser: %s", avahi_strerror(avahi_client_errno(client->getAvahiClient())));
		throw 0; //TODO improve this
	}
	serviceResolveTryCount = 5;
}

afServiceBrowser::afServiceBrowser(afAvahiClient *client, AvahiIfIndex interface, AvahiProtocol protocol, std::string type, std::string domain, AvahiLookupFlags flags, void* data)
{
	this->client = client;
	this->interface = interface;
	this->protocol = protocol;
	this->type = type;
	this->domain = domain;
	this->flags = flags;
	this->data = data;
	this->bootstrap();
}

afServiceBrowser::afServiceBrowser(afAvahiClient *client, std::string type) {
	this->client = client;
	this->interface = AVAHI_IF_UNSPEC;
	this->protocol = AVAHI_PROTO_UNSPEC;
	this->type = type;
	this->domain = "";
	this->flags = (AvahiLookupFlags) 0;
	this->bootstrap();
}

void freeAfRemoteService(afRemoteService srv) {
	AvahiServiceResolver *sr;
	sr = srv.getServiceResolver();
	if (sr)
		avahi_service_resolver_free(sr);

	//delete the object used to count failed attempts						
	ResolveData *sdata = srv.resolveData;
	if (sdata) {
		delete sdata;
	}
	
	//remove the signal object
	srv.freeSignal();

}

afServiceBrowser::~afServiceBrowser() {
	if (browser)
		avahi_service_browser_free(browser);
	afList<afRemoteService>::iterator it;
	//free service resolvers
	for (it = services.begin() ; it != services.end(); ++it) {
		freeAfRemoteService((*it));
	}
}

//void printls(list<afRemoteService> ls) {
//	cout << "PRINTING SERVICES" << endl;
//	list<afRemoteService>::iterator it;
//	for (it = ls.begin() ; it != ls.end(); ++it) {
//		cout << (*it).getInterface() << " " << (*it).getName() << endl;
//	}
//	cout << "=================" << endl;
//}

//a callback called on a service resolver event
void afServiceBrowser::resolveCallback(AvahiServiceResolver *sr, AvahiIfIndex interface, AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, void *userdata)
{
	ResolveData *data = (ResolveData*) userdata;
    afServiceBrowser *sb = data->sb;
    switch(event)
    {
        // if found service cannot be resolved, throw an error
        case AVAHI_RESOLVER_FAILURE:
        	logger.log(WARN, "Resolver failed to resolve service %s of type %s on interface %d. Error: %s", 
        						name, 
        						type, 
        						interface,
        						avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(sr))));
            if (data->count >= sb->serviceResolveTryCount) {
            	//TODO: it seems that it doesn't make more attempts'
            	logger.log(WARN, "Failed attempts to resolve this service has exceeded the limit. Service resolver object freed");
            	delete data;
	            avahi_service_resolver_free(sr);
            } else {
            	data->count = data->count + 1;
            	logger.log(WARN, "Failed attempt %d out of %d", data->count, sb->serviceResolveTryCount);
            }
            break;

        // if found service can be resolved
        case AVAHI_RESOLVER_FOUND:

			//reset the counter for retries
			data->count = 0;

			std::string sname(name);
			std::string stype(type);
			std::string sdomain(domain);
			std::string shost(host);

			//create the additional info string list
			std::list<std::string> strlist;
			AvahiStringList *tmp = txt;
			while (tmp != NULL) {
				strlist.push_back((const char*) tmp->text);
				tmp = tmp->next;
			}


            afRemoteService rms(sb, interface, protocol, sname, stype, sdomain, strlist, port, shost, *address, sr, new sigc::signal<void, afRemoteService>());
            rms.resolveData = data;
            logger.log(INFO, "Service resolved: %d %d %s %s %s", (int)rms.getInterface(), (int)rms.getProtocol(), rms.getName().c_str(), rms.getType().c_str(), rms.getDomain().c_str());
			
            if (sb->getInternalServices()->find(rms) == sb->getInternalServices()->end()) {

				rms.dontCheckTXT = true;
				afList<afRemoteService>::iterator srv;
				srv = sb->getInternalServices()->find(rms);
				if (srv != sb->getInternalServices()->end()) {

					logger.log(INFO, "Service already in DB, but txt records have changed");
					(*srv).emitSignal();
					
				} else {
					
					rms.dontCheckTXT = false;
		        	sb->getInternalServices()->push_back(rms);
		        	logger.log(INFO, "Service added to DB. Signal sent to %d slots", sb->afServiceAdded.size());
	            	sb->afServiceAdded.emit(rms);
				}

            } else {
            	logger.log(INFO, "Service not added (already in DB)");
            }

//			printls(*(sb->getInternalServices()));


            break;

    }

}


void afServiceBrowser::browseCallback(AvahiServiceBrowser *sb, AvahiIfIndex interface, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void *userdata)
{

	AvahiClient* client = avahi_service_browser_get_client(sb);
    afServiceBrowser *asb = (afServiceBrowser*) userdata;

	switch (event)
    {
        // if browser failure occurs, throw an error
        case AVAHI_BROWSER_FAILURE:
        	logger.log(FATAL, "Browser failed. Error: %s", avahi_strerror(avahi_client_errno(client)));
            break;

        // if browser finds a new service
        case AVAHI_BROWSER_NEW:
        	logger.log(INFO, "New service detected");
        	
        	ResolveData *tdata;
        	tdata = new ResolveData();
        	tdata->sb = asb;
        	tdata->count = 0;
        	
//            cerr << "BROWSER found new service:" << name << "of type:" << type << "in domain:" << domain;
            // try to resolve the new found service. If resolver object cannot be created, throw an error
            if (!(avahi_service_resolver_new(client, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, AvahiLookupFlags (0), resolveCallback, tdata)))
            {
            	logger.log(FATAL, "Failed to create service resolver object for: %s. Error: ", name, avahi_strerror(avahi_client_errno(client)));
            }
            break;

        // if browser finds a service removal event
        case AVAHI_BROWSER_REMOVE:
        	logger.log(INFO, "Removing service: %d %s %s %s", interface, name, type, domain);
        	{
				bool removed = false;
				std::string sname(name);
				std::string stype(type);
				std::string sdomain(domain);
				afServiceBase serv(asb->getClient(), interface, protocol, sname, stype, sdomain);
				afList<afRemoteService>::iterator it;
				//find the signal to be removed
				for (it = asb->getInternalServices()->begin() ; it != asb->getInternalServices()->end(); ++it) {
					if ((afServiceBase) (*it) == serv) {
						
						//emit the service removed signal
						asb->afServiceRemoved.emit((*it));
						
						freeAfRemoteService((*it));
						
						asb->getInternalServices()->erase(it);
						removed = true;
						break;
					}
				}
				logger.log(INFO, ( (removed) ? "Service removed" : "Service not removed"));
        	}

            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        	logger.log(INFO, "ALL_FOR_NOW");
            break;

        case AVAHI_BROWSER_CACHE_EXHAUSTED:
        	logger.log(INFO, "CACHE_EXHAUSTED");
            break;
    }
}

}}


