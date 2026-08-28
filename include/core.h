#pragma once

#include <string>
#include <vector>

namespace bush_tasks {
struct Task {
  std::string text;
  std::vector<std::string> subtasks;
  std::string priority;
  std::string created;
  std::string status;
};

std::string currentDate();

std::vector<Task> loadTasks(const std::string& filePath);
void saveTasks(const std::vector<Task>& tasks, const std::string& filePath);
} // namespace bush_tasks
