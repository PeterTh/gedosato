#pragma once

#include "version.h"

const char* VER_NAME = "Tokikagura";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 15;
const unsigned VER_BUILD = 1684;

const char* VER_STRING = "version 0.15.1684 built on 2014/11/09 19:35:31";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

