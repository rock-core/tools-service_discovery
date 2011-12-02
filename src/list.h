/*
 * \file list.h
 * \author darko.makreshanski@dfki.de
 */

#ifndef _SERVICE_DISCOVERY_LIST_H_
#define _SERVICE_DISCOVERY_LIST_H_

#include <list>
#include <iostream>

namespace servicediscovery {

/**
 * an extension class to std::list which contains a special method for finding elements
 */
template <class T>
class List : public std::list<T> {
public:
	typename std::list<T>::iterator find(const T &x)
	{
	
		typename std::list<T>::iterator it;
		for (it = this->begin(); it != this->end(); it++) {
			if ((*it) == x) {
				return it;
			}
		}
		return this->end();

	}
	
};


/**
 * a special case for list of pointers where finding elements is dependent on the pointed object and not the pointer
 */
template <>
template <class T>
class List <T*> : public std::list<T*> {
public:
	typename std::list<T>::iterator find(T &x)
	{
		typename std::list<T*>::iterator it;
		for (it = this->begin() ; it != this->end(); it++) {
			T* ptr = (*it);
			if ((*ptr) == x) {
				return it;
			}
		}
		return this->end();
	}
};

} // end namespace servicediscovery


#endif /* _SERVICE_DISCOVERY_LIST_H_ */
