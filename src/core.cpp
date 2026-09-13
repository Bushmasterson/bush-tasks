#include "bush_tasks/core.h"

#include <nlohmann/json.hpp>

#include <algorithm>
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

int priorityRank(const std::string& p) {
  if (p == bush_tasks::kPriorityUrgent) return 0;
  if (p == bush_tasks::kPriorityHigh) return 1;
  if (p == bush_tasks::kPriorityMedium) return 2;
  if (p == bush_tasks::kPriorityLow) return 3;
  return 4;
}

int statusRank(const std::string& s) {
  if (s == bush_tasks::kStatusPending) return 0;
  if (s == bush_tasks::kStatusPostponed) return 1;
  if (s == bush_tasks::kStatusDone) return 2;
  return 3;
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
  oss << std::put_time(&tmBuf, "%Y-%m-%d");
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
    task.created = item.value("created", "");

    const std::string priority = item.value("priority", kPriorityMedium);
    task.priority = isValidPriority(priority) ? priority : kPriorityMedium;

    const std::string status = item.value("status", kStatusPending);
    task.status = isValidStatus(status) ? status : kStatusPending;

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

  std::ifstream existing(filePath);
  if (existing.is_open( )) {
    existing.close( );
    std::remove(bakPath.c_str( ));
    std::rename(filePath.c_str( ), bakPath.c_str( ));
  }

  if (std::rename(tmpPath.c_str( ), filePath.c_str( )) != 0) {
    std::rename(bakPath.c_str( ), filePath.c_str( ));
    return false;
  }

  return true;
}

void sortTasks(std::vector<Task>& tasks) {
  std::stable_sort(tasks.begin( ), tasks.end( ), [](const Task& a, const Task& b) {
    const int sa = statusRank(a.status);
    const int sb = statusRank(b.status);
    if (sa != sb) return sa < sb;

    const int pa = priorityRank(a.priority);
    const int pb = priorityRank(b.priority);
    if (pa != pb) return pa < pb;

    return a.created > b.created;
  });
}

} // namespace bush_tasks
