#pragma once

#include "version.h"

const char* VER_NAME = "How Could Hell Be Any Worse";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 21;
const unsigned VER_BUILD = 2160;

const char* VER_STRING = "version 0.21.2160 built on 2016/02/15 20:19:50";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

