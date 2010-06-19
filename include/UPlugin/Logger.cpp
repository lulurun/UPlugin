#include <map>
#include "Poco/File.h"
#include "Poco/Message.h"
#include "Poco/FileStream.h"
#include "Poco/Path.h"
#include "Poco/Util/LoggingConfigurator.h"
#include "Poco/Util/PropertyFileConfiguration.h"

#include "UPlugin/Env.h"
#include "UPlugin/Logger.h"

namespace UPlugin {

static std::string DEFAULT_CONFIG_FILE = "UPlugin.logger.properties";
static std::string DEFAULT_LOG_FILE = "UPlugin.log";

Poco::SharedPtr<Poco::LogStream> &Logger::GetLogStream(const std::string &name) {
	static Poco::SharedPtr<Poco::LogStream> pLogStream;
	if (pLogStream.get() == NULL) {
		pLogStream = new Poco::LogStream(Poco::Logger::root());
	}
	// TODO @@@ implement get("name")
    return pLogStream;
}

static void strReplace (std::string& str, const std::string& from, const std::string& to) {
    std::string::size_type pos = 0;
    while(pos = str.find(from, pos), pos != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

void Logger::Initialize() {
	try {
		Poco::Util::LoggingConfigurator configurator;

		Poco::Path user_dir_path(Env::GetInstance()->getDataDirectory());
		Poco::Path logger_config_file(user_dir_path, DEFAULT_CONFIG_FILE);
		Poco::Path log_file(user_dir_path, DEFAULT_LOG_FILE);
		std::string log_file_path = log_file.toString();
		strReplace(log_file_path, "\\", "\\\\");

		std::string DEFAULT_CONFIG_STRING = "";
		DEFAULT_CONFIG_STRING += "logging.loggers.root.channel = c0\n";
		DEFAULT_CONFIG_STRING += "logging.loggers.root.level = debug\n";
		DEFAULT_CONFIG_STRING += "logging.formatters.f1.class = PatternFormatter\n";
		DEFAULT_CONFIG_STRING += "logging.formatters.f1.pattern = [%q] %Y/%m/%d %H:%M:%S - %t\n";
		DEFAULT_CONFIG_STRING += "logging.channels.c0.class = SplitterChannel\n";
		DEFAULT_CONFIG_STRING += "logging.channels.c0.channel1 = c1\n";
		DEFAULT_CONFIG_STRING += "logging.channels.c0.channel2 = c2\n";
		DEFAULT_CONFIG_STRING += "logging.channels.c1.class = ConsoleChannel\n";
		DEFAULT_CONFIG_STRING += "logging.channels.c1.formatter = f1\n";
		DEFAULT_CONFIG_STRING += "logging.channels.c2.class = FileChannel\n";
		DEFAULT_CONFIG_STRING += "logging.channels.c2.path = ";
		DEFAULT_CONFIG_STRING += log_file_path + "\n";
		DEFAULT_CONFIG_STRING += "logging.channels.c2.formatter = f1\n";

		Poco::File config_file(logger_config_file);
		if (!config_file.exists())
		{
			Poco::FileOutputStream fos(logger_config_file.toString());
			fos << DEFAULT_CONFIG_STRING;
			fos.close();
		}
		configurator.configure(new Poco::Util::PropertyFileConfiguration(logger_config_file.toString()));

		//Logger::pLogStream.assign(new Poco::LogStream(Poco::Logger::root()));
	} catch (Poco::Exception &ex) {
		//throw ex;// TODO @@@ ...
	}
};

}

