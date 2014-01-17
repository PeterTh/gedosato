#include "string_utils.h"

bool matchWildcard(const string& str, const string& pattern) {
	string regex = pattern;
	boost::replace_all(regex, ".", "\\.");
	boost::replace_all(regex, "*", ".*");
	regex = string(".*") + regex;
	std::regex rx(regex, std::regex::icase);
	return std::regex_match(str, rx);
}
