#pragma once

#include "version.h"

const char* VER_NAME = "Underground Network";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 9;
const unsigned VER_BUILD = 864;

const char* VER_STRING = "version 0.9.864 built on 2014/07/11 16:31:05";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

