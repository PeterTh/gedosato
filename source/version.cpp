#pragma once

#include "version.h"

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 5;
const unsigned VER_BUILD = 527;

const char* VER_STRING = "version 0.5.527 built on 2014/05/18 16:41:14";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif
