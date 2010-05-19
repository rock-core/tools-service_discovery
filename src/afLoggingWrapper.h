/*
 * @file LoggingWrapper.h
 * @author Marlon Meyer, mmeyer@tzi
 * @author Thomas Roehr, thomas.roehr@dfki.de
 *
 * @brief Logging class currently based on log4cpp -- future logging should take advantage of log4cxx, which is closer to the 
 * log4j implementation and more current
 */

#ifndef LOGGINGWRAPPER_H_
#define LOGGINGWRAPPER_H_

#undef debug
#undef error
#undef info

//#ifdef LOGGER_EXTERNAL_		
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
//#endif

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <typeinfo>


//#include "ScoutConfiguration.h"
//#include "ScoutMessageQueues.h"
//#include "IScoutSubSystemProcess.h"



using namespace std;

namespace dfki { namespace communication {

	enum Priority	{ FATAL, ERROR, WARN, INFO, DEBUG, TRACE, NOTSET };

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

//		/**
//		 * Activates the local logging
//		 */
//		void activateLocalLogging();
//		/**
//		 * Deactivates the local logging
//		 */
//		void deactivateLocalLogging();

		/**
		 * specified where to be logged locally. Note that local logging has to be activated.
		 */
		void setLocalOutputs(bool file, bool console);

//		/**
//		 * Activates the remote logging
//		 */
//		void activateRemoteLogging();
//		/**
//		 * Deactivates the remote logging
//		 */
//		void deactivateRemoteLogging();
		
//		/**
// 		* Set the asynchronous error handler, so that error message can 
// 		* be forwarded to the control center
// 		* @param process
// 		*/
//		void setAsyncErrorHandler(IScoutSubSystemProcess* process);

	private:
		std::string classname_;

//		bool isLocalLoggingActive;
//		bool isRemoteLoggingActive;
		
//		#ifdef LOGGER_EXTERNAL_
		log4cxx::LoggerPtr logger_;
//		#endif

//		static Lunares::IScoutSubSystemProcess* asyncErrorHandler_;
	};

}} // end namespace

#endif /* LOGGINGWRAPPER_H_ */

