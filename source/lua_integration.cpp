#include "lua_integration.h"

#include "main.h"
#include "settings.h"


namespace {
	void lualog(int level, string str) {
		SDLOG(level, str.c_str());
	}
}

LuaManager::LuaManager() : state{ true }
{
	state["gdst_log"] = &lualog;
	state.Load(getInstalledFileName("lua/global.lua"));
}

