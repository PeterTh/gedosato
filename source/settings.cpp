#include "settings.h"

#include <fstream>
#include <string>
#include <regex>
#include <boost/filesystem.hpp>

#include "main.h"
#include "window_manager.h"

#define SETTINGS_FILE_NAME (INTERCEPTOR_NAME".ini")
#define USER_SETTINGS_FILE_NAME (INTERCEPTOR_NAME"_user.ini")

void ResolutionSettings::readResolution(const char* source) {
	string sstring(source);

	std::cmatch results;
	bool success = regex_match(source, results, std::regex(R"(\s*(\d+)\s*x\s*(\d+)\s*@\s*(\d+)\s*)"));
	if(success) {
		resolutions.emplace_back(std::atoi(results[1].str().c_str()), std::atoi(results[2].str().c_str()), std::atoi(results[3].str().c_str()));
	}
	else {
		SDLOG(-1, "ERROR: ill-formatted resolution string: %s\n", source);
	}
}

bool ResolutionSettings::setDSRes(const unsigned width, const unsigned height) {
	unsigned index = 0;
	for(const auto& res : resolutions) {
		if(res.width == width && res.height == height) {
			activeRes = index;
			return true;
		}
		index++;
	}
	return false;
}

const ResolutionSettings::Resolution& ResolutionSettings::getResolution(int n) const {
	if(static_cast<unsigned>(n) >= getNumResolutions()) n = activeRes;
	return resolutions[n];
}

void ResolutionSettings::clearResolutions() {
	resolutions.clear();
}

// Settings --------------------------------------------------------------------

Settings Settings::instance;

void Settings::clearAll() {
	#define SETTING(_type, _var, _inistring, _defaultval) \
		_var = _defaultval;
	#include "Settings.def"
	#undef SETTING
}

void Settings::load(const string &fn) {
	if(!boost::filesystem::exists(fn)) return;
	std::ifstream sfile;
	sfile.open(fn, std::ios::in);
	while(!sfile.eof()) {
		string bstring;
		std::getline(sfile, bstring);
		if(bstring[0] == '#') continue;
		if(bstring.size() <= 1) continue;

		if(bstring.find("renderResolution") == 0) {
			resSettings.readResolution(bstring.substr(strlen("renderResolution") + 1).c_str());
		}
		if(bstring.find("clearRenderResolutions") == 0) {
			resSettings.clearResolutions();
		}
		if(bstring.find("addProcessingPass") == 0) {
			processingPasses.push_back(bstring.substr(strlen("addProcessingPass") + 1));
		}

		#define SETTING(_type, _var, _inistring, _defaultval) \
		if(bstring.find(_inistring) == 0) { \
			read(bstring.substr(strlen(_inistring) + 1).c_str(), _var); \
		}
		#include "Settings.def"
		#undef SETTING
	}
	sfile.close();

	// find native monitor resolution
	int width = 1920, height = 1080;
	const POINT ptZero = { 0, 0 };
	auto monitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	auto success = GetMonitorInfo(monitor, &info);
	if(!success) {
		SDLOG(-1, "ERROR: GetMonitorInfo failed!\n");
	}
	else {
		width = info.rcMonitor.right - info.rcMonitor.left;
		height = info.rcMonitor.bottom - info.rcMonitor.top;
	}
	if(getPresentWidth() == 0) PresentWidth = width;
	if(getPresentHeight() == 0) PresentHeight = height;

	baseLogLevel = LogLevel;
}

void Settings::load() {
	clearAll();
	resSettings.clearResolutions();
	processingPasses.clear();
	SDLOG(1, "Loading general settings.\n");
	load(getConfigFileName(SETTINGS_FILE_NAME));
	SDLOG(1, "Loading user settings.\n");
	load(getConfigFileName(USER_SETTINGS_FILE_NAME));
	SDLOG(1, "Loading game-specific settings.\n");
	load(getConfigFileName(getExeFileName() + "\\" + SETTINGS_FILE_NAME));
	SDLOG(1, "Loading game-specific user settings.\n");
	load(getConfigFileName(getExeFileName() + "\\" + USER_SETTINGS_FILE_NAME));
	SDLOG(1, "All settings loaded.\n");
	if(resSettings.getNumResolutions() == 0) {
		resSettings.readResolution("3840x2160@60");
		SDLOG(-1, "WARNING: no rendering resolutions specified in ini\n")
	}
}

void Settings::report() {
	SDLOG(0, "= Settings read:\n");
	#define SETTING(_type, _var, _inistring, _defaultval) \
	log(_inistring, _var);
	#include "Settings.def"
	#undef SETTING
	SDLOG(0, "=============\n");
}

void Settings::init() {
	if(!inited) {
		inited = true;
	}
}

void Settings::shutdown() {
	if(inited) {
		inited = false;
	}
}

void Settings::elevateLogLevel(unsigned level) {
	LogLevel = level;
}

void Settings::restoreLogLevel() {
	LogLevel = baseLogLevel;
}


// reading --------------------------------------------------------------------

void Settings::read(const char* source, bool& value) {
	std::string ss(source);
	if(ss.find("true")==0 || ss.find("1")==0 || ss.find("TRUE")==0 || ss.find("enable")==0) value = true;
	else value = false;
}

void Settings::read(const char* source, int& value) {
	sscanf_s(source, "%d", &value);
}

void Settings::read(const char* source, unsigned& value) {
	sscanf_s(source, "%u", &value);
}

void Settings::read(const char* source, float& value) {
	sscanf_s(source, "%f", &value);
}

void Settings::read(const char* source, std::string& value) {
	value.assign(source);
	boost::algorithm::trim(value);
}

void Settings::read(const char* source, std::vector<std::string>& value) {
	string src {source};	
	boost::algorithm::trim(src);
	value.push_back(src);
}

// logging --------------------------------------------------------------------

void Settings::log(const char* name, bool value) {
	SDLOG(0, " - %s : %s\n", name, value ? "true" : "false");
}

void Settings::log(const char* name, int value) {
	SDLOG(0, " - %s : %d\n", name, value);
}

void Settings::log(const char* name, unsigned value) {
	SDLOG(0, " - %s : %u\n", name, value);
}

void Settings::log(const char* name, float value) {
	SDLOG(0, " - %s : %f\n", name, value);
}

void Settings::log(const char* name, const std::string& value) {
	SDLOG(0, " - %s : %s\n", name, value.c_str());
}

void Settings::log(const char* name, const std::vector<std::string>& value) {
	SDLOG(0, " - %s : %s\n", name, boost::algorithm::join(value, ", "));
}
