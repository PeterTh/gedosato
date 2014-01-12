#pragma once

#include "version.h"

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 1;
const unsigned VER_BUILD = 16;

const char* VER_STRING = "version 0.1.16 built on 2014/01/12 21:41:27";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif
