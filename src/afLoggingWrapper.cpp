/*
 * @file afLoggingWrapper.cpp
 *
 * @author Marlon Meyer, mmeyer@tzi.de
 * @author Thomas Roehr, thomas.roehr@dfki.de
 * @author Darko Makreshanski, darko.makreshanski@dfki.de
 *
 * @brief A logging wrapper that currently used log4cxx and allows remote logging to the 
 * control center when the asyncerror process is set
 */

#include "afLoggingWrapper.h"
#include "afConfigurator.h"
#include <iostream>

namespace dfki { namespace communication {


	afLoggingWrapper::afLoggingWrapper(std::string classname) : classname_(classname)
	{

		logger_ = log4cxx::Logger::getLogger(classname.c_str());

		if (!(afConfigurator::getInstance()->isConfigured())) {
			afConfigurator::getInstance()->configureDefaultLoggingProperties();
		}

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
	}



}} // end namespace 
