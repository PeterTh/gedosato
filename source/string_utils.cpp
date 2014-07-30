#include "string_utils.h"

#include <stdarg.h>

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
