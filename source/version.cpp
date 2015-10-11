#pragma once

#include "version.h"

const char* VER_NAME = "Disharmonized Genesis";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 18;
const unsigned VER_BUILD = 1869;

const char* VER_STRING = "version 0.18.1869 built on 2015/10/11 20:38:55";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

