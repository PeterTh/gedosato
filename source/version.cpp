#pragma once

#include "version.h"

const char* VER_NAME = "Cross Again";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 13;
const unsigned VER_BUILD = 1353;

const char* VER_STRING = "version 0.13.1353 built on 2014/08/31 18:50:01";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

