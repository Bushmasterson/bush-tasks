#include "bush_tasks/cli.h"

#include "bush_tasks/config.h"
#include "bush_tasks/core.h"
#include "bush_tasks/i18n.h"
#include "bush_tasks/version.h.in"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

std::string readRestOfLine(std::istringstream& iss) {
  std::string rest;
  std::getline(iss, rest);
  if (!rest.empty( ) && rest.front( ) == ' ') {
    rest.erase(0, 1);
  }
  return rest;
}

int readValidIndex(std::istringstream& iss, std::size_t taskCount) {
  int index = 0;
  if (!(iss >> index) || index < 1 || index > static_cast<int>(taskCount)) {
    std::cout << bush_tasks::tr("tasks.invalid_number") << "\n";
    return 0;
  }
  return index;
}

void printAvailableLanguages( ) {
  std::cout << bush_tasks::tr("settings.available") << ":\n";
  for (const auto& l : bush_tasks::availableLocales( )) {
    std::cout << "  " << l.code << "  —  " << l.name << " (" << l.nameEn << ")\n";
  }
}

void renderSettings( ) {
  using bush_tasks::tr;
  std::cout << tr("settings.current") << ":\n";

  std::string currentName = bush_tasks::currentLanguage( );
  for (const auto& l : bush_tasks::availableLocales( )) {
    if (l.code == bush_tasks::currentLanguage( )) {
      currentName = l.name;
      break;
    }
  }
  std::cout << "  " << tr("settings.language") << ":  " << bush_tasks::currentLanguage( ) << " (" << currentName
            << ")\n";
  std::cout << "  " << tr("settings.config_file") << ":  " << bush_tasks::configFilePath( ) << "\n";
  std::cout << "\n";
  printAvailableLanguages( );
}

void handleSettingsCommand(std::istringstream& iss) {
  using bush_tasks::tr;

  std::string sub;
  iss >> sub;

  if (sub.empty( )) {
    renderSettings( );
    return;
  }

  if (sub != "language") {
    std::cout << tr("settings.usage") << "\n";
    return;
  }

  std::string code;
  iss >> code;

  if (code.empty( )) {
    std::string currentName = bush_tasks::currentLanguage( );
    for (const auto& l : bush_tasks::availableLocales( )) {
      if (l.code == bush_tasks::currentLanguage( )) {
        currentName = l.name;
        break;
      }
    }
    std::cout << tr("settings.language_current", {{"name", currentName}, {"code", bush_tasks::currentLanguage( )}})
              << "\n";
    return;
  }

  if (!bush_tasks::setLanguage(code)) {
    std::cout << tr("settings.unknown_language", {{"code", code}}) << "\n\n";
    printAvailableLanguages( );
    return;
  }

  bush_tasks::Config cfg = bush_tasks::loadConfig( );
  cfg.language = code;
  bush_tasks::saveConfig(cfg);

  std::string newName = code;
  for (const auto& l : bush_tasks::availableLocales( )) {
    if (l.code == code) {
      newName = l.name;
      break;
    }
  }

  std::cout << tr("settings.language_changed", {{"name", newName}, {"code", code}}) << "\n";
}

void renderVersion( ) {
  std::cout << "bush-tasks " << BUSH_TASKS_VERSION << "\n";
}

} // namespace

namespace bush_tasks {

void renderHeader( ) {
  std::cout << tr("app.title") << "\n";
}

void renderTasks(const std::vector<Task>& tasks) {
  if (tasks.empty( )) {
    std::cout << tr("tasks.empty") << "\n";
    return;
  }

  for (std::size_t i = 0; i < tasks.size( ); ++i) {
    const Task& task = tasks[i];
    std::cout << (i + 1) << ". [" << task.status << "] [" << task.priority << "] " << task.text << " ("
              << tr("tasks.created") << ": " << task.created << ")\n";

    for (const auto& subtask : task.subtasks) {
      std::cout << "    - " << subtask << "\n";
    }
  }
}

void renderHelp( ) {
  std::cout << tr("help.header") << "\n"
            << "  " << tr("help.add") << "\n"
            << "  " << tr("help.del") << "\n"
            << "  " << tr("help.edit") << "\n"
            << "  " << tr("help.sub") << "\n"
            << "  " << tr("help.priority") << "\n"
            << "  " << tr("help.status") << "\n"
            << "  " << tr("help.tasks") << "\n"
            << "  " << tr("help.clear") << "\n"
            << "  " << tr("help.settings") << "\n"
            << "  " << tr("help.exit") << "\n";
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
      std::cout << tr("tasks.no_text") << "\n";
      return false;
    }
    Task task;
    task.text = text;
    task.priority = kPriorityMedium;
    task.created = currentDate( );
    task.status = kStatusPending;
    tasks.push_back(std::move(task));
    std::cout << tr("tasks.added") << "\n";
    return false;
  }

  if (command == "del") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    tasks.erase(tasks.begin( ) + (index - 1));
    std::cout << tr("tasks.deleted") << "\n";
    return false;
  }

  if (command == "edit") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    const std::string text = readRestOfLine(iss);
    if (text.empty( )) {
      std::cout << tr("tasks.no_text") << "\n";
      return false;
    }
    tasks[index - 1].text = text;
    std::cout << tr("tasks.updated") << "\n";
    return false;
  }

  if (command == "sub") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    const std::string text = readRestOfLine(iss);
    if (text.empty( )) {
      std::cout << tr("tasks.no_subtask_text") << "\n";
      return false;
    }
    tasks[index - 1].subtasks.push_back(text);
    std::cout << tr("tasks.subtask_added") << "\n";
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
      std::cout << tr("tasks.priority_invalid") << "\n";
      return false;
    }
    tasks[index - 1].priority = level;
    std::cout << tr("tasks.priority_updated") << "\n";
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
      std::cout << tr("tasks.status_invalid") << "\n";
      return false;
    }
    tasks[index - 1].status = newStatus;
    std::cout << tr("tasks.status_updated") << "\n";
    return false;
  }

  if (command == "tasks" || command == "list" || command == "ls") {
    renderTasks(tasks);
    return false;
  }

  if (command == "clear") {
    tasks.clear( );
    std::cout << tr("tasks.all_cleared") << "\n";
    return false;
  }

  if (command == "settings" || command == "config" || command == "lang") {
    handleSettingsCommand(iss);
    return false;
  }

  if (command == "help" || command == "?") {
    renderHelp( );
    return false;
  }

  if (command == "version") {
    renderVersion( );
    return false;
  }

  if (command == "exit" || command == "quit" || command == "q") {
    return true;
  }

  std::cout << tr("tasks.unknown_command", {{"command", command}}) << "\n";
  std::cout << "Type 'help' to see all available commands.\n";
  return false;
}

} // namespace bush_tasks
