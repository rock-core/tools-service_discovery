#include "ServiceBrowser.hpp"
#include "Client.hpp"
#include <stdexcept>
#include <base-logging/Logging.hpp>

namespace servicediscovery {
namespace avahi {

void ServiceBrowser::bootstrap()
{
    browser = avahi_service_browser_new(
        client->getAvahiClient(),
        interface,
        protocol,
        getInType(),
        getInDomain(),
        flags,
        browseCallback,
        this);

    if (!browser)
    {
        char buffer[512];
        snprintf(buffer, 512, "Failed to create avahi service browser: %s", avahi_strerror(avahi_client_errno(client->getAvahiClient())));
        LOG_FATAL(buffer);
        throw std::runtime_error(buffer);
    }
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

ServiceBrowser::ServiceBrowser(Client *client, std::string type)
{
    this->client = client;
    this->interface = AVAHI_IF_UNSPEC;
    this->protocol = AVAHI_PROTO_UNSPEC;
    this->type = type;
    this->domain = "";
    this->flags = (AvahiLookupFlags) 0;
    this->bootstrap();
}

void ServiceBrowser::cleanup()
{
    if (browser)
    {
        avahi_service_browser_free(browser);
        browser = NULL;
    }

    //free service resolvers
    boost::unique_lock<boost::mutex> lock(mServicesMutex);
    List<RemoteService>::iterator it;
    for (it = services.begin() ; it != services.end(); ++it)
    {
        it->freeServiceResolver();
    }
}

ServiceBrowser::~ServiceBrowser()
{}

//a callback called on a service resolver event
void ServiceBrowser::resolveCallback(AvahiServiceResolver *sr, AvahiIfIndex interface, AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, void *userdata)
{
    ResolveData *data = (ResolveData*) userdata;
    ServiceBrowser *sb = data->serviceBrowser;
    switch(event)
    {
        // if found service cannot be resolved, throw an error
        case AVAHI_RESOLVER_FAILURE:
            LOG_INFO("Resolver failed to resolve service %s of type %s on interface %d. Error: %s", 
                name,
                type,
                interface,
                avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(sr))));
            if (data->count >= ServiceBrowser::MAX_RESOLVER_TRIES)
            {
                //TODO: it seems that it doesn't make more attempts'
                LOG_WARN("Failed attempts to resolve this service has exceeded the limit. Service resolver object freed");
                delete data;
                avahi_service_resolver_free(sr);
            } else {
                data->count = data->count + 1;
                LOG_WARN("Failed attempt %d out of %d", data->count, ServiceBrowser::MAX_RESOLVER_TRIES);
            }
            break;

        // if found service can be resolved
        // Note that once the resolver has been found and the service should be kept informed
        // about updates, the AvahiServiceResolver has to be kept (should not be deleted)
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
            while (tmp != NULL)
            {
                strlist.push_back((const char*) tmp->text);
                tmp = tmp->next;
            }

            RemoteService rms(sb, interface, protocol, sname, stype, sdomain, strlist, port, shost, *address, sr);
            rms.resolveData = data;
            ServiceConfiguration remoteConfig = rms.getConfiguration();


            LOG_INFO("Service resolved: %d %d %s %s %s", remoteConfig.getInterfaceIndex(), remoteConfig.getProtocol(), remoteConfig.getName().c_str(), remoteConfig.getType().c_str(), remoteConfig.getDomain().c_str());

            boost::unique_lock<boost::mutex> lock(sb->getServicesMutex());
            if (sb->getInternalServices()->find(rms) == sb->getInternalServices()->end())
            {
                rms.dontCheckTXT = true;
                List<RemoteService>::iterator srv;
                srv = sb->getInternalServices()->find(rms);
                if (srv != sb->getInternalServices()->end())
                {
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

            break;
    }

}


void ServiceBrowser::browseCallback(AvahiServiceBrowser *sb, AvahiIfIndex interface, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void *userdata)
{
    AvahiClient *client = avahi_service_browser_get_client(sb);
    ServiceBrowser *serviceBrowser = (ServiceBrowser*) userdata;

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
            tdata->serviceBrowser = serviceBrowser;
            tdata->count = 0;

            // try to resolve the new found service. If resolver object cannot be created, throw an error
            if (!(avahi_service_resolver_new(client, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, AvahiLookupFlags (0), resolveCallback, tdata)))
            {
                LOG_FATAL("Failed to create service resolver object for: %s. Error: %s", name, avahi_strerror(avahi_client_errno(client)));
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
                Service serv(serviceBrowser->getClient(), interface, protocol, sname, stype, sdomain);
                List<RemoteService>::iterator it;
                //find the signal to be removed
                {
                    boost::unique_lock<boost::mutex> lock(serviceBrowser->getServicesMutex());
                    for (it = serviceBrowser->getInternalServices()->begin() ; it != serviceBrowser->getInternalServices()->end(); ++it)
                    {
                        ServiceConfiguration sc = it->getConfiguration();
                        // The notification of service removal does not contain the TXT information
                        // thus comparision needs to be done here without TXT fields
                        if ( sc.compareWithoutTXT(serv.getConfiguration()) )
                        {
                            //emit the service removed signal
                            serviceBrowser->serviceRemovedEmit((*it));
                            serviceBrowser->getInternalServices()->erase(it);
                            removed = true;
                            break;
                        }
                    }
                }

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

} // end namespace avahi
} // end namespace servicediscovery
