/*
 * afServiceBase.cpp
 *
 *  Created on: Mar 19, 2010
 *      Author: darko
 */

#include "afServiceBase.h"
#include <iostream>

using namespace std;

afServiceBase::afServiceBase(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain
			) {
	if (!client) {
		cerr << "Client pointer NULL\n";
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

bool afServiceBase::operator==(afServiceBase comp) {
	if (interface != comp.getInterface())
		return false;
	if (protocol != comp.getProtocol())
		return false;
	if (name.compare(comp.getName()) != 0)
		return false;
	if (type.compare(comp.getType()) != 0)
		return false;
	if (domain.compare(comp.getDomain()) != 0)
		return false;
	return true;
}
