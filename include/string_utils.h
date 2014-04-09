#pragma once

#include <string>
#include <regex>
#include <boost/algorithm/string.hpp>

using std::string;

bool matchWildcard(const string& str, const string& pattern);
