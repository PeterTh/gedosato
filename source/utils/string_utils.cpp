#include "utils/string_utils.h"

#include <stdarg.h>

#include <iomanip>

#include <dxgi1_2.h>
#include <d3d11_2.h>

bool matchWildcard(const string& str, const string& pattern) {
	string regex = pattern;
	const char* escapeChars[] = { ".", "+", "(", ")", "[", "]", "^", "$"};
	for(auto ch : escapeChars) {
		boost::replace_all(regex, ch, string("\\") + ch);
	}
	boost::replace_all(regex, "*", ".*");
	boost::replace_all(regex, "?", ".");
	regex = string(".*") + regex;
	std::regex rx(regex, std::regex::icase);
	return std::regex_match(str, rx);
}

std::ostream& operator<<(std::ostream& os, REFGUID guid) {
	os << "{";
	os << std::setfill('0');
	os.width(8);
	os << std::hex << guid.Data1 << '-';

	os.width(4);
	os << std::hex << guid.Data2 << '-';

	os.width(4);
	os << std::hex << guid.Data3 << '-';

	os.width(2);
	os  << std::hex
		<< static_cast<short>(guid.Data4[0])
		<< static_cast<short>(guid.Data4[1])
		<< '-'
		<< static_cast<short>(guid.Data4[2])
		<< static_cast<short>(guid.Data4[3])
		<< static_cast<short>(guid.Data4[4])
		<< static_cast<short>(guid.Data4[5])
		<< static_cast<short>(guid.Data4[6])
		<< static_cast<short>(guid.Data4[7]);
	os << "}";

	#define UUID_OBJECT(__UUID_OBJECT_) \
	if(guid == __uuidof(__UUID_OBJECT_)) { \
		os << "(__uuidof(" #__UUID_OBJECT_ "))"; \
	}
	#include "uuids.def"

	return os;
}
