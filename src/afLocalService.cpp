
/*
 * afLocalService.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afLocalService.h"
#include <iostream>

using namespace std;

afLocalService::~afLocalService() {
	if (group) {
		avahi_entry_group_free(group);
	}
}

afLocalService::afLocalService(afAvahiClient *client,
		AvahiIfIndex interf,
		AvahiProtocol prot,
		AvahiPublishFlags flags,
		std::string name,
		std::string type,
		std::string domain,
		uint16_t port,
		std::list<std::string> list,
		bool publish)
	: afService(client, interf, prot, name, type, domain, port, list)
{
	group = NULL;
	this->flags = flags;
	if (publish) {
		int pres = this->publish();
	}
}

afLocalService::afLocalService(
		afAvahiClient *client,
		std::string name,
		std::string type,
		uint16_t port,
		std::list<std::string> list,
		bool publish)
	: afService(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, name, type, "", port, list)
{
	group = NULL;
	flags = (AvahiPublishFlags) 0;
	if (publish) {
		int pres = this->publish();
	}
}

int afLocalService::publish()
{

	if (!getClient()) {
		cerr << "Client pointer is NULL\n";
		return -1;
	}


	if (group) {
		cerr << "AvahiEntryGroup pointer not null. Already published?\n";
		return -2;
	}

	if (!(group = avahi_entry_group_new(getClient()->getAvahiClient(), entry_group_callback, this))) {
		cerr << "Failed to create entry group: " << avahi_strerror(avahi_client_errno(getClient()->getAvahiClient())) << endl;
        return -3;
    }

	int ret;
    if ((ret = avahi_entry_group_add_service_strlst(
				group,
				getInterface(),
				getProtocol(),
				flags,
				getName().c_str(),
				getType().c_str(),
				getDomainChar(),
				NULL,
				getPort(),
				getTxt())) < 0) {

    	cerr << "Failed to add service: " << avahi_strerror(ret) << endl;
    	unpublish();
    	return -4;
    }
    
    avahi_entry_group_add_record(
    
    );

    if ((ret = avahi_entry_group_commit(group)) < 0) {
    	cerr << "Failed to commit entry group: " << avahi_strerror(ret) << endl;
    	unpublish();
    	return -5;
    }
    
    return 0;

}

void afLocalService::unpublish()
{
	if (group) {
		avahi_entry_group_free(group);
		group = NULL;
	}
}

void afLocalService::entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata)
{
	afLocalService* ls = (afLocalService*) userdata;
    switch (state) {
        case AVAHI_ENTRY_GROUP_ESTABLISHED :
            cout << "SERVICE: " << ls->getName() << " ESTABLISHED\n";
            break;

        case AVAHI_ENTRY_GROUP_COLLISION : {
			cerr << "Service collision for service: " << ls->getType() << " : " << ls->getName() << endl;
            break;
        }

        case AVAHI_ENTRY_GROUP_FAILURE :
        	cerr
				<< "Entry group for service: "
				<< ls->getName()
				<< " failed: "
				<<  avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g)))
				<< endl;
            break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
            ;
    }
}
