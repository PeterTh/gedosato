
#include "game_plugin.h"

#include "plugins/generic.h"
#include "plugins/generic_depth.h"
#include "plugins/dark_souls_2.h"
#include "plugins/mitsurugi.h"

#include "string_utils.h"

GamePlugin* GamePlugin::getPlugin(IDirect3DDevice9 *device, RSManager &manager) {
	#define PLUGIN(_name, _exename, _class) \
	if(matchWildcard(getExeFileName(), _exename)) { \
		_class* ret = new _class(device, manager); \
		ret->name = _name; \
		return ret; \
	}
	#include "plugins.def"
	return new GamePlugin(device, manager);
}
