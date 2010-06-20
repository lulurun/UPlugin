/*
 * Logger.h
 *
 *  Created on: Apr 16, 2009
 *      Author: lulu
 */

#ifndef UPlugin_LOGGER_H_
#define UPlugin_LOGGER_H_

#include <string>
#include "Poco/Logger.h"
#include "Poco/SharedPtr.h"
#include "Poco/LogStream.h"
#include "UPlugin/npUPlugin.h"

namespace UPlugin {

// TODO @@@ how to manage multiple _loggers
	class UPlugin_API Logger {
public:
	static Poco::SharedPtr<Poco::LogStream> &GetLogStream(const std::string &name = "");
	static void Initialize();
};

#ifndef WIN32
#define TRACE_LOG(message)	\
	{ UPlugin::Logger::GetLogStream()->trace() << __PRETTY_FUNCTION__ << " " << message << std::endl; }
#else
#define TRACE_LOG(...) \
	{ UPlugin::Logger::GetLogStream()->trace() << __VA_ARGS__ << std::endl; }
#endif

#define DEBUG_LOG(...)	\
	{ UPlugin::Logger::GetLogStream()->debug() << __VA_ARGS__ << std::endl; }
#define INFO_LOG(...)	\
	{ UPlugin::Logger::GetLogStream()->information() << __VA_ARGS__ << std::endl; }
#define WARN_LOG(...)	\
	{ UPlugin::Logger::GetLogStream()->warning() << __VA_ARGS__ << std::endl; }
#define ERROR_LOG(...)	\
	{ UPlugin::Logger::GetLogStream()->error() << __VA_ARGS__ << std::endl; }
#define FATAL_LOG(...)	\
	{ UPlugin::Logger::GetLogStream()->fatal() << __VA_ARGS__ << std::endl; }

}

#endif /* UPlugin_LOGGER_H_ */
