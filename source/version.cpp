#pragma once

#include "version.h"

const char* VER_NAME = "Hopeful Jasper";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 20;
const unsigned VER_BUILD = 2056;

const char* VER_STRING = "version 0.20.2056 built on 2015/12/29 20:27:37";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

