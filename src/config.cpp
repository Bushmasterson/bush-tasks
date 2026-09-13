#include "bush_tasks/config.h"

#include <nlohmann/json.hpp>

#include <cctype>
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

// Reject obviously unsafe paths supplied via environment variables:
//   - empty or containing NUL
//   - not absolute
//   - any ".." path component (traversal)
//
// This breaks taint propagation from std::getenv() into file access calls.
bool isSafeAbsolutePath(const std::string& path) {
  if (path.empty( )) {
    return false;
  }
  if (path.find('\0') != std::string::npos) {
    return false;
  }

#ifdef _WIN32
  const bool isDriveAbsolute = path.size( ) >= 3 && std::isalpha(static_cast<unsigned char>(path[0])) != 0 &&
                               path[1] == ':' && (path[2] == '\\' || path[2] == '/');
  const bool isUnc = path.size( ) >= 2 && (path[0] == '\\' || path[0] == '/') && (path[1] == '\\' || path[1] == '/');
  if (!isDriveAbsolute && !isUnc) {
    return false;
  }
#else
  if (path[0] != '/') {
    return false;
  }
#endif

  // Walk path components and reject any that equals "..".
  std::size_t start = 0;
  while (start <= path.size( )) {
    std::size_t end = path.find_first_of("/\\", start);
    if (end == std::string::npos) {
      end = path.size( );
    }
    const std::size_t len = end - start;
    if (len == 2 && path[start] == '.' && path[start + 1] == '.') {
      return false;
    }
    if (end == path.size( )) {
      break;
    }
    start = end + 1;
  }

  return true;
}

std::string baseConfigDir( ) {
#ifdef _WIN32
  if (const char* appdata = std::getenv("APPDATA"); appdata != nullptr) {
    const std::string candidate = appdata;
    if (isSafeAbsolutePath(candidate)) {
      return candidate;
    }
  }
  if (const char* profile = std::getenv("USERPROFILE"); profile != nullptr) {
    const std::string candidate = profile;
    if (isSafeAbsolutePath(candidate)) {
      return candidate;
    }
  }
#else
  if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg != nullptr) {
    const std::string candidate = xdg;
    if (isSafeAbsolutePath(candidate)) {
      return candidate;
    }
  }
  if (const char* home = std::getenv("HOME"); home != nullptr) {
    const std::string candidate = home;
    if (isSafeAbsolutePath(candidate)) {
      return candidate + "/.config";
    }
  }
#endif
  // Safe fallback: current directory. Constant, no env taint.
  return ".";
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
