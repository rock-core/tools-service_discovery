/*
 * afServiceBrowser.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afServiceBrowser.h"

namespace dfki { namespace communication {

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
		std::cerr << "Failed to create avahi service browser: " << avahi_strerror(avahi_client_errno(client->getAvahiClient())) << std::endl;
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
            std::cerr << "RESOLVER failed to resolve service" << name << " of type " << type << " in domain " << domain << std::endl;
            std::cerr << "Error: " << avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(sr))) << std::endl;
            if (data->count >= sb->serviceResolveTryCount) {
            	std::cerr << "Failed attempts to resolve this service has exceeded the limit. Service resolver object freed.\n";
            	delete data;
	            avahi_service_resolver_free(sr);
            } else {
            	data->count = data->count + 1;
            	std::cerr << "Failed attempt " << data->count << " out of " << sb->serviceResolveTryCount << std::endl;
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
			std::cout << "SERVICE RESOLVED: " << rms.getInterface() << " " << rms.getProtocol() << " " << rms.getName() << " " << rms.getType() << " " << rms.getDomain() << " ; ";
			
			
            if (sb->getInternalServices()->find(rms) == sb->getInternalServices()->end()) {

				rms.dontCheckTXT = true;
				afList<afRemoteService>::iterator srv;
				srv = sb->getInternalServices()->find(rms);
				if (srv != sb->getInternalServices()->end()) {
	
					std::cout << "SERVICE ALREADY IN DB, BUT TXT RECORD HAS CHANGED\n";
					(*srv).emitSignal();
					
				} else {
					
					rms.dontCheckTXT = false;
		        	sb->getInternalServices()->push_back(rms);
		            std::cout << "SERVICE ADDED: SIGNAL TO " << sb->afServiceAdded.size() << " SLOTS" << std::endl;
	            	sb->afServiceAdded.emit(rms);
				}

            } else {
	            std::cout << "SERVICE NOT ADDED (ALREADY IN DB)" << std::endl;
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
            std::cerr << "BROWSER failed. Error: " << avahi_strerror(avahi_client_errno(client)) << std::endl;
            break;

        // if browser finds a new service
        case AVAHI_BROWSER_NEW:
        	std::cout << "NEW SERVICE FOUND" << std::endl;
        	
        	ResolveData *tdata;
        	tdata = new ResolveData();
        	tdata->sb = asb;
        	tdata->count = 0;
        	
//            cerr << "BROWSER found new service:" << name << "of type:" << type << "in domain:" << domain;
            // try to resolve the new found service. If resolver object cannot be created, throw an error
            if (!(avahi_service_resolver_new(client, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, AvahiLookupFlags (0), resolveCallback, tdata)))
            {
                std::cerr << "Failed to resolve service:" << name << ". Error: " << avahi_strerror(avahi_client_errno(client)) << std::endl;
            }
            break;

        // if browser finds a service removal event
        case AVAHI_BROWSER_REMOVE:
			std::cout << "REMOVING SERVICE: " << interface << " " << name << " " << type << " " << domain << " ...   ";
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
				std::cout <<( (removed) ? "SERVICE REMOVED" : "SERVICE NOT REMOVED") << std::endl;
        	}

            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        	std::cout << "ALL_FOR_NOW" << std::endl;
//            qDebug() << "ALL_FOR_NOW";
            break;

        case AVAHI_BROWSER_CACHE_EXHAUSTED:
        	std::cout << "CACHE_EXHAUSTED" << std::endl;
//            qDebug() << "CACHE_EXCHAUSTED";
            break;
    }
}

}}


