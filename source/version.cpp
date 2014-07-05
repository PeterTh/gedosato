#pragma once

#include "version.h"

const char* VER_NAME = "Underground Network";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 9;
const unsigned VER_BUILD = 840;

const char* VER_STRING = "version 0.9.840 built on 2014/07/05 15:12:27";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

