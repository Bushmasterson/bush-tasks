#include "bush_tasks/core.h"

#include <nlohmann/json.hpp>

#include <cstdio>
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

LoadResult loadTasks(const std::string& filePath) {
  LoadResult result;

  std::ifstream file(filePath);
  if (!file.is_open( )) {
    result.status = LoadStatus::Missing;
    return result;
  }

  nlohmann::json root;
  try {
    file >> root;
  } catch (const std::exception&) {
    result.status = LoadStatus::Corrupt;
    return result;
  }

  if (!root.is_array( )) {
    result.status = LoadStatus::Corrupt;
    return result;
  }

  result.tasks.reserve(root.size( ));

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

    result.tasks.push_back(std::move(task));
  }

  return result;
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

  const std::string tmpPath = filePath + ".tmp";
  const std::string bakPath = filePath + ".bak";

  // 1) write to tmp
  {
    std::ofstream tmp(tmpPath, std::ios::trunc);
    if (!tmp.is_open( )) {
      return false;
    }
    tmp << root.dump(2);
    if (!tmp.good( )) {
      std::remove(tmpPath.c_str( ));
      return false;
    }
  }

  // 2) rotate existing file to .bak (best-effort)
  std::ifstream existing(filePath);
  if (existing.is_open( )) {
    existing.close( );
    std::remove(bakPath.c_str( ));
    std::rename(filePath.c_str( ), bakPath.c_str( ));
  }

  // 3) promote tmp to target
  if (std::rename(tmpPath.c_str( ), filePath.c_str( )) != 0) {
    // try to restore from .bak if rename failed
    std::rename(bakPath.c_str( ), filePath.c_str( ));
    return false;
  }

  return true;
}

} // namespace bush_tasks
