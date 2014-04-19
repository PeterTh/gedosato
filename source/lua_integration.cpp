#include "lua_integration.h"

#include <selene.h>

#include "main.h"
#include "settings.h"

namespace {
	void lualog(int level, string str) {
		SDLOG(level, "LUA: %s\n", str.c_str());
	}
}

LuaManager LuaManager::instance;

LuaManager::LuaManager() : state(true), inited(false)
{
	state["gdst_log"] = &lualog;
}

LuaManager::~LuaManager() 
{
}

void LuaManager::init()
{
	if (!inited) {
		inited = true;
		SDLOG(0, "LuaManger: initializing\n");
		state.Load(getInstalledFileName("lua/global.lua"));
		SDLOG(0, "LuaManger: initialization complete\n");
	}
	else {
		SDLOG(0, "ERROR: multiple calls to LuaManager::init\n");
	}
}

