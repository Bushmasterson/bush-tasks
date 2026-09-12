#include "bush_tasks/config.h"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

namespace bush_tasks {

namespace {

fs::path baseConfigDir( ) {
#ifdef _WIN32
  const char* appdata = std::getenv("APPDATA");
  if (appdata && *appdata) {
    return fs::path(appdata);
  }
  const char* profile = std::getenv("USERPROFILE");
  return profile ? fs::path(profile) : fs::path(".");
#else
  const char* xdg = std::getenv("XDG_CONFIG_HOME");
  if (xdg && *xdg) {
    return fs::path(xdg);
  }
  const char* home = std::getenv("HOME");
  return home ? fs::path(home) / ".config" : fs::path(".");
#endif
}

} // namespace

std::string configDirectory( ) {
  return (baseConfigDir( ) / "bush-tasks").string( );
}

std::string configFilePath( ) {
  return (baseConfigDir( ) / "bush-tasks" / "config.json").string( );
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
  std::error_code ec;
  fs::create_directories(configDirectory( ), ec);
  if (ec) {
    return false;
  }

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
