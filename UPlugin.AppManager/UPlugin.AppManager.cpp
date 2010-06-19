#include <memory>
#include <istream>
#include <ostream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include "UPlugin.AppManager.h"
#include "UPlugin/Env.h"
#include "UPlugin/Logger.h"
#include "Poco/Exception.h"
#include "Poco/Format.h"
#include "Poco/Timestamp.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/File.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/BinaryReader.h"
#include "Poco/InflatingStream.h"
#include "Poco/DeflatingStream.h"
#include "Poco/MemoryStream.h"

namespace UPlugin {

static char* np_strdup(const char *s) {
	if ((const char *) 0 == s ) s = "";
	size_t n = strlen(s) + 1;
	char *rv = (char *) Util::MemAlloc(n);
	if ((char *) 0 != rv ) memcpy(rv, s, n);
	return rv;
}

AppManagerPlugin::AppManagerPlugin()
	:ScriptablePluginObject(), m_state(DEFAULT), m_thread(NULL)
{
}

AppManagerPlugin::AppManagerPlugin(NPP npp)
	:ScriptablePluginObject(npp), m_state(DEFAULT), m_thread(NULL)
{
	m_identifier = "AppManager";

	register_method("Install", this, &AppManagerPlugin::Install);
	register_method("Uninstall", this, &AppManagerPlugin::Uninstall);
	register_method("GetStatus", this, &AppManagerPlugin::GetStatus);
	register_method("GetAppList", this, &AppManagerPlugin::GetAppList);
}

AppManagerPlugin::~AppManagerPlugin()
{
	if (m_thread != NULL) {
		delete m_thread;
	}
}

class AppInfo {
public:
	std::string name;
	std::string dir;
	std::string ver;
	std::string key_file;
	time_t last_upd;

	AppInfo():last_upd(0) {}
	static const std::string META_FILE;

	AppInfo(const std::string &name, const std::string &dir, const std::string &ver, const std::string &key_file, int last_upd=0) {
		this->name = name;
		this->dir = dir;
		this->ver = ver;
		this->key_file = key_file;
		this->last_upd = last_upd;
	}

	void save() {
		Poco::Path p(this->dir);
		if (p.isDirectory()) {
			p.append(META_FILE);
			std::ofstream ofs(p.toString().c_str());
			ofs << this->ver << std::endl;
			ofs << this->key_file << std::endl;
			Poco::File f(p);
			this->last_upd = f.created().epochTime();
		} else {
			throw Poco::Exception(Poco::format("can not find install dir of %s", this->name));
		}
	}

	std::string toJson() {
		std::string ret = "{";
		ret.append(Poco::format("name:'%s',", this->name));
		ret.append(Poco::format("dir:'%s',", this->dir));
		ret.append(Poco::format("ver:'%s',", this->ver));
		ret.append(Poco::format("key:'%s',", this->key_file));
		ret.append(Poco::format("upd:'%d'", this->last_upd));
		ret.append("}");
		return ret;
	}

	static AppInfo Load(const std::string &name, const Poco::Path &dir_path) {
		TRACE_LOG("AppINFO::Load enter");
		AppInfo ret;
		Poco::Path p = dir_path;
		p.append(META_FILE);
		Poco::File f(p);
		if(f.exists()) {
			std::ifstream ifs(p.toString().c_str());
			if (!getline(ifs, ret.ver)) {
				WARN_LOG("can not get version for " << name);
				ret.ver = "0.0.0.0";
			}
			if (!getline(ifs, ret.key_file)) {
				WARN_LOG("can not get key_file for " << name);
				ret.key_file = "";
			}
			ret.last_upd =  f.created().epochTime();
			ret.name = name;
			ret.dir = dir_path.toString();
		} else {
			throw Poco::Exception(Poco::format("can not find meta file of %s", name));
		}
		TRACE_LOG("AppINFO::Load leave");
		return ret;
	}
};

const std::string AppInfo::META_FILE = ".meta";

class ArchiveReader {
private:
	class Header {
	public:
		enum LinkIndicatorEnum {
			Default = -1,
			NormalFile = 0,
			HardLink,
			SymbolicLink,
			CharacterSpecial,
			BlockSpecial,
			Directory,
			FIFO
		};
		std::string FilePath;
		int FileSize;
		LinkIndicatorEnum LinkIndicator;
		Header():FileSize(0), LinkIndicator(Header::Default) {}
	};

