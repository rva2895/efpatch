#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

#include <stdint.h>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "consts.h"
#ifdef TARGET_VOOBLY
#include "iuserpatch.h"
#endif
#include "log.h"

#include "src\idb\idb_types.h"
#include "src\idb\idb_functions.h"

#include "structs.h"
#include "common.h"

#define ZLIB_WINAPI

//#define CHEATDLL_NOLOG

#define EFPATCH_VERSION "EFPATCH for EF v1.5.4 (1.5.4.2) " __DATE__ " " __TIME__
