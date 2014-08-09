#pragma once

#include "main.h"

#include <string>

string getRegString(const string& location, const string& name);
bool getRegBool(const string& location, const string& name, const bool fallback = false);
