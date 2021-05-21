// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#ifdef TARGET_VOOBLY
//#define CHEATDLL_NOLOG

#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // »сключите редко используемые компоненты из заголовков Windows
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
#include "common.h"
#ifdef TARGET_VOOBLY
#include "iuserpatch.h"
#endif
#include "structs.h"
#include "log.h"

#define ZLIB_WINAPI
