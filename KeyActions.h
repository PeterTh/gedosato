#pragma once

#include <map>

class KeyActions {
	static KeyActions instance;
	
	typedef std::map<int, std::string> IntStrMap;
	IntStrMap keyBindingMap;

	#define ACTION(_name, _action) \
	void _name();
 	#include "Actions.def"
	#undef ACTION
	
	void performAction(const char* name);

public:
	static KeyActions& get() {
		return instance;
	}

	KeyActions() {}
	
	void load();
	void report();

	void processIO();
};
