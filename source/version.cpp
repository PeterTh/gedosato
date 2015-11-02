#pragma once

#include "version.h"

const char* VER_NAME = "Chaos Flare";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 19;
const unsigned VER_BUILD = 1961;

const char* VER_STRING = "version 0.19.1961 built on 2015/11/02 20:32:39";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

