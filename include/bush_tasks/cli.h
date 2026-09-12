#pragma once

#include "bush_tasks/core.h"

#include <string>
#include <vector>

namespace bush_tasks {

void renderHeader();
void renderTasks(const std::vector<Task>& tasks);
void renderHelp();

// Processes a single interactive command.
// Returns true if the main loop should exit.
bool handleCommand(const std::string& input, std::vector<Task>& tasks);

} // namespace bush_tasks
