#pragma once

#include "main.h"

#include <map>
#include <tuple>

class KeyActions {
	static KeyActions instance;

	struct ActionBinding {
		bool downLastFrame;
		bool ctrl, alt, shift;
		std::string action;
	};

	using IntBindingMap = std::multimap<int, ActionBinding>;
	IntBindingMap keyBindingMap;
	using IntStrMap = std::multimap<int, string>;
	IntStrMap buttonBindingMap[4];

	#define ACTION(_name, _action) \
	void _name();
 	#include "actions.def"
	#undef ACTION
	
	void performAction(const char* name);
	void load(const string &fn);
	bool loadBinding(const char* keyName, int keyVal, const string& bstring);

public:
	static KeyActions& get() {
		return instance;
	}

	KeyActions() {}
	
	void load();
	void report();

	void processIO();
};
