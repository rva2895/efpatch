// stdafx.h: ���������� ���� ��� ����������� ��������� ���������� ������
// ��� ���������� ������ ��� ����������� �������, ������� ����� ������������, ��
// �� ����� ����������
//

#pragma once

#ifdef TARGET_VOOBLY
//#define CHEATDLL_NOLOG

#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ��������� ����� ������������ ���������� �� ���������� Windows
// ����� ���������� Windows:
#include <windows.h>

#include <stdint.h>
#include <vector>
#include <string>

// TODO: ���������� ����� ������ �� �������������� ���������, ����������� ��� ���������

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
