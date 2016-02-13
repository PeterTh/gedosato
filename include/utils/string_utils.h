#pragma once

#include <string>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <guiddef.h>

using std::string;

bool matchWildcard(const string& str, const string& pattern);

namespace {
	using formatter = decltype(boost::format(""));

	template<typename T>
	formatter format_impl(formatter fmt, const T& elem) {
		return fmt % elem;
	}
	template<typename T, typename... Rest>
	formatter format_impl(formatter fmt, T elem, const Rest&... rest) {
		return format_impl(fmt % elem, rest...);
	}
}

template<typename... Rest> 
string format(const char* formatString, const Rest&... rest) {
	return format_impl(boost::format(formatString), rest...).str();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::unique_ptr<T>& dt) {
	os << &(*dt);
	return os;
}

std::ostream& operator<<(std::ostream& os, REFGUID guid);
