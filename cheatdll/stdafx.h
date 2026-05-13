#pragma once

#include "targetver.h"

//#define EFPATCH_MEMORY_DEBUG
//#define EFPATCH_USE_ATEXIT
//#define EFPATCH_ENABLE_DEBUG_COMMANDS
//#define EFPATCH_ENABLE_TIMING

#if defined(_DEBUG) && defined(EFPATCH_MEMORY_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>
#if defined(_DEBUG) && defined(EFPATCH_MEMORY_DEBUG)
#include <crtdbg.h>
#endif

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <MMSystem.h>

#include <stdint.h>
#include <vector>
#include <string>
#include <array>
#include <map>

#include <stdio.h>

#include "consts.h"
#ifdef TARGET_VOOBLY
#include "iuserpatch.h"
#endif
#include "log.h"
#include "registry.h"
#include "utf8.h"

#include "src\idb\idb_types.h"
#include "src\idb\idb_functions.h"

#include "structs.h"
#include "common.h"

#define EFPATCH_VERSION "EFPATCH for EF v1.5.6-rc1 (1.5.5.1) " __DATE__ " " __TIME__
