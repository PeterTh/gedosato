#include "settings.h"

#include <fstream>
#include <string>
#include <regex>
#include <boost/filesystem.hpp>

#include "main.h"
#include "window_manager.h"


void ResolutionSettings::readResolution(char* source) {
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

// Settings --------------------------------------------------------------------

Settings Settings::instance;

void Settings::load(const string &fn) {
	if(!boost::filesystem::exists(fn)) return;
	std::ifstream sfile;
	sfile.open(fn, std::ios::in);
	char buffer[128];
	while(!sfile.eof()) {
		sfile.getline(buffer, 128);
		if(buffer[0] == '#') continue;
		if(sfile.gcount() <= 1) continue;
		std::string bstring(buffer);

		if(bstring.find("renderResolution") == 0) {
			resSettings.readResolution(buffer + strlen("renderResolution") + 1);
		}

		#define SETTING(_type, _var, _inistring, _defaultval) \
		if(bstring.find(_inistring) == 0) { \
			read(buffer + strlen(_inistring) + 1, _var); \
		}
		#include "Settings.def"
		#undef SETTING
	}
	sfile.close();

	if(getPresentWidth() == 0) PresentWidth = getRenderWidth();
	if(getPresentHeight() == 0) PresentHeight = getRenderHeight();

	baseLogLevel = LogLevel;
}

void Settings::load() {
	SDLOG(1, "Loading general settings.\n");
	load(getConfigFileName(SETTINGS_FILE_NAME));
	SDLOG(1, "Loading game-specific settings.\n");
	load(getConfigFileName(getExeFileName() + "\\" + SETTINGS_FILE_NAME));
	SDLOG(1, "All settings loaded.\n");
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

void Settings::read(char* source, bool& value) {
	std::string ss(source);
	if(ss.find("true")==0 || ss.find("1")==0 || ss.find("TRUE")==0 || ss.find("enable")==0) value = true;
	else value = false;
}

void Settings::read(char* source, int& value) {
	sscanf_s(source, "%d", &value);
}

void Settings::read(char* source, unsigned& value) {
	sscanf_s(source, "%u", &value);
}

void Settings::read(char* source, float& value) {
	sscanf_s(source, "%f", &value);
}

void Settings::read(char* source, std::string& value) {
	value.assign(source);
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
