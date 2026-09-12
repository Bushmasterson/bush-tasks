#include "bush_tasks/cli.h"
#include "bush_tasks/core.h"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

namespace {

// Reads the remainder of the line, stripping a single leading space.
std::string readRestOfLine(std::istringstream& iss) {
  std::string rest;
  std::getline(iss, rest);
  if (!rest.empty( ) && rest.front( ) == ' ') {
    rest.erase(0, 1);
  }
  return rest;
}

// Reads a 1-based task index and validates it against [1, taskCount].
// On failure prints an error and returns 0.
int readValidIndex(std::istringstream& iss, std::size_t taskCount) {
  int index = 0;
  if (!(iss >> index) || index < 1 || index > static_cast<int>(taskCount)) {
    std::cout << "Invalid task number\n";
    return 0;
  }
  return index;
}

} // namespace

namespace bush_tasks {

void renderHeader( ) {
  std::cout << "bush-tasks\n";
}

void renderTasks(const std::vector<Task>& tasks) {
  if (tasks.empty( )) {
    std::cout << "Tasks list is empty\n";
    return;
  }

  for (std::size_t i = 0; i < tasks.size( ); ++i) {
    const Task& task = tasks[i];
    std::cout << (i + 1) << ". [" << task.status << "] [" << task.priority << "] " << task.text
              << " (created: " << task.created << ")\n";

    for (const auto& subtask : task.subtasks) {
      std::cout << "    - " << subtask << "\n";
    }
  }
}

void renderHelp( ) {
  std::cout << "Commands:\n"
            << "  add <text>\n"
            << "  del <number>\n"
            << "  edit <number> <text>\n"
            << "  sub <number> <text>\n"
            << "  priority <number> <low|medium|high|urgent>\n"
            << "  status <number> <pending|done|postponed>\n"
            << "  tasks\n"
            << "  clear\n"
            << "  exit\n";
}

bool handleCommand(const std::string& input, std::vector<Task>& tasks) {
  std::istringstream iss(input);
  std::string command;
  iss >> command;

  if (command.empty( )) {
    return false;
  }

  if (command == "add") {
    const std::string text = readRestOfLine(iss);
    if (text.empty( )) {
      std::cout << "No task text specified\n";
      return false;
    }
    Task task;
    task.text = text;
    task.priority = kPriorityMedium;
    task.created = currentDate( );
    task.status = kStatusPending;
    tasks.push_back(std::move(task));
    std::cout << "Task added\n";
    return false;
  }

  if (command == "del") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    tasks.erase(tasks.begin( ) + (index - 1));
    std::cout << "Task deleted\n";
    return false;
  }

  if (command == "edit") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    const std::string text = readRestOfLine(iss);
    if (text.empty( )) {
      std::cout << "No new task text specified\n";
      return false;
    }
    tasks[index - 1].text = text;
    std::cout << "Task updated\n";
    return false;
  }

  if (command == "sub") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    const std::string text = readRestOfLine(iss);
    if (text.empty( )) {
      std::cout << "No subtask text specified\n";
      return false;
    }
    tasks[index - 1].subtasks.push_back(text);
    std::cout << "Subtask added\n";
    return false;
  }

  if (command == "priority") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    std::string level;
    iss >> level;
    if (!isValidPriority(level)) {
      std::cout << "Priority must be: low, medium, high or urgent\n";
      return false;
    }
    tasks[index - 1].priority = level;
    std::cout << "Priority updated\n";
    return false;
  }

  if (command == "status") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    std::string newStatus;
    iss >> newStatus;
    if (!isValidStatus(newStatus)) {
      std::cout << "Status must be: pending, done or postponed\n";
      return false;
    }
    tasks[index - 1].status = newStatus;
    std::cout << "Status updated\n";
    return false;
  }

  if (command == "tasks") {
    renderTasks(tasks);
    return false;
  }

  if (command == "clear") {
    tasks.clear( );
    std::cout << "All tasks cleared\n";
    return false;
  }

  if (command == "exit" || command == "quit") {
    return true;
  }

  std::cout << "Unknown command: " << command << "\n";
  return false;
}

} // namespace bush_tasks
