#pragma once

std::vector<std::pair<std::wstring, std::wstring>> query_languages();
void install_language(const std::wstring& lang);
void setLanguageDllHooks();
