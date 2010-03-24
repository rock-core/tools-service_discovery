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
