#include "LocalService.hpp"
#include <base/Logging.hpp>

namespace servicediscovery { 
namespace avahi {

LocalService::~LocalService() {
}

LocalService::LocalService(Client *client,
		AvahiIfIndex interf,
		AvahiProtocol prot,
		AvahiPublishFlags flags,
		std::string name,
		std::string type,
		std::string domain,
		uint16_t port,
		std::list<std::string> list,
		uint32_t ttl,
		bool publish)
	: Service(client, interf, prot, name, type, domain, port, list)
	, mGroup(0)
	, mFlags((AvahiPublishFlags) 0) //(flags | AVAHI_PUBLISH_ALLOW_MULTIPLE))
	, mTTL(ttl)
	, mPublished(false)
{
	if(publish) {
		this->publish();
	}
}

LocalService::LocalService(
		Client *client,
		std::string name,
		std::string type,
		uint16_t port,
		std::list<std::string> list,
		uint32_t ttl,
		bool publish)
	: Service(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, name, type, "", port, list)
	, mGroup(0)
	, mFlags( (AvahiPublishFlags) 0 ) //(AVAHI_PUBLISH_ALLOW_MULTIPLE | AVAHI_PUBLISH_USE_MULTICAST) )
	, mTTL(ttl)
{
	if(publish) {
		this->publish();
	}
}

int LocalService::publish()
{
	if (!getClient()) {
		LOG_FATAL("Publish - client pointer is NULL");
		return -1;
	}

	if (mGroup) {
		LOG_WARN("Publish - entry group pointer is not null. Service is already published?");
		return -2;
	}


	// The entry_group_callback is called whenever the state of the entry group changes and called for the first
	// time with avahi_entry_group_new
	//
#ifdef __CUSTOM_TTL__
	if (!(mGroup = avahi_entry_group_new_custom_ttl(getClient()->getAvahiClient(), entry_group_callback, this, mTTL))) {
#else
	if (!(mGroup = avahi_entry_group_new(getClient()->getAvahiClient(), entry_group_callback, this))) {
#endif		
		LOG_FATAL("Publish - Failed to create entry group: %s", avahi_strerror(avahi_client_errno(getClient()->getAvahiClient())));
		return -3;
	}
   
	int ret;
	ServiceConfiguration config = getConfiguration();

	if(avahi_entry_group_is_empty(mGroup)) {
	    if ((ret = avahi_entry_group_add_service_strlst(
	                            mGroup,
	                            AVAHI_IF_UNSPEC,
	                            AVAHI_PROTO_UNSPEC,
	                            mFlags,
	                            config.getName().c_str(),
	                            config.getType().c_str(),
	                            config.getDomain().c_str(),
	                            NULL,
	                            getPort(),
	                            getTxt())) < 0) {
	
	            LOG_FATAL("Failed to add service to the entry group: %s", avahi_strerror(ret));
	            _unpublish();
	            return -4;
	    } 

	    if ((ret = avahi_entry_group_commit(mGroup)) < 0) {
	        LOG_FATAL("Failed to commit entry group: %s", avahi_strerror(ret));
	        _unpublish();
	        return -5;
	    }

	} else {
	    LOG_INFO("Entry group for local service '%s' is not empty", config.getName().c_str());
	}
    
	return 0;
}

void LocalService::unpublish()
{
        // "... please do not free the entry group and create a new one ..."
        // see avahi-common/defs.h
        // so use reset
	UniqueClientLock lock;
	_unpublish();
}

void LocalService::_unpublish()
{
        // "... please do not free the entry group and create a new one ..."
        // see avahi-common/defs.h
        // so use reset
	if (mGroup) {
		avahi_entry_group_reset(mGroup);
		mPublished = false;
	}
}

bool LocalService::published()
{
    return mPublished;
}

void LocalService::entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata)
{
    LocalService* localService = (LocalService*) userdata;
    ServiceConfiguration config = localService->getConfiguration();

    switch (state) {
        case AVAHI_ENTRY_GROUP_ESTABLISHED :
		LOG_INFO("Entry group callback - Service %s is established/published", config.getName().c_str());
		localService->mPublished = true;
		break;

        case AVAHI_ENTRY_GROUP_COLLISION :
	      	LOG_FATAL("Entry group callback - Service collision for %s/%s", config.getType().c_str(), config.getName().c_str());
		break;

        case AVAHI_ENTRY_GROUP_FAILURE :
        	LOG_FATAL("Entry group callback - Entry group failed for service %s, reason: %s", config.getName().c_str(), avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))));
            break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
		LOG_INFO("Entry group uncommited: '%s", config.getName().c_str());
		break;

        case AVAHI_ENTRY_GROUP_REGISTERING:
		LOG_INFO("Entry group registering: '%s'", config.getName().c_str());
		break;
        default: 
                break;
    }
}

int LocalService::updateStringList(std::list<std::string> listn) {
	if (!mGroup) {
		LOG_FATAL("updateStringList - Entry group not found for updating string list");
		return -1;
	}
	
	AvahiStringList *list = Service::getTxt(listn);
	int res;
        ServiceConfiguration config = getConfiguration();
	if ((res = avahi_entry_group_update_service_txt_strlst(
				mGroup,
				config.getInterfaceIndex(),
				config.getProtocol(),
				mFlags,
				config.getName().c_str(),
				config.getType().c_str(),
				config.getDomain().c_str(),
				list)) < 0) {
		LOG_FATAL("updateStringList - Failed to update txt records: %s", avahi_strerror(res));
		return -2;
	}
	
	
	setStringList(listn); 
	
	return 0;
}

} // end namespace avahi
} // end namespace servicediscovery
