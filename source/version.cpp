#pragma once

#include "version.h"

const char* VER_NAME = "Poltergeist";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 10;
const unsigned VER_BUILD = 881;

const char* VER_STRING = "version 0.10.881 built on 2014/07/14 22:48:07";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

