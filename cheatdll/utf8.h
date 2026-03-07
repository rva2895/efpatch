#pragma once

std::wstring UTF8ToWide(const char* us, int c = -1);
std::wstring UTF8ToWide(const std::string& us);
std::string WideToUTF8(const wchar_t* ws, int c = -1);
std::string WideToUTF8(const std::wstring& ws);

#define UTF8ToWide_c_str(x) (UTF8ToWide(x).c_str())
#define WideToUTF8_c_str(x) (WideToUTF8(x).c_str())
#define UTF8ToWide_c_str_NULL(x) (x ? UTF8ToWide(x).c_str() : NULL)
#define WideToUTF8_c_str_NULL(x) (x ? WideToUTF8(x).c_str() : NULL)

void setUTF8Hooks();
