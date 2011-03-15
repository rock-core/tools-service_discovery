#include "rice/Class.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"
#include "rice/Array.hpp"

#include <vector>
#include <service-discovery/ServiceDiscovery.h>
#include <service-discovery/ServiceDescription.h>

using namespace Rice;
namespace dc = rock::communication;


typedef std::vector<dc::ServiceDescription> ServiceList;

// Define the data types for the ruby objects here
Data_Type<dc::ServiceDescription> rb_cServiceDescription;
Data_Type<ServiceList> serviceList;

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
	ServiceDiscovery(const std::string& name, const std::string& type): discovery_(), configuration_(name, type, 12000)
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

	void start() 
	{
		discovery_.start(configuration_);
	}

	/**
	 * Search for services
	 * TODO: Improve the pattern search of the underlying service-discovery library
         */
	std::vector<dc::ServiceDescription> findServices(const std::string& name)
	{
		return discovery_.findServices(dc::ServiceDiscovery::SearchPattern(name));
	}
};

}

Data_Type<wrap::ServiceDiscovery> rb_cServiceDiscovery;

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

 // Defining the ruby object 'ServiceDescription'
 rb_cServiceDescription = define_class<dc::ServiceDescription>("ServiceDescription")
	.define_constructor(Constructor<dc::ServiceDescription, const std::string&>())
	.define_method("getName", &dc::ServiceDescription::getName)
	.define_method("setDescription", &dc::ServiceDescription::setDescription)
	.define_method("getDescription", &dc::ServiceDescription::getDescription)
	.define_method("getLabels", &wrap_getLabels)
	;
 
 // Defining the ruby object 'ServiceDiscovery'
 rb_cServiceDiscovery = define_class<wrap::ServiceDiscovery>("ServiceDiscovery")
	// constructor (name, servicetype)
	.define_constructor(Constructor<wrap::ServiceDiscovery, const std::string&, const std::string&>())
	.define_method("setDescription",&wrap::ServiceDiscovery::setDescription)
	.define_method("getDescription",&wrap::ServiceDiscovery::getDescription)
	.define_method("start", &wrap::ServiceDiscovery::start)
	.define_method("findServices", &wrap::ServiceDiscovery::findServices)
	;

}

