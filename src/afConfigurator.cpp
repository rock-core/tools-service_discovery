

#include "afConfigurator.h"

namespace dfki { namespace communication {

afConfigurator * afConfigurator::instance = NULL;

void afConfigurator::configureLoggingProperties(std::string str)
{
	log4cxx::PropertyConfigurator::configure(str.c_str());
	loggingConfigured = true;
}

void afConfigurator::configureDefaultLoggingProperties()
{
	log4cxx::helpers::Properties props;
// Good resource:  http://www.johnmunsch.com/projects/Presentations/docs/Log4J/log.properties 	
	props.put("log4j.rootLogger", "INFO, stmConsole");
	props.put("log4j.appender.stmConsole","org.apache.log4j.ConsoleAppender");
	props.put("log4j.appender.stmConsole.layout","org.apache.log4j.PatternLayout");
	props.put("log4j.appender.stmConsole.layout.ConversionPattern","%d [%t] %-5p %c - %m%n");
	log4cxx::PropertyConfigurator::configure(props);
}

afConfigurator* afConfigurator::getInstance()
{	
	if(instance == NULL)
	{
		instance = new afConfigurator();
	}
	
	return instance;
}

afConfigurator::afConfigurator()
{
	loggingConfigured = false;
}

afConfigurator::~afConfigurator()
{
	if(afConfigurator::instance != NULL)
	{
		delete afConfigurator::instance;
		afConfigurator::instance = NULL;
	}

}


}}


