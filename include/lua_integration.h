#include <selene.h>

class LuaManager {
	static LuaManager instance;

	sel::State state;
	
	LuaManager();
	~LuaManager();

public:

	static LuaManager& get() {
		return instance;
	}
};