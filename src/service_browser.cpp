/*
 * service_browser.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "service_browser.h"
#include "client.h"

namespace servicediscovery {

void ServiceBrowser::bootstrap() {
        client->lock();
	browser = avahi_service_browser_new(
				client->getAvahiClient(),
				interface,
				protocol,
				getInType(),
				getInDomain(),
				flags,
				browseCallback,
				this);
        client->unlock();

	if (!browser) {
		LOG_FATAL("Failed to create avahi service browser: %s", avahi_strerror(avahi_client_errno(client->getAvahiClient())));
		throw 0; //TODO improve this
	}
	if (
			sem_init(&services_sem,0,1) == -1
				||
			sem_init(&service_added_sem,0,1) == -1
				||
			sem_init(&service_removed_sem,0,1) == -1
                                ||
                        sem_init(&service_updated_sem, 0, 1) == -1
		) {
		LOG_FATAL("Semaphore initialization failed");
		throw 1;
	}

	serviceResolveTryCount = 5;
}

ServiceBrowser::ServiceBrowser(Client *client, AvahiIfIndex interface, AvahiProtocol protocol, std::string type, std::string domain, AvahiLookupFlags flags, void* data)
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

ServiceBrowser::ServiceBrowser(Client *client, std::string type) {
	this->client = client;
	this->interface = AVAHI_IF_UNSPEC;
	this->protocol = AVAHI_PROTO_UNSPEC;
	this->type = type;
	this->domain = "";
	this->flags = (AvahiLookupFlags) 0;
	this->bootstrap();
}

void freeAfRemoteService(RemoteService srv) {
	AvahiServiceResolver *sr;
	sr = srv.getServiceResolver();
	if (sr)
        {
		avahi_service_resolver_free(sr);
                sr = NULL;
        }

	//delete the object used to count failed attempts						
	ResolveData *sdata = srv.resolveData;
	if (sdata) {
		delete sdata;
	}
	
	//remove the signal object
	//srv.freeSignal();
}

ServiceBrowser::~ServiceBrowser() {
        client->lock();
	List<RemoteService>::iterator it;
	//free service resolvers
	sem_wait(&services_sem);
	for (it = services.begin() ; it != services.end(); ++it) {
		freeAfRemoteService((*it));
	}
	sem_post(&services_sem);

	if (browser)
        {       
		avahi_service_browser_free(browser);
                browser = NULL;
        }
        client->unlock();
}

//void printls(list<RemoteService> ls) {
//	cout << "PRINTING SERVICES" << endl;
//	list<RemoteService>::iterator it;
//	for (it = ls.begin() ; it != ls.end(); ++it) {
//		cout << (*it).getInterface() << " " << (*it).getName() << endl;
//	}
//	cout << "=================" << endl;
//}

//a callback called on a service resolver event
void ServiceBrowser::resolveCallback(AvahiServiceResolver *sr, AvahiIfIndex interface, AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, void *userdata)
{
    ResolveData *data = (ResolveData*) userdata;
    ServiceBrowser *sb = data->sb;
    switch(event)
    {
        // if found service cannot be resolved, throw an error
        case AVAHI_RESOLVER_FAILURE:
        	LOG_WARN("Resolver failed to resolve service %s of type %s on interface %d. Error: %s", 
        						name, 
        						type, 
        						interface,
        						avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(sr))));
            if (data->count >= sb->serviceResolveTryCount) {
            	//TODO: it seems that it doesn't make more attempts'
            	LOG_WARN("Failed attempts to resolve this service has exceeded the limit. Service resolver object freed");
            	delete data;
	            avahi_service_resolver_free(sr);
            } else {
            	data->count = data->count + 1;
            	LOG_WARN("Failed attempt %d out of %d", data->count, sb->serviceResolveTryCount);
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


            RemoteService rms(sb, interface, protocol, sname, stype, sdomain, strlist, port, shost, *address, sr);
            rms.resolveData = data;
            ServiceConfiguration remoteConfig = rms.getConfiguration();
            

            LOG_INFO("Service resolved: %d %d %s %s %s", remoteConfig.getInterfaceIndex(), remoteConfig.getProtocol(), remoteConfig.getName().c_str(), remoteConfig.getType().c_str(), remoteConfig.getDomain().c_str());
			
			sem_wait(sb->getServicesSem());
            if (sb->getInternalServices()->find(rms) == sb->getInternalServices()->end()) {
				
				
				rms.dontCheckTXT = true;
				List<RemoteService>::iterator srv;
				srv = sb->getInternalServices()->find(rms);
				if (srv != sb->getInternalServices()->end()) {
					LOG_INFO("Service already in DB, but txt records have changed");
					sb->serviceUpdatedEmit(rms);
					
				} else {
					
					rms.dontCheckTXT = false;
		        	sb->getInternalServices()->push_back(rms);

		        	LOG_INFO("Service added to DB. Signal sent to %d slots", sb->ServiceAdded.size());
	            	        sb->serviceAddedEmit(rms);
		
				}

            } else {

            	LOG_INFO("Service not added (already in DB)");
            }
			sem_post(sb->getServicesSem());

//			printls(*(sb->getInternalServices()));


            break;

    }

}


void ServiceBrowser::browseCallback(AvahiServiceBrowser *sb, AvahiIfIndex interface, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void *userdata)
{
    AvahiClient* client = avahi_service_browser_get_client(sb);
    ServiceBrowser *asb = (ServiceBrowser*) userdata;

	switch (event)
    {
        // if browser failure occurs, throw an error
        case AVAHI_BROWSER_FAILURE:
        	LOG_FATAL("Browser failed. Error: %s", avahi_strerror(avahi_client_errno(client)));
            break;

        // if browser finds a new service
        // since avahi annouces on various interface such as eth0, wlan0, pan0
        // a service event for every individual service is thrown
        case AVAHI_BROWSER_NEW:
        	LOG_INFO("New service %s of type %s detected in domain %s: interface #%d ", name, type, domain, interface);
        	
        	ResolveData *tdata;
        	tdata = new ResolveData();
        	tdata->sb = asb;
        	tdata->count = 0;
        	
            // try to resolve the new found service. If resolver object cannot be created, throw an error
            if (!(avahi_service_resolver_new(client, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, AvahiLookupFlags (0), resolveCallback, tdata)))
            {
            	LOG_FATAL("Failed to create service resolver object for: %s. Error: ", name, avahi_strerror(avahi_client_errno(client)));
            }
            break;

        // if browser finds a service removal event
        case AVAHI_BROWSER_REMOVE:
        	LOG_INFO("Trying to remove service: %d %s %s %s", interface, name, type, domain);
        	{
				bool removed = false;
				std::string sname(name);
				std::string stype(type);
				std::string sdomain(domain);
				Service serv(asb->getClient(), interface, protocol, sname, stype, sdomain);
				List<RemoteService>::iterator it;
				//find the signal to be removed
				sem_wait(asb->getServicesSem());
				for (it = asb->getInternalServices()->begin() ; it != asb->getInternalServices()->end(); ++it) {
					if (it->getConfiguration() == serv.getConfiguration()) {
						
						//emit the service removed signal
						asb->serviceRemovedEmit((*it));
						
						freeAfRemoteService((*it));
						
						asb->getInternalServices()->erase(it);
						removed = true;
						break;
					}
				}
				sem_post(asb->getServicesSem());

                                if(removed)
                                {
                                    LOG_INFO("Service removed: %s", sname.c_str());
                                } else {
                                    LOG_INFO("Service not removed: %s", sname.c_str());
                                }
        	}

            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        	LOG_INFO("ALL_FOR_NOW");
            break;

        case AVAHI_BROWSER_CACHE_EXHAUSTED:
        	LOG_INFO("CACHE_EXHAUSTED");
            break;
    }
}

} // end namespace servicediscovery


