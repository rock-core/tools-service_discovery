/*
 * afLocalService.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFLOCALSERVICE_H_
#define AFLOCALSERVICE_H_

#include "afAvahiClient.h"
#include "afService.h"
#include <avahi-client/publish.h>

class afLocalService: public afService {
private:
	AvahiEntryGroup *group;
	AvahiPublishFlags flags;
public:

	afLocalService(
			afAvahiClient *client,
			std::string name,
			std::string type,
			uint16_t port,
			std::list<std::string> list,
			bool publish=true
			);
	afLocalService(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			AvahiPublishFlags flags,
			std::string name,
			std::string type,
			std::string domain,
			uint16_t port,
			std::list<std::string> list,
			bool publish=false
			);
	virtual ~afLocalService();

	static void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata);

	int publish();
	void update();

};

#endif /* AFLOCALSERVICE_H_ */
