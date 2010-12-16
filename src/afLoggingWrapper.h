/*
 * @file LoggingWrapper.h
 * @author Marlon Meyer, mmeyer@tzi
 * @author Thomas Roehr, thomas.roehr@rock.de
 *
 * @brief Logging class currently based on log4cpp -- future logging should take advantage of log4cxx, which is closer to the 
 * log4j implementation and more current
 */

#ifndef LOGGINGWRAPPER_H_
#define LOGGINGWRAPPER_H_

#undef debug
#undef error
#undef info

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <typeinfo>


//#include "ScoutConfiguration.h"
//#include "ScoutMessageQueues.h"
//#include "IScoutSubSystemProcess.h"



using namespace std;

namespace rock { namespace communication {

	enum Priority	{ FATAL, ERROR, WARN, INFO, DEBUG
//	, TRACE, NOTSET 
	};

	/**
	 * @class LoggingWrapper
	 * @brief LoggingWrapper is a Wrapper for simple use of log4cxx
	 */
	class afLoggingWrapper 
	{
	public:

		/**
		 * Sets up the logger
		 * @param classname Classname that identifies the current logger (where the logger logs)
		 */
		afLoggingWrapper(std::string classname);		

		virtual ~afLoggingWrapper();

		/**
		 * Logs an message with the defined priority and source
		 * @param priority defines the priority level of the message
		 * @param message the message to be logged
		 */
		void log(Priority priority, std::string message);
		
		/**
 		* Logs a message with a given priority, can be used with printf style format
 		* @param priority priority level
 		* @param format string
 		* @param ... variable argument list
 		*/
		void log(Priority priority, const char* format, ...);


		/**
		 * specified where to be logged locally. Note that local logging has to be activated.
		 */
		void setLocalOutputs(bool file, bool console);


	private:
		std::string classname_;

		log4cxx::LoggerPtr logger_;

	};

}} // end namespace

#endif /* LOGGINGWRAPPER_H_ */

