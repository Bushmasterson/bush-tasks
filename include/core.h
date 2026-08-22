#pragma once

#include <string>
#include <vector>

namespace bush_tasks
{
    std::vector<std::string> loadTasks(const std::string &filePath);
    void saveTasks(const std::vector<std::string> &tasks, const std::string &filePath);
}