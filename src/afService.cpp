/*
 * afService.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afService.h"

#include <list>
#include <string>
#include <iostream>

using namespace std;

afService::afService(const afService& serv) : afServiceBase((const afServiceBase&) serv)
{
	list<string> strlist = serv.getStringList();
	AvahiStringList *txt = NULL;
	if(strlist.size() > 0) {
		list<string>::iterator it;
		it = strlist.begin();
		txt = avahi_string_list_new((*it).c_str(), NULL);
		it++;
		for (;it != strlist.end(); it++) {
			txt = avahi_string_list_add(txt, (*it).c_str());
		}
	}
	this->txt = txt;
}

afService::afService(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain,
			uint16_t port,
			list<string> strlist
			) : afServiceBase(client, interf, prot, name, type, domain) {
	this->port = port;

	AvahiStringList *txt = NULL;
	if(strlist.size() > 0) {
		list<string>::iterator it;
		it = strlist.begin();
		txt = avahi_string_list_new((*it).c_str(), NULL);
		it++;
		for (;it != strlist.end(); it++) {
			txt = avahi_string_list_add(txt, (*it).c_str());
		}
	}
	this->stringlist = strlist;
	this->txt = txt;
}

afService::~afService() {
	if (txt)
		avahi_string_list_free(txt);
}

bool afService::operator==(afService comp) {
	bool upres = (afServiceBase) (*this) == (afServiceBase) comp;
	if (!upres)
		return false;
	if (port != comp.getPort())
		return false;
	if (stringlist != comp.getStringList())
		return false;
	return true;
}
