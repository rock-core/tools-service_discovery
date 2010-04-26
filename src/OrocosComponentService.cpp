
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <sstream>
#include "OrocosComponentService.h"

using namespace std;
using namespace dfki::communication;

OrocosComponentServiceBase::OrocosComponentServiceBase(list<string> &list)
{
	
	
	int tokens = 0;
	
	{
		std::list<string>::iterator it;
		for (it = list.begin() ; it != list.end() ; it++) {
			if ((*it).substr(0, 8) == "IORSIZE=") {
				tokens = atoi((*it).substr(8).c_str());
				list.erase(it);
				break;
			}
		}
	}
	
	if (tokens == 0) {
		cerr << "OrocosComponentServiceBase: IOR not found in string list.\n";
		throw ior_not_in_list;
	}
	
	vector<string> ior_vector(tokens, "");
	for (int i = 0 ; i < tokens ; i++) {
		std::list<string>::iterator it;
		for (it = list.begin() ; it != list.end() ; ++it) {
			if ((*it).substr(0, 3) == "IOR") {
				size_t found;
				found = (*it).find("=");

				if (found == string::npos) {
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
	
	string ior_t = "";
	for (int i = 0 ; i < tokens ; i++) {
		if (ior_vector[i] == "") {
			cerr << "OrocosComponentServiceBase: incomplete IOR in string list\n";
			throw ior_incomplete;
		}
		ior_t += ior_vector[i];
	}
	
	IOR = ior_t;
	
}

OrocosComponentServiceBase::OrocosComponentServiceBase(string IOR, list<string> &list)
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
	
}


