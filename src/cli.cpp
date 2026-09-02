#include "cli.h"
#include "core.h"

#include <iostream>
#include <sstream>

namespace bush_tasks {
void renderHeader( ) {
  std::cout << "bush-tasks" << std::endl;
}

void renderTasks(const std::vector<Task>& tasks) {
  if (tasks.empty( )) {
    std::cout << "Tasks list is empty" << std::endl;
    return;
  }

  for (size_t i = 0; i < tasks.size( ); ++i) {
    const auto& task = tasks[i];
    std::cout << i + 1 << ". [" << task.status << "] [" << task.priority << "] " << task.text
              << " (created: " << task.created << ")" << std::endl;

    for (size_t j = 0; j < task.subtasks.size( ); ++j) {
      std::cout << "    - " << task.subtasks[j] << std::endl;
    }
  }
}

void renderHelp( ) {
  std::cout << "Commands: add <text>, del <text>, edit <number> <text>, "
            << "sub <number> <text>, priority <number> <low|medium|high|urgent>, "
            << "status <number> <done|postponed|pending>, done, tasks, clear, exit" << std::endl;
}

bool handleCommand(const std::string& input, std::vector<Task>& tasks) {
  std::string command;
  std::string argument;
  std::istringstream iss(input);
  iss >> command;

  if (command == "add") {
    std::getline(iss, argument);
    if (!argument.empty( ) && argument.front( ) == ' ') {
      argument.erase(0, 1);
    }

    Task task;
    task.text = argument;
    task.priority = "medium";
    task.created = currentDate( );
    task.status = "pending";
    tasks.push_back(task);

    std::cout << "Task added" << std::endl;
  } else if (command == "del") {
    int index;
    iss >> index;
    if (index < 1 || index > static_cast<int>(tasks.size( ))) {
      std::cout << "Invalid task number" << std::endl;
    } else {
      tasks.erase(tasks.begin( ) + index - 1);
      std::cout << "Task deleted" << std::endl;
    }
  } else if (command == "edit") {
    int index;
    iss >> index;
    if (index < 1 || index > static_cast<int>(tasks.size( ))) {
      std::cout << "Invalid task number" << std::endl;
    } else {
      std::getline(iss, argument);
      if (!argument.empty( ) && argument.front( ) == ' ') {
        argument.erase(0, 1);
      }

      if (argument.empty( )) {
        std::cout << "No new task text specified" << std::endl;
      } else {
        tasks[index - 1].text = argument;
        std::cout << "Task updated" << std::endl;
      }
    }
  } else if (command == "sub") {
    int index;
    iss >> index;
    if (index < 1 || index > static_cast<int>(tasks.size( ))) {
      std::cout << "Invalid task number" << std::endl;
    } else {
      std::getline(iss, argument);
      if (!argument.empty( ) && argument.front( ) == ' ') {
        argument.erase(0, 1);
      }

      if (argument.empty( )) {
        std::cout << "No subtask text specified" << std::endl;
      } else {
        tasks[index - 1].subtasks.push_back(argument);
        std::cout << "Subtask added" << std::endl;
      }
    }
  } else if (command == "priority") {
    int index;
    iss >> index;
    std::string level;
    iss >> level;

    if (index < 1 || index > static_cast<int>(tasks.size( ))) {
      std::cout << "Invalid task number" << std::endl;
    } else if (level != "low" && level != "medium" && level != "high" && level != "urgent") {
      std::cout << "Priority must be: low, medium, high or urgent" << std::endl;
    } else {
      tasks[index - 1].priority = level;
      std::cout << "Priority updated" << std::endl;
    }
  } else if (command == "status") {
    int index;
    iss >> index;
    std::string newStatus;
    iss >> newStatus;

    if (index < 1 || index > static_cast<int>(tasks.size( ))) {
      std::cout << "Invalid task number" << std::endl;
    } else if (newStatus != "done" && newStatus != "postponed" && newStatus != "pending") {
      std::cout << "Status must be: done, postponed or pending" << std::endl;
    } else {
      tasks[index - 1].status = newStatus;
      std::cout << "Status updated" << std::endl;
    }
  } else if (command == "clear") {
    tasks.clear( );
    std::cout << "All tasks cleared" << std::endl;
  } else if (command == "tasks") {
    renderTasks(tasks);
  } else if (command == "exit") {
    return true;
  } else {
    std::cout << "Unknown command" << std::endl;
  }

  return false;
}

void pause( ) {
  std::cout << "Press any key to continue..." << std::endl;
  std::cin.get( );
}
} // namespace bush_tasks
