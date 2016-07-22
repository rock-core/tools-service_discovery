#include "RemoteService.hpp"
#include "ServiceBrowser.hpp"
#include <iostream>
#include <stdexcept>
#include <base-logging/Logging.hpp>

namespace servicediscovery { 
namespace avahi {

RemoteService::RemoteService( ServiceBrowser *browser
        , AvahiIfIndex interf
        , AvahiProtocol prot
        , std::string name
        , std::string type
        , std::string domain
        , std::list<std::string> list
        , uint16_t port
        , std::string host_name
        , AvahiAddress address
        , AvahiServiceResolver *sr )
    : Service(browser->getClient(), interf, prot, name, type, domain, port, list)
{
    this->host_name = host_name;
    this->address = address;
    this->sr = sr;
}

RemoteService::~RemoteService()
{}


bool RemoteService::operator ==(RemoteService serv)
{
    bool upres = (Service) (*this) == (Service) serv;
    if (!upres)
        return false;
    if (host_name.compare(serv.getHostName()) != 0)
        return false;
    if (getAddressString().compare(serv.getAddressString()) != 0)
        return false;
    return true;
}

void RemoteService::freeServiceResolver()
{
    if(sr)
    {
        if(resolveData)
        {
            delete resolveData;
            resolveData = NULL;
        }

        avahi_service_resolver_free(sr);
        sr = NULL;
    }
}

std::string RemoteService::getAddressString()
{
    char a[AVAHI_ADDRESS_STR_MAX];
    avahi_address_snprint(a, sizeof(a), &address);
    std::string addr(a);
    return addr;
}

} // end namespace avahi
} // end namespace servicediscovery
