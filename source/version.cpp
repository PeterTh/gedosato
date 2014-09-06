#pragma once

#include "version.h"

const char* VER_NAME = "Cross Again";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 13;
const unsigned VER_BUILD = 1417;

const char* VER_STRING = "version 0.13.1417 built on 2014/09/06 13:00:50";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

