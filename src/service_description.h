#ifndef _SERVICE_DISCOVERY_SERVICEDESCRIPTION_H_
#define _SERVICE_DISCOVERY_SERVICEDESCRIPTION_H_

#include <stdint.h>
#include <list> 
#include <algorithm>
#include <string>
#include <vector>
#include <avahi-core/lookup.h>

namespace servicediscovery { 

class ServiceEvent;

/**
* @class ServiceDescription
* @brief Description and parameterization of a service instance
*
*/
class ServiceDescription
{
	friend class ServiceEvent;

	protected: 
		/* Name of service */
		std::string name_;
                /* Type of service such as _service._tcp */
		std::string type_;
                /* network protocol connected with this service. either ipv4 or ipv6 or both */
		AvahiProtocol protocol_;
                /* Name of domain such as .local */
		std::string domain_;

                /*Index of network interface connected with this service */
		AvahiIfIndex interfaceIndex_;

		std::list<std::string> descriptions_;
		std::vector<std::string> labels_;

		static const uint32_t mDNSMaxRecordSize;
		static const uint32_t mDNSMaxPayloadSize;
		
		ServiceDescription();	

		int getDescriptionSize() const;

	public:
		ServiceDescription(const std::string& servicename);

		virtual ~ServiceDescription();

		bool operator==(const ServiceDescription& other) const;

		bool operator!=(const ServiceDescription& other) const;

		std::string getName() const;
		
		void setName(const std::string& name);

                /**
                 * compare this ServiceDescription without concering TXT data
                 */
                bool compareWithoutTXT(const ServiceDescription& other) const;

		/**
		* Get the description associated with this service and 
		* a certain label
		* \param label 
		*/	
		std::string getDescription(const std::string& label) const;
	
		/**
		* Set a description field (identified by a label) associated with this service.
		* Note that the description should be as brief as possible for performance 
    * reasons, since a low level protocol mDNS needs to handle the data.
    *
    * This method can be used several times for adding different label/descriptions 
    * to this service. It's also possible to overwrite/update a specific label with
    * another description.
		* \param label Label of description
		* \param description Description
		*/	
		void setDescription(const std::string& label, const std::string& description);

                /**
                * Allows to set the txt field of a service in a raw fashion
                */
		void setRawDescriptions(const std::list<std::string>& descriptions);

		/**
		* Get all raw description fields
                * \return List of txt fields
		*/
		std::list<std::string> getRawDescriptions() const;

                /**
                * Retrieve all labels that are available in the description 
                * of the service
                * \return List of labels
                */
		std::vector<std::string> getLabels() const;

                /**
                * Set type of service, such as _myservice._tcp
                */
		void setType(const std::string& type);
	
                /**
                * Retrieve type of service
                * \return type
                */	
		std::string getType() const;
	
                /**
                * Set protocol of service by name of protocol
                */	
		void setProtocol(int protocol);
		
		int getProtocol() const;

		std::string getProtocolString() const;

                /**
                * Set domain such as .local
                */
		void setDomain(const std::string& domain);
	
                /**
                * Get domain sucha as .local
                */	
		std::string getDomain() const;
	
                /**
                * Set interface index
                */	
		void setInterfaceIndex(int interfaceIndex);
		
                /**
                * Get interface index
                */
		int getInterfaceIndex() const;
};


}
#endif // _SERVICE_DISCOVERY_SERVICEDESCRIPTION_H_



