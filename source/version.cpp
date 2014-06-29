#pragma once

#include "version.h"

const char* VER_NAME = "Underground Network";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 9;
const unsigned VER_BUILD = 826;

const char* VER_STRING = "version 0.9.826 built on 2014/06/29 18:43:36";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

