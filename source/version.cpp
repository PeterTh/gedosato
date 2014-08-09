#pragma once

#include "version.h"

const char* VER_NAME = "At Sixes And Sevens";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 11;
const unsigned VER_BUILD = 1086;

const char* VER_STRING = "version 0.11.1086 built on 2014/08/09 21:45:18";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

