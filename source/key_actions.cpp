#include "key_actions.h"

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
using namespace std;
#include <boost/filesystem.hpp>

#include <XInput.h>

#include "main.h"
#include "window_manager.h"
#include "settings.h"
#include "renderstate_manager.h"
#include "scaling.h"

#define KEY_FILE_NAME (INTERCEPTOR_NAME"Keys.ini")
#define USER_KEY_FILE_NAME (INTERCEPTOR_NAME"Keys_user.ini")

KeyActions KeyActions::instance;

void KeyActions::load(const string &fn) {
	if(!boost::filesystem::exists(fn)) return;
	std::ifstream sfile;
	sfile.open(fn, std::ios::in);
	char buffer[256];
	while(!sfile.eof()) {
		sfile.getline(buffer, 256);
		if(buffer[0] == '#') continue;
		if(sfile.gcount() <= 1) continue;
		string bstring(buffer);

		// Keyboard
		size_t pos = bstring.npos;
		char postChar;
		#define KEY(_name, _val) \
		pos = bstring.find(#_name); \
		postChar = buffer[pos + strlen(#_name)]; \
		if(pos != bstring.npos && (postChar == '\r' || postChar == '\n' || postChar == ' ' || postChar == '\0')) { \
			string action; stringstream ss(bstring); ss >> action; \
			keyBindingMap.insert(make_pair(_val, action)); \
			continue; \
		}
		#include "Keys.def"
		#undef KEY

		// Xinput
		#define BUTTON(_name) { \
		std::cmatch results; \
		bool success = std::regex_search(buffer, results, std::regex("X(\\d)_" #_name)); \
		if(success) { \
			string action; stringstream ss(bstring); ss >> action; \
			buttonBindingMap[std::atoi(results[1].str().c_str())].insert(make_pair(XINPUT_GAMEPAD_##_name, action)); \
			continue; \
		} }
 		#include "xinput.def"
		#undef BUTTON
	}
	sfile.close();
}

void KeyActions::load() {
	SDLOG(1, "Loading general keybindings.\n");
	load(getConfigFileName(KEY_FILE_NAME));
	SDLOG(1, "Loading user keybindings.\n");
	load(getConfigFileName(USER_KEY_FILE_NAME));
	SDLOG(1, "Loading game-specific keybindings.\n");
	load(getConfigFileName(getExeFileName() + "\\" + KEY_FILE_NAME));
	SDLOG(1, "Loading game-specific user keybindings.\n");
	load(getConfigFileName(getExeFileName() + "\\" + USER_KEY_FILE_NAME));
	SDLOG(1, "All keybindings loaded.\n");
}

void KeyActions::report() {
	SDLOG(0, "= Loaded Keybindings:\n");
	for(const auto& elem : keyBindingMap) {
		SDLOG(0, " - %p => %s\n", elem.first, elem.second.c_str());
	}
	SDLOG(0, "= Loaded Button bindings:\n");
	for(int i = 0; i < 4; ++i) {
		for(const auto& elem : buttonBindingMap[i]) {
			SDLOG(0, " - %d : %p => %s\n", i, elem.first, elem.second.c_str());
		}
	}
	SDLOG(0, "=============\n");
	
	SDLOG(5, "= Possible Actions:\n");
	#define ACTION(_name, _action) \
	SDLOG(5, "%s, ", #_name);
 	#include "Actions.def"
	#undef ACTION
	SDLOG(5, "=============\n");
	
	SDLOG(5, "= Possible Keys:\n");
	#define KEY(_name, _val) \
	SDLOG(5, "%s, ", #_name);
 	#include "Keys.def"
	#undef KEY
	SDLOG(5, "=============\n");
}

void KeyActions::performAction(const char* name) {
	#define ACTION(_name, _action) \
	if(strcmp(#_name, name) == 0) _name();
 	#include "actions.def"
	#undef ACTION
}

void KeyActions::processIO() {
	// check if top level window owned by this process
	auto activeHandle = GetForegroundWindow();
	if(activeHandle == NULL) {
		return;
	}
	auto procId = GetCurrentProcessId();
	DWORD activeProcId;
	GetWindowThreadProcessId(activeHandle, &activeProcId);
	if(activeProcId != procId) return;

	// keyboard
	for(IntStrMap::const_iterator i = keyBindingMap.begin(); i != keyBindingMap.end(); ++i) {
		if(GetAsyncKeyState(i->first)&1) {
			SDLOG(0, "Action triggered: %s\n", i->second.c_str());
			performAction(i->second.c_str());
		}
	}

	// xinput
	static XINPUT_STATE oldStates[4];
	for(int i = 0; i < 4; ++i) {
		XINPUT_STATE state;
		if(XInputGetState(i, &state) == ERROR_SUCCESS) {
			if(state.dwPacketNumber != oldStates[i].dwPacketNumber) {				
				#define BUTTON(_name) { \
				auto curState = state.Gamepad.wButtons & XINPUT_GAMEPAD_##_name; \
				auto prevState = oldStates[i].Gamepad.wButtons & XINPUT_GAMEPAD_##_name; \
				if(curState && !prevState) { \
					auto elem = buttonBindingMap[i].find(XINPUT_GAMEPAD_##_name); \
					if(elem != buttonBindingMap[i].end()) performAction(elem->second.c_str()); \
				} }
 				#include "xinput.def"
				#undef BUTTON

				oldStates[i] = state;
			}
		}
	}
}


#define ACTION(_name, _action) \
void KeyActions::##_name() { _action; };
#include "Actions.def"
#undef ACTION
