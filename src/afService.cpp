/*
 * afService.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include "afService.h"


namespace rock { namespace communication {

AvahiStringList* afService::getTxt(std::list<std::string> lst)
{
	AvahiStringList *stxt = NULL;
	if(lst.size() > 0) {
		std::list<std::string>::iterator it;
		it = lst.begin();
		stxt = avahi_string_list_new((*it).c_str(), NULL);
		it++;
		for (;it != lst.end(); it++) {
			stxt = avahi_string_list_add(stxt, (*it).c_str());
		}
	}
	return stxt;
}

afService::afService(const afService& serv) : afServiceBase((const afServiceBase&) serv)
{
	
	this->stringlist = serv.getStringList();
	//create avahistringlist from a list<string>
	//this is done to prevent using the same instance of avahistringlist on object copy and then deleting it on the destruction of the object
	this->txt = afService::getTxt(serv.getStringList());
	this->port = serv.getPort();
	this->dontCheckTXT = serv.dontCheckTXT;
}

afService::afService(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain,
			uint16_t port,
			std::list<std::string> strlist
			) : afServiceBase(client, interf, prot, name, type, domain) {
	this->port = port;
	this->dontCheckTXT = false;

	//create avahistringlist from list<string>
	this->stringlist = strlist;
	this->txt = afService::getTxt(strlist);
}

afService::~afService() {
	if (txt)
		avahi_string_list_free(txt);
}

bool afService::operator==(const afService& comp) {
	bool upres = (afServiceBase) (*this) == (afServiceBase) comp;
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

}}

