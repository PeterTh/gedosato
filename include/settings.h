#pragma once

#include <string>
#include <vector>

#include "main.h"

class ResolutionSettings {
public:
	struct Resolution {
		unsigned width, height, hz;
		Resolution(unsigned w, unsigned h, unsigned z) : width(w), height(h), hz(z) {}
	};

	void readResolution(char* source);

	size_t getNumResolutions() const { return resolutions.size(); }
	const Resolution& getResolution(int n) const;

	unsigned getActiveWidth() const { return resolutions[activeRes].width; }
	unsigned getActiveHeight() const { return resolutions[activeRes].height; }
	unsigned getActiveHz() const { return resolutions[activeRes].hz; }

	bool setDSRes(const unsigned width, const unsigned height);

private:
	std::vector<Resolution> resolutions;
	unsigned activeRes = 0;
};

class Settings {
	static Settings instance;
	
	ResolutionSettings resSettings;
	bool inited;
	unsigned baseLogLevel;

	void read(char* source, bool& value);
	void read(char* source, int& value);
	void read(char* source, unsigned& value);
	void read(char* source, float& value);
	void read(char* source, std::string& value);
	
	void log(const char* name, bool value);
	void log(const char* name, int value);
	void log(const char* name, unsigned value);
	void log(const char* name, float value);
	void log(const char* name, const std::string& value);

	#define SETTING(_type, _var, _inistring, _defaultval) \
	private: _type _var; \
	public: _type get##_var() const { return _var; };
	#include "settings.def"
	#undef SETTING

	void load(const string& fn);

public:
	static Settings& get() {
		return instance;
	}
	static ResolutionSettings& getResSettings() {
		return get().resSettings;
	}
	
	void load();
	
	void report();
	void init();
	void shutdown();
	
	Settings() : inited(false) {
		#define SETTING(_type, _var, _inistring, _defaultval) \
		_var = _defaultval;
		#include "Settings.def"
		#undef SETTING
	}

	void elevateLogLevel(unsigned level);
	void restoreLogLevel();

	unsigned getRenderWidth() { return resSettings.getActiveWidth(); }
	unsigned getRenderHeight() { return resSettings.getActiveHeight(); }
};

