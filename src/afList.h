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

/**
 * an extension class to std::list which contains a special method for finding elements
 * in this framework, the below special case is used
 */
template <class T>
class afList : public std::list<T> {
public:
	T* find(T &x)
	{
		typename std::list<T>::iterator it;
		for (it = this->begin(); it != this->end(); it++) {
			if ((*it) == x) {
				return &(*it);
			}
		}
		return NULL;

	}
};


/**
 * a special case for list of pointers where finding elements is dependent on the pointed object and not the pointer
 */
template <>
template <class T>
class afList <T*> : public std::list<T*> {
public:
	T* find(T &x)
	{
		typename std::list<T*>::iterator it;
		for (it = this->begin() ; it != this->end(); it++) {
			T* ptr = (*it);
			if ((*ptr) == x) {
				return (*it);
			}
		}
		return NULL;
	}
};

#endif /* AFLIST_H_ */
