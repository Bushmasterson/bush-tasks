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

// Allowed values for Task::priority
inline constexpr const char* kPriorityLow = "low";
inline constexpr const char* kPriorityMedium = "medium";
inline constexpr const char* kPriorityHigh = "high";
inline constexpr const char* kPriorityUrgent = "urgent";

// Allowed values for Task::status
inline constexpr const char* kStatusPending = "pending";
inline constexpr const char* kStatusDone = "done";
inline constexpr const char* kStatusPostponed = "postponed";

bool isValidPriority(const std::string& value);
bool isValidStatus(const std::string& value);

// Returns the current date formatted as "DD.MM.YY".
std::string currentDate( );

// Loads tasks from a JSON file.
// Returns an empty vector if the file does not exist, cannot be read,
// or is not a valid JSON array of objects.
std::vector<Task> loadTasks(const std::string& filePath);

// Saves tasks to a JSON file.
// Returns true on success, false if the file could not be written.
bool saveTasks(const std::vector<Task>& tasks, const std::string& filePath);

} // namespace bush_tasks
