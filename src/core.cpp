#include "core.h"
#include "json.hpp"

#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

namespace bush_tasks
{
    std::string currentDate()
    {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d.%m.%y");
        return oss.str();
    }

    std::vector<Task> loadTasks(const std::string &filePath)
    {
        std::vector<Task> tasks;
        std::ifstream file(filePath);

        if (!file.is_open())
        {
            return tasks;
        }

        json j;
        try
        {
            file >> j;
        }
        catch (...)
        {
            return tasks;
        }

        for (const auto &item : j)
        {
            Task task;
            task.text = item.value("text", "");
            task.priority = item.value("priority", "medium");
            task.created = item.value("created", "");
            task.status = item.value("status", "pending");

            if (item.contains("subtasks"))
            {
                for (const auto &sub : item["subtasks"])
                {
                    task.subtasks.push_back(sub.get<std::string>());
                }
            }

            tasks.push_back(task);
        }

        return tasks;
    }

    void saveTasks(const std::vector<Task> &tasks, const std::string &filePath)
    {
        json j = json::array();

        for (const auto &task : tasks)
        {
            json item;
            item["text"] = task.text;
            item["subtasks"] = task.subtasks;
            item["priority"] = task.priority;
            item["created"] = task.created;
            item["status"] = task.status;
            j.push_back(item);
        }

        std::ofstream file(filePath);
        file << j.dump(2);
    }
}
