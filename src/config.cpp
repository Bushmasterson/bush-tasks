#include "bush_tasks/config.h"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <fstream>
#include <string>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace bush_tasks {

namespace {

std::string baseConfigDir( ) {
#ifdef _WIN32
  const char* appdata = std::getenv("APPDATA");
  if (appdata && *appdata) {
    return appdata;
  }
  const char* profile = std::getenv("USERPROFILE");
  if (profile && *profile) {
    return profile;
  }
  return ".";
#else
  const char* xdg = std::getenv("XDG_CONFIG_HOME");
  if (xdg && *xdg) {
    return xdg;
  }
  const char* home = std::getenv("HOME");
  if (home && *home) {
    return std::string(home) + "/.config";
  }
  return ".";
#endif
}

void makeDir(const std::string& path) {
  if (path.empty( )) {
    return;
  }
#ifdef _WIN32
  _mkdir(path.c_str( ));
#else
  mkdir(path.c_str( ), 0755);
#endif
}

void ensureDirs(const std::string& path) {
  if (path.empty( )) {
    return;
  }
  std::string accum;
  accum.reserve(path.size( ));
  for (std::size_t i = 0; i < path.size( ); ++i) {
    accum.push_back(path[i]);
    const bool isSep = (path[i] == '/' || path[i] == '\\');
    const bool isLast = (i + 1 == path.size( ));
    if ((isSep || isLast) && accum.size( ) > 1) {
      makeDir(accum);
    }
  }
}

std::string joinPath(const std::string& a, const std::string& b) {
  if (a.empty( )) {
    return b;
  }
  const char last = a.back( );
  if (last == '/' || last == '\\') {
    return a + b;
  }
  return a + "/" + b;
}

} // namespace

std::string configDirectory( ) {
  return joinPath(baseConfigDir( ), "bush-tasks");
}

std::string configFilePath( ) {
  return joinPath(configDirectory( ), "config.json");
}

Config loadConfig( ) {
  Config cfg;

  std::ifstream f(configFilePath( ));
  if (!f.is_open( )) {
    return cfg;
  }

  try {
    nlohmann::json j;
    f >> j;
    if (j.contains("language") && j["language"].is_string( )) {
      cfg.language = j["language"].get<std::string>( );
    }
  } catch (const std::exception&) {
    // Broken config: fall back to defaults.
  }

  return cfg;
}

bool saveConfig(const Config& cfg) {
  ensureDirs(configDirectory( ));

  nlohmann::json j;
  j["language"] = cfg.language;

  std::ofstream f(configFilePath( ));
  if (!f.is_open( )) {
    return false;
  }

  f << j.dump(2) << "\n";
  return f.good( );
}

} // namespace bush_tasks
