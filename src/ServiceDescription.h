#ifndef __SERVICEDISCOVERY_SERVICEDESCRIPTION_H_
#define __SERVICEDISCOVERY_SERVICEDESCRIPTION_H_

#include <stdint.h>
#include <list> 
#include <algorithm>
#include <string>
#include <vector>

namespace dfki { namespace communication {

class ServiceEvent;

/**
* @class ServiceDescription
* @brief Description and parameterization of a service
*
*/
class ServiceDescription
{
	friend class ServiceEvent;

	protected: 
		// Identifying name		
		std::string name_;
		std::string type_;
		std::string protocol_;
		std::string domain_;
		int interfaceIndex_;

		std::list<std::string> descriptions_;
		std::vector<std::string> labels_;

		// uint32_t descriptionsSize_;

		static const uint32_t mDNSMaxRecordSize;
		static const uint32_t mDNSMaxPayloadSize;
		
		ServiceDescription();	

		void setRawDescriptions(const std::list<std::string>& descriptions);

	public:
		ServiceDescription(const std::string& servicename);

		virtual ~ServiceDescription();

		bool operator==(const ServiceDescription& other) const;

		bool operator!=(const ServiceDescription& other) const;

		std::string getName() const;
		
		void setName(const std::string& name);

		/**
		* Get the description associated with this service and 
		* a certain label
		* \param label 
		*/	
		std::string getDescription(const std::string& label) const;
	
		/**
		* Set a description field (identified by a label) associated with this service.
		* Note that the description should be as 
		* brief as possible for performance reasons,
		* since a low level protocol mDNS needs to handle the data
		* \param label Label of description
		* \param description Description
		*/	
		void setDescription(const std::string& label, const std::string& description);

		/**
		* Get all raw description fields
		*/
		std::list<std::string> getRawDescriptions() const;

		std::vector<std::string> getLabels() const;

		void setType(const std::string& type);
		
		std::string getType() const;
		
		void setProtocol(const std::string& domain);
		
		std::string getProtocol() const;

		void setDomain(const std::string& domain);
		
		std::string getDomain() const;
		
		void setInterfaceIndex(int interfaceIndex);
		
		int getInterfaceIndex() const;
};


}}

#endif // __SERVICEDISCOVERY_SERVICEDESCRIPTION_H_



