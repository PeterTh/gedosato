#include "registry.h"

#include <algorithm>
#include <array>
#include <boost/algorithm/string.hpp>

string getRegString(const string& location, const string& name) {
	static const unsigned CHARS = 2048;
    HKEY key;
    TCHAR value[CHARS]; 
    DWORD bufLen = CHARS*sizeof(TCHAR);
    long ret;
    ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, location.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key);
    if(ret != ERROR_SUCCESS) ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, location.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &key);
	if(ret != ERROR_SUCCESS) return string();
    ret = RegQueryValueExA(key, name.c_str(), 0, 0, (LPBYTE) value, &bufLen);
    RegCloseKey(key);
    if((ret != ERROR_SUCCESS) || (bufLen+1 > CHARS*sizeof(TCHAR))) return string();
	if(value[bufLen] != '\0') value[bufLen+1] = '\0';
    return std::string(value);
}

bool getRegBool(const string& location, const string& name, const bool fallback) {
	bool ret = fallback;
	string val = getRegString(location, name);
	if(val.size() > 0) {
		const std::array<const string,4> trueValues = { "on", "true", "enabled", "1" };
		ret = std::any_of(trueValues.cbegin(), trueValues.cend(), [&val](const string& v){ return boost::algorithm::iequals(val, v); });
	}
	return ret;
}
