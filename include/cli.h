#pragma once

#include <string>
#include <vector>
#include "core.h"

namespace bush_tasks
{
    void renderHeader();
    void renderTasks(const std::vector<Task> &tasks);
    void renderHelp();
    bool handleCommand(const std::string &input, std::vector<Task> &tasks);
    void pause();
}
