#pragma once

#include "version.h"

const char* VER_NAME = "Third Ensemble";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 16;
const unsigned VER_BUILD = 1724;

const char* VER_STRING = "version 0.16.1724 built on 2014/12/11 22:42:20";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

