#include "service_description.h"
#include <cmath>
#include <base/logging.h>

namespace servicediscovery {

// Maximum size a mDNS record can store per entry
const uint32_t ServiceDescription::mDNSMaxRecordSize = 200;
const uint32_t ServiceDescription::mDNSMaxPayloadSize = 2000;

ServiceDescription::ServiceDescription() : name_(), type_(), protocol_(AVAHI_PROTO_UNSPEC), domain_(), interfaceIndex_(AVAHI_IF_UNSPEC), descriptions_(), labels_()
{
}

ServiceDescription::ServiceDescription(const std::string& servicename) : name_(servicename), type_(), protocol_(), domain_(), interfaceIndex_(), descriptions_(), labels_()
{
}

ServiceDescription::~ServiceDescription()
{
}


std::string ServiceDescription::getName() const
{
	return name_;
}

void ServiceDescription::setName(const std::string& name)
{
	name_ = name;
}

bool ServiceDescription::compareWithoutTXT(const ServiceDescription& other) const 
{
        if( interfaceIndex_ != other.getInterfaceIndex() && !(interfaceIndex_ == AVAHI_IF_UNSPEC || other.getInterfaceIndex() == AVAHI_IF_UNSPEC))
            return false;

        if(protocol_ != other.getProtocol() && !(protocol_ == AVAHI_PROTO_UNSPEC || other.getProtocol() == AVAHI_PROTO_UNSPEC))
            return false;

        if( getName() != other.getName())
            return false;

        if( getType() != other.getType())
            return false;

        // Compare domains and ignore consider that "" defaults to 'local'
	std::string td1 = (domain_ == "" || domain_ == "local") ? "" : domain_;
	std::string td2 = (other.getDomain() == "" || other.getDomain() == "local") ? "" : other.getDomain();

	if (td1 != td2)
		return false;

        return true;
}

bool ServiceDescription::operator==(const ServiceDescription& other) const
{
        if(!compareWithoutTXT(other))
            return false;

	std::vector<std::string> labels = this->getLabels();

        int labelsSize = labels.size();

        for(int i = 0; i < labelsSize; i++)
        {  
		std::string label = labels_[i];

                if(this->getDescription(label) != other.getDescription(label))
                        return false;
        }   
	return true;
}

bool ServiceDescription::operator!=(const ServiceDescription& other) const
{
	return !(*this == other);
}


std::string ServiceDescription::getDescription(const std::string& label) const
{
	std::list<std::string>::const_iterator it;

	std::string tmpDescription("");
	int labelSize = label.size();
	for(it = descriptions_.begin(); it != descriptions_.end(); it++)
	{
		std::string descriptionItem = *it;
	
		if( descriptionItem.substr(0, labelSize) == label )
		{
			tmpDescription += descriptionItem.substr((labelSize + 1), descriptionItem.size() - (labelSize)); // + 1 in order to remove the "=" sign
		}
	}
	return tmpDescription;
}

void ServiceDescription::setDescription(const std::string& label, const std::string& description)
{
	// Total size of mDNS up to the IP MTU of the physical interface, less
	// the space required for UP header (20 bytes IPv4/40 bytes IPv6, and UDP header 8bytes)
	// MAXIMUM 9000 bytes !!!! we consider max as 2312 (WiFi)
	// considering IPv6 and UDP: 2312 - 48 = 2264 bytes
	// sety margin for description we use max 2000 bytes
	// http://files.multicastdns.org/drt-cheshire-dnsext-multicastdns.txt
	// page 41
	int descriptionsSize = description.size();

	// +1 for the equal sign
	int labelSize = label.size() + 1;
	if( labelSize > 100 )
	{
		LOG_WARN("Size of label is larger than 100 bytes. Description payload of less than 100 bytes.");
	} else if (labelSize >= 200)
	{
		LOG_FATAL("Size of label exceeds maximum payload size");
		return;
	}

	// Based on size how much bytes does the description consume
	int requiredNumberOfEntries = ceil( static_cast<double>(descriptionsSize) / static_cast<double>(mDNSMaxRecordSize - label.size()) );

	int currentDescriptionSize = getDescriptionSize();
	unsigned int currentPayloadSize = descriptionsSize + requiredNumberOfEntries * label.size() + currentDescriptionSize;

	if( currentPayloadSize > mDNSMaxPayloadSize)
	{
		LOG_FATAL("Size of descriptions exceeds maximum payload size: %d vs. maximum %d", currentPayloadSize, mDNSMaxPayloadSize);
	}

	std::list<std::string>::iterator it;	

	std::list<std::string> tmpDescriptions;

	// positive selection of items
	// leave out items with the same label
	for(it = descriptions_.begin(); it != descriptions_.end(); it++)
	{		
		std::string item = *it;
		if( item.substr(0, label.size()) != label)
			tmpDescriptions.push_back(item);
	}

	// add new items
	for(int i = 0; i < requiredNumberOfEntries; i++)
	{
		// split description in chunks of 200 - label +1
		// create entry such as: "IOR=2812739831273"
		std::string entry;
		if (i == 0)
			entry = label + "=" + description.substr(200*i,200 - labelSize);
		else
			entry = label + "=" + description.substr((200-labelSize)*i,200 - labelSize);
		tmpDescriptions.push_back(entry);
	}

	descriptions_ = std::list<std::string>();

	// add Label to labels
	if(! static_cast<int>(count(labels_.begin(), labels_.end(), label)))
		labels_.push_back(label);	

	descriptions_ = tmpDescriptions;
}

std::list<std::string> ServiceDescription::getRawDescriptions() const
{
	return descriptions_;
}

void ServiceDescription::setRawDescriptions(const std::list<std::string>& descriptions)
{
	descriptions_ = descriptions;

	labels_	= std::vector<std::string>();

	std::string currentLabel;
	// Extract markers from descriptions
	std::list<std::string>::iterator it;
	for(it = descriptions_.begin(); it != descriptions_.end(); it++)
	{
		std::string descriptionItem = *it;
		int markerPos = descriptionItem.find('=',0);
		std::string tmpLabel = descriptionItem.substr(0, markerPos);
		if(tmpLabel != currentLabel)
		{
			labels_.push_back(tmpLabel);
			currentLabel = tmpLabel;
		}
	}
}

std::vector<std::string> ServiceDescription::getLabels() const
{
	return labels_;
}

void ServiceDescription::setType(const std::string& type)
{
	type_ = type;
}


std::string ServiceDescription::getType() const
{
	return type_;
}

void ServiceDescription::setProtocol(int protocol)
{
	protocol_ = protocol;
}

int ServiceDescription::getProtocol() const
{
    return protocol_;
}

std::string ServiceDescription::getProtocolString() const
{
        return std::string( avahi_proto_to_string(protocol_) );
}

void ServiceDescription::setDomain(const std::string& domain)
{
    domain_ = domain;

}

std::string ServiceDescription::getDomain() const
{
    return domain_;
}

void ServiceDescription::setInterfaceIndex(int interfaceIndex)
{
	interfaceIndex_ = interfaceIndex;
}

int ServiceDescription::getInterfaceIndex() const
{
	return interfaceIndex_;
}

int ServiceDescription::getDescriptionSize() const
{
    std::list<std::string> descriptions = getRawDescriptions();
    std::list<std::string>::iterator it = descriptions.begin();

    int size = 0;

    for(; it != descriptions.end(); it++)
    {
	size += it->size();
    }

    return size;
}


} // end namespace servicediscovery

