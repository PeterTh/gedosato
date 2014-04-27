#pragma once

#include "version.h"

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 3;
const unsigned VER_BUILD = 393;

const char* VER_STRING = "version 0.3.393 built on 2014/04/28 00:13:07";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif
