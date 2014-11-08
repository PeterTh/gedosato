#pragma once

#include "version.h"

const char* VER_NAME = "Tokikagura";

const unsigned VER_MAJOR = 0;
const unsigned VER_MINOR = 15;
const unsigned VER_BUILD = 1682;

const char* VER_STRING = "version 0.15.1682 built on 2014/11/08 11:15:02";

#ifdef _DEBUG
const char* MODE_STRING = "DEBUG";
#else
const char* MODE_STRING = "RELEASE";
#endif

