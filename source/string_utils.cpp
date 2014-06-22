#include "string_utils.h"

#include <stdarg.h>

string format(const char* formatString, ...) {
	const unsigned BUFFER_SIZE = 2048 * 8;
	char buffer[BUFFER_SIZE];
	va_list arglist;
	va_start(arglist, formatString);
	vsnprintf_s(buffer, BUFFER_SIZE, formatString, arglist);
	va_end(arglist);
	return string(buffer);
}

bool matchWildcard(const string& str, const string& pattern) {
	string regex = pattern;
	boost::replace_all(regex, ".", "\\.");
	boost::replace_all(regex, "*", ".*");
	regex = string(".*") + regex;
	std::regex rx(regex, std::regex::icase);
	return std::regex_match(str, rx);
}
