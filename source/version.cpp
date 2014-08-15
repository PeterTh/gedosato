#pragma once

#include "version.h"

const char* VER_NAME = "Sepia Recollections";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 12;
const unsigned VER_BUILD = 1185;

const char* VER_STRING = "version 0.12.1185 built on 2014/08/15 12:22:32";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

