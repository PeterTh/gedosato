#pragma once

#include "version.h"

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 4;
const unsigned VER_BUILD = 423;

const char* VER_STRING = "version 0.4.423 built on 2014/04/30 19:27:37";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif
