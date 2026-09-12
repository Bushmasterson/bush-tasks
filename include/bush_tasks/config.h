#pragma once

#include <string>

namespace bush_tasks {

struct Config {
  std::string language = "en";
};

std::string configDirectory( );
std::string configFilePath( );
Config loadConfig( );
bool saveConfig(const Config& cfg);

} // namespace bush_tasks