	static std::string trim(const std::string &s) {
		std::string::const_iterator left = std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun(isspace)));
		std::string::const_reverse_iterator right = std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun(isspace)));
		return std::string(left, right.base());
	}

	static int oct2dec(char *bytes, int start, int count) {
		int d = 0;
		int base = 1;
		for(int i=count-1; i>0; i--) {
			d |= (bytes[start+i] - '0') * base;
			base <<= 3;
		}
		return d;
	}

	static bool ReadHeader(std::stringstream &stream, Header &header) {
		char bytes[512];
		int gcount = stream.read(bytes, 512).gcount();
		std::cout << gcount << std::endl;
		if (gcount == 0 || bytes[0] == 0) {
			// End of file
			return false;
		}
		std::string _str(bytes);
		header.FilePath = trim(_str);
		// @@@ ref. http://en.wikipedia.org/wiki/Tar_(file_format)
		header.FileSize = oct2dec(bytes, 124, 11);
		if (header.FileSize < 0)
			return false;
		header.LinkIndicator = static_cast<Header::LinkIndicatorEnum>(bytes[156] - '0');
		return true;
	}

	static bool ReadEntry(std::stringstream &stream, std::string &file_path, Header::LinkIndicatorEnum &indicator, std::vector<char> &data) {
		try {
			file_path = "";
			indicator = Header::NormalFile;
			Header header;
			if(!ReadHeader(stream, header)) return false;

			file_path = header.FilePath;
			indicator = header.LinkIndicator;
			if (header.FileSize == 0) return true;
			data.resize(header.FileSize);
			stream.read(&data[0], header.FileSize);
			// Read the rest of the empty padding in the 512 byte block
			if (header.FileSize % 512 != 0)
			{
				int paddingLeft = 512 - (header.FileSize % 512);
				char padding[512];
				stream.read(padding, paddingLeft);
			}
			return true;
		} catch (std::exception &e) {
			ERROR_LOG("Error read entry: " << e.what());
			return false;
		}
	}
public:

	static void ExtractPKG(std::istream &istr, const std::string &out_dir) {
		INFO_LOG("ExtractPKG " << out_dir);
		Poco::File f(out_dir);
		if (!f.exists()) f.createDirectories();
		Poco::Path base_path(out_dir);

		std::stringstream ss;
		Poco::InflatingOutputStream inflater(ss, Poco::InflatingStreamBuf::STREAM_GZIP);
		Poco::StreamCopier::copyStream(istr, inflater);
		inflater.close();

		std::string path;
		Header::LinkIndicatorEnum indicator;
		std::vector<char> data;
		while (ArchiveReader::ReadEntry(ss, path, indicator, data))
		{
			Poco::Path abs_path(out_dir);
			Poco::Path p(path);
			abs_path.append(p);
			switch (indicator)
			{
			case Header::NormalFile:
				{
					if (data.size() > 0) {
						std::ofstream ofs;
						ofs.open(abs_path.toString().c_str(), std::ios::out | std::ios::binary);
						ofs.write(&data[0], data.size());
					}
				}
				break;
			case Header::Directory:
				{
					Poco::File f(abs_path);
					if (!f.exists()) f.createDirectories();
				}
				break;
			default:
				break;
			}
		}
	}
};

struct InstallThread_Param {
	std::string name;
	std::string url;
	std::string version;
	std::string key_file;
	AppManagerPlugin &plugin;

	InstallThread_Param(const std::string &name, const std::string &url, const std::string &version, const std::string &key_file, AppManagerPlugin &plugin)
		: name(name), url(url), version(version), key_file(key_file), plugin(plugin) {}
};

