#pragma once

#include "version.h"

const char* VER_NAME = "Stranger Than Fiction";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 6;
const unsigned VER_BUILD = 609;

const char* VER_STRING = "version 0.6.609 built on 2014/06/01 22:01:37";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

