#ifndef _SERVICE_DISCOVERY_SINGLETON_H_
#define _SERVICE_DISCOVERY_SINGLETON_H_

#include <boost/noncopyable.hpp>


template<class Derived>
class Singleton
{

private:
	static Derived* msInstance;

protected:
	Singleton() {}

public:
	static Derived* getInstance()
	{
		static CGuard g;

		if(msInstance == 0)
		{
			msInstance = new Derived(); 
		}
	
		return msInstance;
	}

	virtual ~Singleton()
	{
	}


	// Nested singleton helper class
	class CGuard
	{
		public: 
			~CGuard()
			{
				if(msInstance != NULL)
				{
					delete msInstance;
					msInstance = NULL;
				}
			}

	};

	friend class CGuard;

};

template<typename Derived> Derived* Singleton<Derived>::msInstance = 0;


#endif // _FAMOS_SINGLETON_H_
