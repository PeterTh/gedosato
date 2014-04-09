#pragma once

#include "version.h"

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 1;
const unsigned VER_BUILD = 247;

const char* VER_STRING = "version 0.1.247 built on 2014/04/09 19:54:41";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif
