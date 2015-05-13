#pragma once

#include "version.h"

const char* VER_NAME = "Forlane | Rei";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 17;
const unsigned VER_BUILD = 1811;

const char* VER_STRING = "version 0.17.1811 built on 2015/05/13 19:25:05";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

