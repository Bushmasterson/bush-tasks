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

constexpr const char* kSeparator = "──────────────────────────────────────────────";

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

std::string localeName(const std::string& code) {
  for (const auto& l : bush_tasks::availableLocales( )) {
    if (l.code == code) {
      return l.name;
    }
  }
  return code;
}

void printAvailableLanguages( ) {
  const std::string current = bush_tasks::currentLanguage( );

  for (const auto& l : bush_tasks::availableLocales( )) {
    const bool isCurrent = (l.code == current);
    std::cout << "  " << (isCurrent ? "▶ " : "  ") << l.code << "  —  " << l.name << " (" << l.nameEn << ")"
              << (isCurrent ? "  ← " + bush_tasks::tr("settings.current_mark") : "") << "\n";
  }
}

void printHowToChange( ) {
  std::cout << "\n"
            << "  " << bush_tasks::tr("settings.how_to_change") << "\n"
            << "     settings language <code>\n"
            << "\n"
            << "  " << bush_tasks::tr("settings.example") << ":\n"
            << "     settings language ru\n";
}

void renderSettings( ) {
  using bush_tasks::tr;

  const std::string current = bush_tasks::currentLanguage( );
  const std::string currentName = localeName(current);

  std::cout << kSeparator << "\n"
            << "  " << tr("settings.current") << "\n"
            << kSeparator << "\n"
            << "  " << tr("settings.language") << ":  " << current << "  (" << currentName << ")\n"
            << "  " << tr("settings.config_file") << ":  " << bush_tasks::configFilePath( ) << "\n";

  std::cout << "\n"
            << kSeparator << "\n"
            << "  " << tr("settings.available") << "\n"
            << kSeparator << "\n";

  printAvailableLanguages( );
  printHowToChange( );
}

void handleSettingsCommand(std::istringstream& iss) {
  using bush_tasks::tr;

  std::string sub;
  iss >> sub;

  if (sub.empty( )) {
    renderSettings( );
    return;
  }

  if (sub == "reset") {
    bush_tasks::Config cfg; // defaults: language = "en"
    bush_tasks::saveConfig(cfg);
    bush_tasks::setLanguage(cfg.language);
    std::cout << "✓ " << tr("settings.reset_done") << "\n";
    return;
  }

  if (sub != "language" && sub != "lang") {
    std::cout << tr("settings.usage") << "\n";
    printHowToChange( );
    return;
  }

  if (sub != "language" && sub != "lang") {
    std::cout << tr("settings.usage") << "\n";
    printHowToChange( );
    return;
  }

  std::string code;
  iss >> code;

  // settings language — показать текущий и как менять
  if (code.empty( )) {
    const std::string current = bush_tasks::currentLanguage( );
    const std::string currentName = localeName(current);

    std::cout << tr("settings.language_current", {{"name", currentName}, {"code", current}}) << "\n";
    std::cout << "\n"
              << kSeparator << "\n"
              << "  " << tr("settings.available") << "\n"
              << kSeparator << "\n";
    printAvailableLanguages( );
    printHowToChange( );
    return;
  }

  // settings language <code> — попытка смены
  if (!bush_tasks::setLanguage(code)) {
    std::cout << tr("settings.unknown_language", {{"code", code}}) << "\n\n";

    std::cout << kSeparator << "\n"
              << "  " << tr("settings.available") << "\n"
              << kSeparator << "\n";
    printAvailableLanguages( );
    printHowToChange( );
    return;
  }

  bush_tasks::Config cfg = bush_tasks::loadConfig( );
  cfg.language = code;
  bush_tasks::saveConfig(cfg);

  const std::string newName = localeName(code);

  std::cout << "✓ " << tr("settings.language_changed", {{"name", newName}, {"code", code}}) << "\n";
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
