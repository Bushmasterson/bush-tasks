#include "core.h"

#include <fstream>
#include <sstream>

namespace bush_tasks
{
    std::vector<std::string> loadTasks(const std::string &filePath)
    {
        std::vector<std::string> tasks;
        std::ifstream file(filePath);
        std::string line;

        while (std::getline(file, line))
        {
            tasks.push_back(line);
        }

        return tasks;
    }

    void saveTasks(const std::vector<std::string> &tasks, const std::string &filePath)
    {
        std::ofstream file(filePath);
        for (const auto &task : tasks)
        {
            file << task << std::endl;
        }
    }
}