#pragma once

#include "version.h"

const char* VER_NAME = "Third Ensemble";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 16;
const unsigned VER_BUILD = 1770;

const char* VER_STRING = "version 0.16.1770 built on 2014/12/25 18:43:44";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

