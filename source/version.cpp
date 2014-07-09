#pragma once

#include "version.h"

const char* VER_NAME = "Underground Network";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 9;
const unsigned VER_BUILD = 853;

const char* VER_STRING = "version 0.9.853 built on 2014/07/09 19:02:54";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

