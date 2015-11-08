#pragma once

#include "version.h"

const char* VER_NAME = "Chaos Flare";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 19;
const unsigned VER_BUILD = 1987;

const char* VER_STRING = "version 0.19.1987 built on 2015/11/08 10:19:12";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

