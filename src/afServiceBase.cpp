/*
 * afServiceBase.cpp
 *
 *  Created on: Mar 19, 2010
 *      Author: darko
 */

#include "afServiceBase.h"

namespace rock { namespace communication {

static afLoggingWrapper logger("afServiceBase");

afServiceBase::afServiceBase(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain
			) {
	if (!client) {
		logger.log(FATAL, "Client pointer NULL");
		throw 0; //TODO do sth else
	}
	this->client = client;
	this->interface = interf;
	this->protocol = prot;
	this->name = name;
	this->type = type;
	this->domain = domain;
}


afServiceBase::~afServiceBase() {
	// TODO Auto-generated destructor stub
}

bool afServiceBase::operator==(const afServiceBase& comp) {
	if (interface != comp.getInterface() &&  !(interface == AVAHI_IF_UNSPEC || comp.getInterface() == AVAHI_IF_UNSPEC))
		return false;
	if (protocol != comp.getProtocol() && !( protocol == AVAHI_PROTO_UNSPEC || comp.getProtocol() == AVAHI_PROTO_UNSPEC))
		return false;
	if (name.compare(comp.getName()) != 0)
		return false;
	if (type.compare(comp.getType()) != 0)
		return false;
	std::string td1 = (domain == "" || domain == "local") ? "" : domain;
	std::string td2 = (comp.getDomain() == "" || comp.getDomain() == "local") ? "" : comp.getDomain();
	if (td1 != td2)
		return false;
	return true;
}

}}
