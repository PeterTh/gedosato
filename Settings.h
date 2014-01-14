#pragma once

#include <string>
#include <Windows.h>

class Settings {
	static Settings instance;
	
	bool inited, langOverridden;
	unsigned curFPSlimit, baseLogLevel;

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
	#include "Settings.def"
	#undef SETTING

public:
	static Settings& get() {
		return instance;
	}
	
	void load();
	
	void report();
	void init();
	void shutdown();
	
	Settings() : inited(false), langOverridden(false) {
		#define SETTING(_type, _var, _inistring, _defaultval) \
		_var = _defaultval;
		#include "Settings.def"
		#undef SETTING
	}

	void elevateLogLevel(unsigned level);
	void restoreLogLevel();
};