void InstallThread_Handler(void *data) {
	TRACE_LOG("InstallThread_Handler enter");

	InstallThread_Param *param = reinterpret_cast<InstallThread_Param *>(data);

	// Download
	param->plugin.setState(AppManagerPlugin::DOWNLOAD_PKG);

	Poco::URI uri(param->url);
	Poco::Net::HTTPClientSession s(uri.getHost(), uri.getPort());
	Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathEtc());
	s.sendRequest(request);
	Poco::Net::HTTPResponse response;
	std::istream& rs = s.receiveResponse(response);

	// Unzip & Install
	param->plugin.setState(AppManagerPlugin::EXTRACT_PKG);

	Poco::Path inst_dir(Env::GetInstance()->getAppBaseDirectory());
	inst_dir.append(Poco::Path(param->name));
	Poco::Path temp_dir(Env::GetInstance()->getAppBaseDirectory());
	temp_dir.append(Poco::Path(param->name + "_tmp"));

	try {
		ArchiveReader::ExtractPKG(rs, temp_dir.toString());

		param->plugin.setState(AppManagerPlugin::INSTALL);
		// TODO @@@ stop/unload the app if running
		{
			Poco::File f(inst_dir);
			if (f.exists()) f.remove(true);
		}
		{
			Poco::File f(temp_dir);
			if (f.exists()) {
				f.moveTo(inst_dir.toString());
			}
		}
	} catch (Poco::Exception &e) {
		ERROR_LOG("install failed" << e.displayText());
		param->plugin.setState(AppManagerPlugin::_ERROR);
		// clean up
		Poco::File f(temp_dir);
		if (f.exists()) {
			f.remove(true);
		}
		return;
	}

	// finalize, write meta info
	inst_dir.append(AppInfo::META_FILE);
	std::ofstream ofs(inst_dir.toString().c_str());
	ofs << param->version << std::endl;
	ofs << param->key_file << std::endl;
	ofs.close();

	param->plugin.setState(AppManagerPlugin::FINISH);
	TRACE_LOG("InstallThread_Handler leave");
};

void AppManagerPlugin::setState(STATE s) {
	// TODO @@@ lock ??
	this->m_state = s;
}

bool AppManagerPlugin::Install(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	BOOLEAN_TO_NPVARIANT(false, *result);

	if (argCount < 2) return false;
	std::string name(args[0].value.stringValue.UTF8Characters);
	if (name.empty()) return false;
	TRACE_LOG("install " << name);

	std::string url(args[1].value.stringValue.UTF8Characters);
	if (url.empty()) return false;
	std::string key_file;
	if (argCount > 2) key_file = args[2].value.stringValue.UTF8Characters;
	std::string version;
	if (argCount > 3) version = args[3].value.stringValue.UTF8Characters;
	if (key_file.empty()) key_file = Env::GetUPluginAPPDLL(name);

	this->setState(AppManagerPlugin::CHK_VERSION);

	m_thread = new Poco::Thread(m_identifier.append("_").append(name));
	std::auto_ptr<InstallThread_Param> param(new InstallThread_Param(name, url, version, key_file, *this));
	m_thread->start(InstallThread_Handler, param.release());

	return true;
}

bool AppManagerPlugin::Uninstall(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	BOOLEAN_TO_NPVARIANT(false, *result);
	if (argCount != 1) return false;
	if (!NPVARIANT_IS_STRING(args[0])) return false;
	std::string app_name(args[0].value.stringValue.UTF8Characters);
	if (app_name.empty()) return false;
	INFO_LOG("uninstall " << app_name);

	try {
		Poco::Path p(Env::GetInstance()->getAppBaseDirectory());
		p.append(app_name);
		Poco::File f(p);
		if (f.exists()) {
			INFO_LOG("removing from " << p.toString());
			f.remove(true);
		} else {
			ERROR_LOG("can not find the installation of " << app_name);
		}
		BOOLEAN_TO_NPVARIANT(true, *result);
	} catch (Poco::Exception &e) {
		ERROR_LOG("uninstall Exception: " << e.displayText());
		return false;
	}
	return true;
}

bool AppManagerPlugin::GetStatus(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	INT32_TO_NPVARIANT(this->m_state, *result);
	return true;
}

bool AppManagerPlugin::GetAppList(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	TRACE_LOG("GetAppList enter");
	std::string app_list_json = "[";

	Poco::Path p(Env::GetInstance()->getAppBaseDirectory());
	Poco::DirectoryIterator it(p);
	Poco::DirectoryIterator	end;
	while(it != end) {
		try {
			AppInfo app_info = AppInfo::Load(it.name(), it.path());
			app_list_json.append(app_info.toJson());
			app_list_json.append(",");
		} catch (Poco::Exception &e) {
			ERROR_LOG("can not get installed list: " << e.displayText());
		}
		it++;
	}
	app_list_json.append("]");
	STRINGZ_TO_NPVARIANT(np_strdup(app_list_json.c_str()), *result);

	TRACE_LOG("GetAppList leave");

	return true;
}


void AppManagerPlugin::shutdown() {
}

}

