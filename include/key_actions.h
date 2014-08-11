#pragma once

#include "main.h"

#include <map>

class KeyActions {
	static KeyActions instance;

	using IntStrMap = std::map<int, std::string>;
	IntStrMap keyBindingMap;
	IntStrMap buttonBindingMap[4];

	#define ACTION(_name, _action) \
	void _name();
 	#include "actions.def"
	#undef ACTION
	
	void performAction(const char* name);
	void load(const string &fn);

public:
	static KeyActions& get() {
		return instance;
	}

	KeyActions() {}
	
	void load();
	void report();

	void processIO();
};
