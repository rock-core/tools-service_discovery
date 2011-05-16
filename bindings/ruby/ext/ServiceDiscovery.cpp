#include "rice/Class.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"
#include "rice/Array.hpp"

#include <vector>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <base/logging.h>
#include <service_discovery/service_discovery.h>
#include <service_discovery/service_description.h>

using namespace Rice;
namespace dc = servicediscovery;


typedef std::vector<dc::ServiceDescription> ServiceList;
typedef std::vector<std::string> StringList;

// Define the data types for the ruby objects here
Data_Type<dc::ServiceDescription> rb_cServiceDescription;
Data_Type<ServiceList> serviceList;

static Module rb_mServiceDiscovery;

template<>
StringList from_ruby<StringList>(Object types)
{
        Array typeList(types);
        std::vector<std::string> result;
        result.reserve(typeList.size());
        
        Array::iterator it = typeList.begin();
        for(; it != typeList.end(); ++it)
        {
                result.push_back(from_ruby<std::string>(*it));
        }

        return result;
}

template<>
Object to_ruby<StringList>(const StringList& list)
{
        Array array;
        StringList::const_iterator it = list.begin();
        for(; it != list.end(); it++)
        {
                array.push(*it);
        }

        return array;
}

namespace wrap
{

/**
* Wrapper class for service discovery, so that we have an easier interface configuration
* Maybe we should even cleanup the ServiceDiscover interface itself, so that we don't need this wrapper
*/
class ServiceDiscovery
{
	dc::ServiceDiscovery discovery_;
	dc::ServiceConfiguration configuration_;
public:
	/**
	 * \brief ServiceDiscovery with a certain service name and service type
         * \param name Name of service
         * \param type Such as _rimres._tcp
         */
	ServiceDiscovery() : discovery_(), configuration_()
	{
	}

	~ServiceDiscovery()
	{
		discovery_.stop();
	}

	void setDescription(const std::string& label, const std::string& description)
	{
		configuration_.setDescription(label, description);
	}

	std::string getDescription(const std::string& label)
	{
		return configuration_.getDescription(label);
	}

        void update() {
            discovery_.update(configuration_);
        }

        /**
        * Publish service with service name and a type such as _mydomain._tcp
        * \param name Name
        * \param type Service type such as _mydomain._tcp
        */
	void publish(const std::string& name, const std::string& type)
	{
                configuration_.setName(name);
                configuration_.setType(type);
                configuration_.setPort(12000);

		discovery_.start(configuration_);
	}

        /**
        * Listen for additional types such as _additional._tcp
        *
        */
        void listenOn(std::vector<std::string> types)
        {
                discovery_.listenOn(types);
        }

	/**
	 * Search for services
         * To retrieve all call with an empty string 
	 * TODO: Improve the pattern search of the underlying service-discovery library
         */
	std::vector<dc::ServiceDescription> findServices(const std::string& name)
	{
		return discovery_.findServices(dc::ServiceDiscovery::SearchPattern(name));
	}

	std::vector<std::string> getAllServices()
	{
		return discovery_.getServiceNames();
	}

};

}

Data_Type<wrap::ServiceDiscovery> rb_cServiceDiscovery;

// WORKAROUND: should actually use from_ruby<StringList>(domains) here, 
// but using that within minitest fails with 
// TypeError: wrong argument type Array (expected Data)
void wrap_listenOn(Object self, Object domains)
{
        Data_Object<wrap::ServiceDiscovery> sd(self, rb_cServiceDiscovery);

        Array typeList(domains);
        std::vector<std::string> types;
        types.reserve(typeList.size());
        
        Array::iterator it = typeList.begin();
        for(; it != typeList.end(); ++it)
        {
                types.push_back(from_ruby<std::string>(*it));
        }

        sd->listenOn(types);
}


Array wrap_getLabels(Object description)
{
	Data_Object<dc::ServiceDescription> sd(description, rb_cServiceDescription);
	std::vector<std::string> labels = sd->getLabels();
	
	Array labelArray;
	for(int i = 0; i < labels.size(); i++)
	{
		labelArray.push( String(labels[i]) );
	}

	return labelArray;
}

/**
* Making sure ruby can convert a list of service to an array of ServiceDescription objects
*/
template<>
Object to_ruby<ServiceList>(const ServiceList& services)
{
	Array serviceList;
	ServiceList::const_iterator it;
	for(it = services.begin(); it != services.end(); it++)
	{
		serviceList.push( Data_Object<dc::ServiceDescription>(new dc::ServiceDescription(*it)) );
	}

	return serviceList;
}


// Initialise ServiceDiscovery
// The name of the library is required as init
extern "C"
void Init_servicediscovery_ruby()
{

 // Define module Avahi 
 rb_mServiceDiscovery = define_module("Avahi");

 // Defining the ruby class 'ServiceDescription'
 rb_cServiceDescription = define_class_under<dc::ServiceDescription>(rb_mServiceDiscovery, "ServiceDescription")
	.define_constructor(Constructor<dc::ServiceDescription, const std::string&>())
	.define_method("get_name", &dc::ServiceDescription::getName)
	.define_method("set_description", &dc::ServiceDescription::setDescription, (Arg("label"), Arg("description")))
	.define_method("get_description", &dc::ServiceDescription::getDescription, (Arg("label")))
	.define_method("get_labels", &wrap_getLabels)
	;
 
 // Defining the ruby class 'ServiceDiscovery'
 rb_cServiceDiscovery = define_class_under<wrap::ServiceDiscovery>(rb_mServiceDiscovery, "ServiceDiscovery")
	// constructor (name, servicetype)
	.define_constructor(Constructor<wrap::ServiceDiscovery>())
	.define_method("set_description",&wrap::ServiceDiscovery::setDescription, (Arg("label"), Arg("content")))
	.define_method("get_description",&wrap::ServiceDiscovery::getDescription, (Arg("label")))
	.define_method("publish", &wrap::ServiceDiscovery::publish, (Arg("name"), Arg("type")) )
	.define_method("listen_on", wrap_listenOn, (Arg("domain list")) ) //&wrap::ServiceDiscovery::listenOn, (Arg("domain list")) )
        .define_method("update", &wrap::ServiceDiscovery::update)
	.define_method("find_services", &wrap::ServiceDiscovery::findServices, (Arg("servicename")) )
	.define_method("get_all_services", &wrap::ServiceDiscovery::getAllServices)
        ;
}

