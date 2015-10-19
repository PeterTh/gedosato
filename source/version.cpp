#pragma once

#include "version.h"

const char* VER_NAME = "Disharmonized Genesis";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 18;
const unsigned VER_BUILD = 1912;

const char* VER_STRING = "version 0.18.1912 built on 2015/10/19 20:44:13";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

