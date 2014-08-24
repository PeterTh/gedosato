#pragma once

#include "version.h"

const char* VER_NAME = "Sepia Recollections";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 12;
const unsigned VER_BUILD = 1300;

const char* VER_STRING = "version 0.12.1300 built on 2014/08/24 17:57:00";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

