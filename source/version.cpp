#pragma once

#include "version.h"

const char* VER_NAME = "Poltergeist";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 10;
const unsigned VER_BUILD = 891;

const char* VER_STRING = "version 0.10.891 built on 2014/07/16 20:23:21";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

