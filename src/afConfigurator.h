#ifndef __AFCONFIGURATOR_H__
#define __AFCONFIGURATOR_H__

#include <string>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/properties.h>

namespace dfki { namespace communication {

class afConfigurator
{
	public:

		void configureLoggingProperties(std::string);
		void configureDefaultLoggingProperties();
		bool isConfigured() {return loggingConfigured;}
		
		static afConfigurator * getInstance();

	private:
		afConfigurator ();
		virtual ~afConfigurator ();
		afConfigurator (const afConfigurator&);

		static afConfigurator * instance;
		bool loggingConfigured;
};

}}

#endif /* __AFCONFIGURATOR_H__ */

