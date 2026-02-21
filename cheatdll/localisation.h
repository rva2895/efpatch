#pragma once

std::vector<std::pair<std::string, std::string>> query_languages();
void install_language(const std::string& lang);
void setLanguageDllHooks();
