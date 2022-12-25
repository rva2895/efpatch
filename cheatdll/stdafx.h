// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // »сключите редко используемые компоненты из заголовков Windows
// ‘айлы заголовков Windows:
#include <windows.h>

#include <stdint.h>
#include <vector>
#include <string>

// TODO: ”становите здесь ссылки на дополнительные заголовки, требующиес€ дл€ программы

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define EFPATCH_VERSION "EFPATCH for EF v1.5.0 (1.5.0.0) " __DATE__ " " __TIME__
