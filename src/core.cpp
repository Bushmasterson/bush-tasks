#include "bush_tasks/core.h"

#include <nlohmann/json.hpp>

#include <ctime>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

namespace {

bool matchesAny(const std::string& value, std::initializer_list<const char*> options) {
  for (const char* option : options) {
    if (value == option) {
      return true;
    }
  }
  return false;
}

} // namespace

namespace bush_tasks {

bool isValidPriority(const std::string& value) {
  return matchesAny(value, {kPriorityLow, kPriorityMedium, kPriorityHigh, kPriorityUrgent});
}

bool isValidStatus(const std::string& value) {
  return matchesAny(value, {kStatusPending, kStatusDone, kStatusPostponed});
}

std::string currentDate( ) {
  const std::time_t now = std::time(nullptr);
  std::tm tmBuf{ };
#if defined(_WIN32)
  localtime_s(&tmBuf, &now);
#else
  localtime_r(&now, &tmBuf);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tmBuf, "%d.%m.%y");
  return oss.str( );
}

std::vector<Task> loadTasks(const std::string& filePath) {
  std::ifstream file(filePath);
  if (!file.is_open( )) {
    return { };
  }

  nlohmann::json root;
  try {
    file >> root;
  } catch (const std::exception&) {
    return { };
  }

  if (!root.is_array( )) {
    return { };
  }

  std::vector<Task> tasks;
  tasks.reserve(root.size( ));

  for (const auto& item : root) {
    if (!item.is_object( )) {
      continue;
    }

    Task task;
    task.text = item.value("text", "");
    task.priority = item.value("priority", kPriorityMedium);
    task.created = item.value("created", "");
    task.status = item.value("status", kStatusPending);

    if (item.contains("subtasks") && item["subtasks"].is_array( )) {
      for (const auto& sub : item["subtasks"]) {
        if (sub.is_string( )) {
          task.subtasks.push_back(sub.get<std::string>( ));
        }
      }
    }

    tasks.push_back(std::move(task));
  }

  return tasks;
}

bool saveTasks(const std::vector<Task>& tasks, const std::string& filePath) {
  nlohmann::json root = nlohmann::json::array( );

  for (const auto& task : tasks) {
    root.push_back({
        {"text", task.text},
        {"subtasks", task.subtasks},
        {"priority", task.priority},
        {"created", task.created},
        {"status", task.status},
    });
  }

  std::ofstream file(filePath);
  if (!file.is_open( )) {
    return false;
  }

  file << root.dump(2);
  return file.good( );
}

} // namespace bush_tasks
