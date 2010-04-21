/*
 * afServiceBrowser.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afServiceBrowser.h"
#include "afService.h"
#include <iostream>
#include <map>
#include <list>
#include <string>

using namespace std;

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
		cerr << "Failed to create avahi service browser: " << avahi_strerror(avahi_client_errno(client->getAvahiClient()));
		throw 0; //TODO improve this
	}
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

afServiceBrowser::~afServiceBrowser() {
	if (browser)
		avahi_service_browser_free(browser);
	list<afRemoteService*>::iterator it;
	//free service resolvers
	for (it = services.begin() ; it != services.end(); ++it) {
		if (*it) {
			AvahiServiceResolver *sr;
			sr = (*it)->getServiceResolver();
			if (sr)
				avahi_service_resolver_free(sr);
			delete (*it);
		}
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
    afServiceBrowser *sb = (afServiceBrowser*) userdata;
    switch(event)
    {
        // if found service cannot be resolved, throw an error
        case AVAHI_RESOLVER_FAILURE:
            cerr << "RESOLVER failed to resolve service" << name << " of type " << type << " in domain " << domain << endl;
            cerr << "Error: " << avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(sr))) << endl;
            //TODO either free the resolver immediately (as it is now) or maybe wait more to see if the service will be resolved
            avahi_service_resolver_free(sr);
            break;

        // if found service can be resolved
        case AVAHI_RESOLVER_FOUND:

			string sname(name);
			string stype(type);
			string sdomain(domain);
			string shost(host);

			//create the additional info string list
			list<string> strlist;
			AvahiStringList *tmp = txt;
			while (tmp != NULL) {
				strlist.push_back((const char*) tmp->text);
				tmp = tmp->next;
			}


            afRemoteService *rms = new afRemoteService(sb, interface, protocol, sname, stype, sdomain, strlist, port, shost, *address, sr);
			cout << "SERVICE RESOLVED: " << rms->getInterface() << " " << rms->getProtocol() << " " << rms->getName() << " " << rms->getType() << " " << rms->getDomain() << " ; ";
			
			
            if (!sb->getServices()->find(*rms)) {

				rms->dontCheckTXT = true;
				afRemoteService *srv;
				if (srv = sb->getServices()->find(*rms)) {
	
					cout << "SERVICE ALREADY IN DB, BUT TXT RECORD HAS CHANGED\n";
					srv->afRemoteServiceSignal.emit(srv);
					
				} else {
					
					rms->dontCheckTXT = false;
		        	sb->getServices()->push_back(rms);
		            cout << "SERVICE ADDED" << endl;
	            	sb->afServiceAdded.emit(rms);
				}

            } else {
	            cout << "SERVICE NOT ADDED (ALREADY IN DB)" << endl;
            }

//			printls(*(sb->getServices()));


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
            cerr << "BROWSER failed. Error: " << avahi_strerror(avahi_client_errno(client));
            break;

        // if browser finds a new service
        case AVAHI_BROWSER_NEW:
        	cout << "NEW SERVICE FOUND" << endl;
//            cerr << "BROWSER found new service:" << name << "of type:" << type << "in domain:" << domain;
            // try to resolve the new found service. If resolver object cannot be created, throw an error
            if (!(avahi_service_resolver_new(client, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, AvahiLookupFlags (0), resolveCallback, userdata)))
            {
                cerr << "Failed to resolve service:" << name << ". Error: " << avahi_strerror(avahi_client_errno(client));
            }
            break;

        // if browser finds a service removal event
        case AVAHI_BROWSER_REMOVE:
			cout << "REMOVING SERVICE: " << interface << " " << name << " " << type << " " << domain << " ...   ";
        	{
				bool removed = false;
				string sname(name);
				string stype(type);
				string sdomain(domain);
				afServiceBase serv(asb->getClient(), interface, protocol, sname, stype, sdomain);
				list<afRemoteService*>::iterator it;
				//find the signal to be removed
				for (it = asb->getServices()->begin() ; it != asb->getServices()->end(); ++it) {
					if ((afServiceBase) (**it) == serv) {
						AvahiServiceResolver *sr;
						sr = (*it)->getServiceResolver();
						if (sr)
							avahi_service_resolver_free(sr);
						
						//emit the service removed signal
						asb->afServiceRemoved.emit((*(*it)));
						
						delete (*it);
						asb->getServices()->erase(it);
						removed = true;
						break;
					}
				}
				cout <<( (removed) ? "SERVICE REMOVED" : "SERVICE NOT REMOVED") << endl;
        	}

            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        	cout << "ALL_FOR_NOW" << endl;
//            qDebug() << "ALL_FOR_NOW";
            break;

        case AVAHI_BROWSER_CACHE_EXHAUSTED:
        	cout << "CACHE_EXHAUSTED" << endl;
//            qDebug() << "CACHE_EXCHAUSTED";
            break;
    }
}





