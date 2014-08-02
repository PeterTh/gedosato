#pragma once

#include <string>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using std::string;

bool matchWildcard(const string& str, const string& pattern);

namespace {
	using formatter = decltype(boost::format(""));

	template<typename T>
	formatter format_impl(formatter fmt, T elem) {
		return fmt % elem;
	}
	template<typename T, typename... Rest>
	formatter format_impl(formatter fmt, T elem, Rest... rest) {
		return format_impl(fmt % elem, rest...);
	}
}

template<typename... Rest> 
string format(const char* formatString, Rest... rest) {
	return format_impl(boost::format(formatString), rest...).str();
}