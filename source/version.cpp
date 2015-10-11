#pragma once

#include "version.h"

const char* VER_NAME = "Forlane | Rei";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 17;
const unsigned VER_BUILD = 1841;

const char* VER_STRING = "version 0.17.1841 built on 2015/10/11 18:25:39";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

