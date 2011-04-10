/*
 * @file logging.h
 * @author Marlon Meyer, mmeyer@tzi
 * @author Thomas Roehr, thomas.roehr@rock.de
 *
 * @brief Logging class based on log4cpp 
 */

#ifndef LOGGINGWRAPPER_H_
#define LOGGINGWRAPPER_H_

#undef debug
#undef error
#undef info

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <typeinfo>

#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/SimpleLayout.hh>

using namespace std;

namespace servicediscovery {

	enum Priority	{ FATAL, ERROR, WARN, INFO, DEBUG
//	, TRACE, NOTSET 
	};

	/**
	 * @class LoggingWrapper
	 * @brief LoggingWrapper is a Wrapper for simple use of log4cxx
	 */
	class LoggingWrapper 
	{
	public:

                static void configure(Priority priority);

		/**
		 * Sets up the logger
		 * @param classname Classname that identifies the current logger (where the logger logs)
		 */
		LoggingWrapper(const std::string& classname);		

		virtual ~LoggingWrapper();

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
		log4cpp::Category* logger_;
                static bool configured;
                static log4cpp::Category* rootCategory;

	};

} // end namespace

#endif /* LOGGINGWRAPPER_H_ */

