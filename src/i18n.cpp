#include "bush_tasks/i18n.h"

#include "bush_tasks/locales_data.h"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace bush_tasks {

namespace {

struct LocaleData {
  std::string code;
  std::string name;
  std::string nameEn;
  std::map<std::string, std::string> messages;
};

std::string g_currentCode = "en";
std::map<std::string, std::string> g_currentMessages;
std::vector<LocaleData> g_locales;

LocaleData parseLocale(const std::string& raw) {
  LocaleData loc;
  try {
    const auto j = nlohmann::json::parse(raw);
    loc.code = j.value("code", "");
    loc.name = j.value("name", loc.code);
    loc.nameEn = j.value("name_en", loc.code);
    if (j.contains("messages") && j["messages"].is_object( )) {
      for (const auto& [k, v] : j["messages"].items( )) {
        if (v.is_string( )) {
          loc.messages[k] = v.get<std::string>( );
        }
      }
    }
  } catch (const std::exception&) {
    // Invalid locale file.
  }
  return loc;
}

std::vector<std::string> customLocaleDirs( ) {
  std::vector<std::string> dirs;
  dirs.push_back("./locales");

#ifdef _WIN32
  if (const char* appdata = std::getenv("APPDATA"); appdata && *appdata) {
    dirs.push_back((fs::path(appdata) / "bush-tasks" / "locales").string( ));
  }
#else
  if (const char* xdg = std::getenv("XDG_DATA_HOME"); xdg && *xdg) {
    dirs.push_back((fs::path(xdg) / "bush-tasks" / "locales").string( ));
  } else if (const char* home = std::getenv("HOME"); home && *home) {
    dirs.push_back((fs::path(home) / ".local" / "share" / "bush-tasks" / "locales").string( ));
  }
#endif

  dirs.push_back("/usr/share/bush-tasks/locales");
  return dirs;
}

void loadBuiltins( ) {
  g_locales.clear( );
  for (const char* raw : {locales_data::EN, locales_data::RU, locales_data::ES}) {
    LocaleData loc = parseLocale(raw);
    if (!loc.code.empty( )) {
      g_locales.push_back(std::move(loc));
    }
  }
}

bool loadCustom(const std::string& code) {
  for (const auto& dir : customLocaleDirs( )) {
    const fs::path p = fs::path(dir) / (code + ".json");
    std::ifstream f(p);
    if (!f.is_open( )) {
      continue;
    }
    const std::string raw((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>( ));
    LocaleData loc = parseLocale(raw);
    if (loc.code != code) {
      continue;
    }
    for (auto& existing : g_locales) {
      if (existing.code == code) {
        existing = std::move(loc);
        return true;
      }
    }
    g_locales.push_back(std::move(loc));
    return true;
  }
  return false;
}

LocaleData* findLocale(const std::string& code) {
  for (auto& l : g_locales) {
    if (l.code == code) {
      return &l;
    }
  }
  return nullptr;
}

} // namespace

void initI18n( ) {
  loadBuiltins( );
  setLanguage("en");
}

std::vector<LocaleInfo> availableLocales( ) {
  std::vector<LocaleInfo> out;
  out.reserve(g_locales.size( ));
  for (const auto& l : g_locales) {
    out.push_back({l.code, l.name, l.nameEn});
  }
  return out;
}

std::string currentLanguage( ) {
  return g_currentCode;
}

bool setLanguage(const std::string& code) {
  if (findLocale(code) == nullptr) {
    loadCustom(code);
  }
  LocaleData* loc = findLocale(code);
  if (loc == nullptr) {
    return false;
  }
  g_currentCode = loc->code;
  g_currentMessages = loc->messages;
  return true;
}

std::string tr(const std::string& key) {
  const auto it = g_currentMessages.find(key);
  if (it == g_currentMessages.end( )) {
    return "[" + key + "]";
  }
  return it->second;
}

std::string tr(const std::string& key, const std::vector<std::pair<std::string, std::string>>& subs) {
  std::string s = tr(key);
  for (const auto& [name, value] : subs) {
    const std::string placeholder = "{" + name + "}";
    std::size_t pos = 0;
    while ((pos = s.find(placeholder, pos)) != std::string::npos) {
      s.replace(pos, placeholder.size( ), value);
      pos += value.size( );
    }
  }
  return s;
}

} // namespace bush_tasks
