#pragma once

#include <string>
#include <vector>

namespace bush_tasks
{
    void renderHeader();
    void renderTasks(const std::vector<std::string> &tasks);
    void renderHelp();
    bool handleCommand(const std::string &input, std::vector<std::string> &tasks);
    void pause();
}
