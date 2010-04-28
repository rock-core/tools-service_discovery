/*
 * afList.h
 *
 *  Created on: Mar 19, 2010
 *      Author: darko
 */

#ifndef AFLIST_H_
#define AFLIST_H_

#include <list>
#include <iostream>

namespace dfki {
namespace communication {

/**
 * an extension class to std::list which contains a special method for finding elements
 */
template <class T>
class afList : public std::list<T> {
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
	
//	template <class D>
//	typename std::list<T>::iterator findUpperClass(const D &x)
//	{
//	
//		typename std::list<T>::iterator it;
//		for (it = this->begin(); it != this->end(); it++) {
//			if ((D) (*it) == x) {
//				return it;
//			}
//		}
//		return this->end();

//	}

};


/**
 * a special case for list of pointers where finding elements is dependent on the pointed object and not the pointer
 */
template <>
template <class T>
class afList <T*> : public std::list<T*> {
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

}
}


#endif /* AFLIST_H_ */
