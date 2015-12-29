
#include "game_plugin.h"

#include "plugins/generic.h"
#include "plugins/generic_depth.h"
#include "plugins/dark_souls_2.h"
#include "plugins/mitsurugi.h"
#include "plugins/ff13.h"
#include "plugins/dw8.h"
#include "plugins/zestiria.h"
#include "plugins/lrff13.h"

#include "utils/string_utils.h"

GamePlugin* GamePlugin::getPlugin(IDirect3DDevice9 *device, RSManagerDX9 &manager) {
	#define PLUGIN(_name, _exename, _class) \
	if(Settings::get().getPluginOverride() == #_class) { \
		_class* ret = new _class(device, manager); \
		ret->name = _name; \
		return ret; \
	}
	#include "plugins.def"
	#undef PLUGIN
	#define PLUGIN(_name, _exename, _class) \
	if(matchWildcard(getExeFileName(), _exename)) { \
		_class* ret = new _class(device, manager); \
		ret->name = _name; \
		return ret; \
	}
	#include "plugins.def"
	#undef PLUGIN
	return new GamePlugin(device, manager);
}
