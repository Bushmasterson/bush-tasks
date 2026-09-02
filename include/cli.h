#pragma once

#include "core.h"
#include <string>
#include <vector>

namespace bush_tasks {
void renderHeader( );
void renderTasks(const std::vector<Task>& tasks);
void renderHelp( );
bool handleCommand(const std::string& input, std::vector<Task>& tasks);
void pause( );
} // namespace bush_tasks
