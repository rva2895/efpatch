#pragma once

#include "targetver.h"

//#define EFPATCH_MEMORY_DEBUG
//#define EFPATCH_USE_ATEXIT
//#define EFPATCH_ENABLE_DEBUG_COMMANDS
//#define EFPATCH_ENABLE_VERBOSE_STATS
//#define EFPATCH_ENABLE_TIMING
//#define EFPATCH_ENABLE_DETERMINISTIC_RANDOM

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
#include "x87_nan.h"
#ifdef TARGET_VOOBLY
#include "iuserpatch.h"
#endif
#include "log.h"
#include "registry.h"
#include "utf8.h"

// THE CONVERSION SWAP (DECOMP_CPP_MIGRATION.md sec 3b, THE INVERTED ORDER,
// owner-approved 2026-11-28): the by-value member ei (LOS_ExploreInfo, 36 B
// @+0x90) requires the idb definitions FIRST, so the rename wraps
// idb_types.h and the class header follows with the clean name. The idb
// struct survives as RGE_Static_Object_idb (alive in the decode references,
// dead in the build); token-safe: RGE_Static_ObjectVtbl /
// RGE_Static_Object__method / RGE_Static_Object_RGE_Static_Object are
// distinct preprocessor tokens. With idb_functions.h included AFTER the
// class, the idb constants' RGE_Static_Object* positions type-check
// against the class.
#define RGE_Static_Object RGE_Static_Object_idb
#include "src\idb\idb_types.h"
#undef RGE_Static_Object
#include "src\engine\object\RGE_Static_Object.h"
#include "src\idb\idb_functions.h"

#include "structs.h"
#include "common.h"

#define ZLIB_WINAPI

#define EFPATCH_VERSION "EFPATCH for EF v1.5.6-rc10 (1.5.5.10) " __DATE__ " " __TIME__
