#pragma once

#include <string>
#include <utility>
#include <vector>

namespace bush_tasks {

struct LocaleInfo {
  std::string code;
  std::string name;
  std::string nameEn;
};

void initI18n( );
std::vector<LocaleInfo> availableLocales( );
std::string currentLanguage( );
bool setLanguage(const std::string& code);

std::string tr(const std::string& key);
std::string tr(const std::string& key, const std::vector<std::pair<std::string, std::string>>& subs);

} // namespace bush_tasks
