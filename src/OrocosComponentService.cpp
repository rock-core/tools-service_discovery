#include "OrocosComponentService.h"

namespace dfki { namespace communication {

static afLoggingWrapper logger("OrocosComponentService");

void OrocosComponentServiceBase::extractIOR(std::list<std::string> &list)
{
	
	int tokens = 0;
	
	{
		std::list<std::string>::iterator it;
		for (it = list.begin() ; it != list.end() ; it++) {
			if ((*it).substr(0, 8) == "IORSIZE=") {
				tokens = atoi((*it).substr(8).c_str());
				list.erase(it);
				break;
			}
		}
	}
	
	if (tokens == 0) {
		logger.log(FATAL, "OrocosComponentServiceBase - IOR not found in string list");
		throw ior_not_in_list;
	}
	
	std::vector<std::string> ior_vector(tokens, "");
	for (int i = 0 ; i < tokens ; i++) {
		std::list<std::string>::iterator it;
		for (it = list.begin() ; it != list.end() ; ++it) {
			if ((*it).substr(0, 3) == "IOR") {
				size_t found;
				found = (*it).find("=");

				if (found == std::string::npos) {
					continue;	
				}
				int ind = atoi( (*it).substr(3, (int) found - 3).c_str() );

				if (ind <= 0 || ind > tokens) {
					continue;
				}
				
				ior_vector[ind - 1] = (*it).substr(found + 1);
				it = list.erase(it);
			}
		}
	}
	
	std::string ior_t = "";
	for (int i = 0 ; i < tokens ; i++) {
		if (ior_vector[i] == "") {
			logger.log(FATAL, "OrocosComponentServiceBase - incomplete IOR in string list");
			throw ior_incomplete;
		}
		ior_t += ior_vector[i];
	}
	
	IOR = ior_t;
	
}

void OrocosComponentServiceBase::insertIOR(std::string IOR, std::list<std::string> &list)
{
	
	int size = IOR.size();

	int tokens = (size / 200);
	if (tokens % 200 != 0) tokens++;

	std::ostringstream sint;
	sint << tokens;
	std::string valt = sint.str();
	list.push_back( "IORSIZE=" + valt);
	for (int i = 0 ; i < tokens ; i++) {
		std::ostringstream sin;
		sin << i+1;
		std::string val = sin.str();
		list.push_back( "IOR" + val + "=" + IOR.substr(200*i, 200) );
	}
	
	this->IOR = IOR;
	
}

bool OrocosComponentRemoteService::operator==(const OrocosComponentRemoteService& ors) 
{
	bool upres = (OrocosComponentServiceBase) (*this) == (OrocosComponentServiceBase) ors;
	if (!upres) {
		return false;
	}
	
	upres = (afRemoteService) (*this) == (afRemoteService) ors;
	if (!upres) {
		return false;
	}
	
	return true;
}

bool OrocosComponentServiceBase::operator==(OrocosComponentServiceBase ors) 
{
	if (IOR != ors.getIOR()) {
		return false;
	}
	return true;
}



}} 
