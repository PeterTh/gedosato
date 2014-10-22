#pragma once

#include "version.h"

const char* VER_NAME = "Barrage am Ring";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 14;
const unsigned VER_BUILD = 1620;

const char* VER_STRING = "version 0.14.1620 built on 2014/10/22 19:15:19";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

