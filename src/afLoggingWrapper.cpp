/*
 * @file afLoggingWrapper.cpp
 *
 * @author Marlon Meyer, mmeyer@tzi.de
 * @author Thomas Roehr, thomas.roehr@dfki.de
 *
 * @brief A logging wrapper that currently used log4cxx and allows remote logging to the 
 * control center when the asyncerror process is set
 */

#include "afLoggingWrapper.h"
#include <iostream>

namespace dfki { namespace communication {


	afLoggingWrapper::afLoggingWrapper(std::string classname) : classname_(classname)
	{
//		#ifdef LOGGER_EXTERNAL_		
		logger_ = log4cxx::Logger::getLogger(classname.c_str());
//		#endif
	
//		afConfiguration* configuration = afConfiguration::getInstance();
		
//		isLocalLoggingActive = scoutConfiguration->localLogging;
//		isRemoteLoggingActive = scoutConfiguration->remoteLogging;
		
//		#ifdef LOGGER_EXTERNAL_	
//		std::cout << "Loading configuration file from " << configuration->getConfig().loggingConfigFile << std::endl;	
//		if (afConfiguration::getInstance()->getLoggingConfigFile() != "") {
//			std::cout << "Loading configuration file from " << afConfiguration::getInstance()->getLoggingConfigFile() << std::endl;	
//	        log4cxx::PropertyConfigurator::configure(afConfiguration::getInstance()->getLoggingConfigFile().c_str());
//	    } else {
//	    	//TODO load default configuration
//	    }
//		#endif
	}

	afLoggingWrapper::~afLoggingWrapper()
	{
	}
	
	void afLoggingWrapper::log(Priority priority, const char* format, ...)
	{
		int n = 100;	
		char buffer[512];
		va_list arguments;

		va_start(arguments, format);
		n = vsnprintf(buffer, sizeof(buffer), format, arguments);
		va_end(arguments);

		log(priority, std::string(buffer));
	}
	
	void afLoggingWrapper::log(Priority priority, std::string message)
	{
		// Class name need only to be appended to async error logger
		// LOG4CXX will be able to log the classname if required (see PatternLayout)
		std::string source = classname_;
		source.append(" : ").append(message);	
		
		//local logging
//		if(isLocalLoggingActive)
//		{
//			#ifdef LOGGER_EXTERNAL_
			switch(priority)
			{
				
		//		case TRACE: 	LOG4CXX_TRACE(logger_, source.toStdString());	break;

				case DEBUG:	LOG4CXX_DEBUG(logger_, message);
												break;
				case INFO:	LOG4CXX_INFO(logger_, message);
												break;
				case WARN:	LOG4CXX_WARN(logger_, message);
												break;
				case ERROR:	LOG4CXX_ERROR(logger_, message);
												break;
				case FATAL:	LOG4CXX_FATAL(logger_, message);
												break;
				default:						break;
			}
//			#endif
//		}

		//remote logging
//		if(isRemoteLoggingActive)

//		{
//			QString content("#LOG [");

//			//note that debug messages will not be send
//			if(priority <= ERROR) content.append("ERROR,");
//			else if(priority <= WARN) content.append("WARN,");
//			else if(priority <= INFO) content.append("INFO,");
//			else
//				return; // do not log other info

//			content.append("SS_SCT_AEM,");

//			//Datum und Zeit
//			QDate date = QDate::currentDate();
//			content.append(date.toString("MMM dd yyyy"));
//			content.append(" ");
//			QTime time = QTime::currentTime();
//			content.append(time.toString(QString("HH:mm:ss"))+",");
//			content.append(",,] ");//fields d1,d2,d3 are empty
//			content.append(source); 

//			//TODO: Welche source bzw. destination hat die log Meldung?
//			Lunares::Message msg(ScoutMessageQueues::Incoming::NominalCmd, ScoutMessageQueues::Outgoing::AsyncError, content);

//			// Check if handler is active
//			if(asyncErrorHandler_ != 0)
//			{
//				asyncErrorHandler_->handleMessage(msg);
//			}
//		}
	}

//	void LoggingWrapper::activateLocalLogging()
//	{
//		isLocalLoggingActive = true;
//	}

//	void LoggingWrapper::deactivateLocalLogging()
//	{
//		isLocalLoggingActive = false;
//	}

//	void LoggingWrapper::activateRemoteLogging()
//	{
//		isRemoteLoggingActive = true;
//	}

//	void LoggingWrapper::deactivateRemoteLogging()
//	{
//		isRemoteLoggingActive = false;
//	}

//	void LoggingWrapper::setAsyncErrorHandler(IScoutSubSystemProcess* process)
//	{
//		asyncErrorHandler_ = process;
//	}

}} // end namespace 
