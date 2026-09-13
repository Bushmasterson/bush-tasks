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

inline constexpr const char* kPriorityLow = "low";
inline constexpr const char* kPriorityMedium = "medium";
inline constexpr const char* kPriorityHigh = "high";
inline constexpr const char* kPriorityUrgent = "urgent";

inline constexpr const char* kStatusPending = "pending";
inline constexpr const char* kStatusDone = "done";
inline constexpr const char* kStatusPostponed = "postponed";

bool isValidPriority(const std::string& value);
bool isValidStatus(const std::string& value);

std::string currentDate( );

enum class LoadStatus { Ok, Missing, Corrupt };

struct LoadResult {
  std::vector<Task> tasks;
  LoadStatus status = LoadStatus::Ok;
};
LoadResult loadTasks(const std::string& filePath);

bool saveTasks(const std::vector<Task>& tasks, const std::string& filePath);

} // namespace bush_tasks
