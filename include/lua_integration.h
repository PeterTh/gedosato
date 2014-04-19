#pragma once

#include <selene.h>

class LuaManager {
	static LuaManager instance;
	bool inited;

	sel::State state;
		
	LuaManager();
	~LuaManager();

public:

	static LuaManager& get() {
		return instance;
	}

	void init();

};