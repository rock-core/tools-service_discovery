/*
 * service.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include <service_discovery/service.h>
#include <base/logging.h>

namespace servicediscovery { 

AvahiStringList* Service::getTxt(std::list<std::string> lst)
{
	AvahiStringList *stxt = NULL;
	if(lst.size() > 0) 
	{
                // initialize string
		std::list<std::string>::iterator it = lst.begin();
		stxt = avahi_string_list_new(it->c_str(), NULL);
		++it;

		for (;it != lst.end(); ++it) 
		{
			stxt = avahi_string_list_add(stxt, it->c_str());
		}
	}
	return stxt;
}

Service::Service(const Service& serv) 
 : configuration_(serv.configuration_), client_(serv.client_) 
{
	this->stringlist = serv.getStringList();
	//create avahistringlist from a list<string>
	//this is done to prevent using the same instance of avahistringlist on object copy and then deleting it on the destruction of the object
	this->txt = Service::getTxt(serv.getStringList());
        this->configuration_.setRawDescriptions(stringlist);

	this->port = serv.getPort();
	this->dontCheckTXT = serv.dontCheckTXT;
}

Service::Service(
			Client *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain,
			uint16_t port,
			std::list<std::string> strlist
			)
{

        client_ = client;
        configuration_.setInterfaceIndex(interf);
        configuration_.setProtocol(prot);
        configuration_.setName(name);
        configuration_.setType(type);
        configuration_.setDomain(domain);

	this->port = port;
	this->dontCheckTXT = false;

	//create avahistringlist from list<string>
	this->stringlist = strlist;
	this->txt = Service::getTxt(strlist);

        this->configuration_.setRawDescriptions(stringlist);
}

Service::~Service()
{
	if (txt)
		avahi_string_list_free(txt);
}

bool Service::operator==(const Service& comp)
{
	bool upres;
        if( !(this->dontCheckTXT || comp.dontCheckTXT) )
            upres = (configuration_ == comp.getConfiguration());
        else 
            upres = configuration_.compareWithoutTXT(comp.getConfiguration());

	if (!upres)
		return false;

	if (port != comp.getPort())
		return false;
	if (!(this->dontCheckTXT || comp.dontCheckTXT)) {
		if (stringlist != comp.getStringList())
			return false;
	}
	return true;
}


void Service::addDescriptionsToConfiguration(const std::list<std::string>& strlist)
{
    ServiceConfiguration config = getConfiguration();

    std::list<std::string>::const_iterator it;

    for(it = strlist.begin(); it != strlist.end(); it++) {
        size_t pos = it->find('=');

        std::string key = it->substr(0, pos);
        std::string val = it->substr(pos + 1);

        config.setDescription(key, val);
    }

    setConfiguration(config);
}

void Service::setConfiguration(const ServiceConfiguration& config)
{
    configuration_ = config;
}

ServiceConfiguration Service::getConfiguration() const
{
    return configuration_;
}

Client* Service::getClient()
{
    assert(client_);
    return client_;
}

void Service::setStringList(std::list<std::string> list)
{
    stringlist = list;
    setTxt( Service::getTxt(stringlist) );
    configuration_.setRawDescriptions(stringlist);
}

} // end namespace servicediscovery

