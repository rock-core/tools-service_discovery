/*
 * afRemoteService.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afRemoteService.h"

#include <iostream>

using namespace std;
using namespace dfki::communication;

afRemoteService::afRemoteService(
			afServiceBrowser *browser,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain,
			std::list<std::string> list,
			uint16_t port,
			std::string host_name,
			AvahiAddress address,
			AvahiServiceResolver *sr
	) : afService(browser->getClient(), interf, prot, name, type, domain, port, list) {
	this->host_name = host_name;
	this->address = address;
	this->sr = sr;
}

afRemoteService::~afRemoteService() {

	
}


bool afRemoteService::operator ==(afRemoteService serv) {
	bool upres = (afService) (*this) == (afService) serv;
	if (!upres)
		return false;
	if (host_name.compare(serv.getHostName()) != 0)
		return false;
	if (getAddressString().compare(serv.getAddressString()) != 0)
		return false;
	return true;
}


