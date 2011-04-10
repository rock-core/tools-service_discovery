/*
 * @file LoggingWrapper.cpp
 *
 * @author Marlon Meyer, mmeyer@tzi.de
 * @author Thomas Roehr, thomas.roehr@rock.de
 * @author Darko Makreshanski, darko.makreshanski@rock.de
 *
 * @brief A simple logging wrapper uses log4cpp
 */

#include <iostream>
#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PatternLayout.hh>
#include "logging.h"

namespace servicediscovery { 


bool LoggingWrapper::configured = false;

LoggingWrapper::LoggingWrapper(const std::string& classname) : classname_(classname)

{
        if(!configured)
            configure(INFO);

	logger_ = &log4cpp::Category::getInstance(classname);
}

LoggingWrapper::~LoggingWrapper()
{
}

void LoggingWrapper::configure(Priority priority)
{
    log4cpp::Priority::PriorityLevel level;
    switch(priority)
    {
        case INFO: level = log4cpp::Priority::INFO; break;
        case DEBUG: level = log4cpp::Priority::DEBUG; break;
        case WARN: level = log4cpp::Priority::WARN; break;
        case ERROR: level = log4cpp::Priority::ERROR; break;
        case FATAL: level = log4cpp::Priority::CRIT; break;
    }
    
    if(!configured) 
    {
        log4cpp::Category& rootCategory = log4cpp::Category::getInstance("");
        //log4cpp::BasicConfigurator::configure();

        log4cpp::Appender *appender = new log4cpp::OstreamAppender("console", &std::cout);
        log4cpp::PatternLayout* layout = new log4cpp::PatternLayout(); 
        layout->setConversionPattern("[%d] [%p] - %m %n");
        appender->setLayout(layout);

        rootCategory.setAppender(appender); 
        rootCategory.setPriority(level);
    }

    configured = true;
}

void LoggingWrapper::log(Priority priority, const char* format, ...)
{
	int n = 100;	
	char buffer[512];
	va_list arguments;

	va_start(arguments, format);
	n = vsnprintf(buffer, sizeof(buffer), format, arguments);
	va_end(arguments);

	log(priority, std::string(buffer));
}

void LoggingWrapper::log(Priority priority, std::string message)
{
	// Class name need only to be appended to async error logger
	// LOG4CXX will be able to log the classname if required (see PatternLayout)
	std::string source = classname_;
	source.append(" : ").append(message);	
	
	switch(priority)
	{
		
		case DEBUG:	logger_->debug(message);
										break;
		case INFO:	logger_->info(message);
										break;
		case WARN:	logger_->warn(message);
										break;
		case ERROR:	logger_->error(message);
										break;
		case FATAL:	logger_->crit(message);
										break;
		default:						break;
	}
}

} // end namespace 
