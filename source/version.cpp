#pragma once

#include "version.h"

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 4;
const unsigned VER_BUILD = 471;

const char* VER_STRING = "version 0.4.471 built on 2014/05/01 22:08:16";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif
