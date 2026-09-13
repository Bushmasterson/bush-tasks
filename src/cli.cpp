#include "bush_tasks/cli.h"

#include "bush_tasks/config.h"
#include "bush_tasks/core.h"
#include "bush_tasks/i18n.h"
#include "bush_tasks/version.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace {

// ANSI colors
constexpr const char* kReset = "\033[0m";
constexpr const char* kBold = "\033[1m";
constexpr const char* kDim = "\033[2m";
constexpr const char* kRed = "\033[31m";
constexpr const char* kGreen = "\033[32m";
constexpr const char* kYellow = "\033[33m";
constexpr const char* kBlue = "\033[34m";
constexpr const char* kMagenta = "\033[35m";
constexpr const char* kCyan = "\033[36m";

bool g_colorEnabled = true;

int terminalWidth( ) {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    const int w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return w > 0 ? w : 80;
  }
  return 80;
#else
  struct winsize w {};
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
    return w.ws_col;
  }
  return 80;
#endif
}

std::string separator( ) {
  const int width = std::min(terminalWidth( ), 80);
  return std::string(static_cast<std::size_t>(width), '-');
}

const char* statusColor(const std::string& status) {
  if (!g_colorEnabled) return "";
  if (status == "done") return kGreen;
  if (status == "pending") return kYellow;
  if (status == "postponed") return kDim;
  return "";
}

const char* priorityColor(const std::string& priority) {
  if (!g_colorEnabled) return "";
  if (priority == "urgent") return kRed;
  if (priority == "high") return kMagenta;
  if (priority == "medium") return kBlue;
  if (priority == "low") return kDim;
  return "";
}

std::string colorize(const char* color, const std::string& text) {
  if (!g_colorEnabled || color[0] == '\0') return text;
  return std::string(color) + text + kReset;
}

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
            << "     settings reset\n"
            << "\n"
            << "  " << bush_tasks::tr("settings.example") << ":\n"
            << "     settings language ru\n";
}

void renderSettings( ) {
  using bush_tasks::tr;

  const std::string current = bush_tasks::currentLanguage( );
  const std::string currentName = localeName(current);

  const std::string sep = separator( );

  std::cout << sep << "\n"
            << "  " << tr("settings.current") << "\n"
            << sep << "\n"
            << "  " << tr("settings.language") << ":  " << current << "  (" << currentName << ")\n"
            << "  " << tr("settings.config_file") << ":  " << bush_tasks::configFilePath( ) << "\n";

  std::cout << "\n"
            << sep << "\n"
            << "  " << tr("settings.available") << "\n"
            << sep << "\n";

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
    const std::string previous = bush_tasks::currentLanguage( );
    bush_tasks::Config cfg;
    if (!bush_tasks::saveConfig(cfg)) {
      std::cout << tr("settings.save_failed") << "\n";
      return;
    }
    bush_tasks::setLanguage(cfg.language);
    std::cout << "✓ " << tr("settings.reset_done") << "\n";
    (void)previous;
    return;
  }

  if (sub != "language" && sub != "lang") {
    std::cout << tr("settings.usage") << "\n";
    printHowToChange( );
    return;
  }

  std::string code;
  iss >> code;

  if (code.empty( )) {
    const std::string current = bush_tasks::currentLanguage( );
    const std::string currentName = localeName(current);

    std::cout << tr("settings.language_current", {{"name", currentName}, {"code", current}}) << "\n";
    std::cout << "\n"
              << separator( ) << "\n"
              << "  " << tr("settings.available") << "\n"
              << separator( ) << "\n";
    printAvailableLanguages( );
    printHowToChange( );
    return;
  }

  if (!bush_tasks::setLanguage(code)) {
    std::cout << tr("settings.unknown_language", {{"code", code}}) << "\n\n";

    std::cout << separator( ) << "\n"
              << "  " << tr("settings.available") << "\n"
              << separator( ) << "\n";
    printAvailableLanguages( );
    printHowToChange( );
    return;
  }

  bush_tasks::Config cfg = bush_tasks::loadConfig( );
  cfg.language = code;
  if (!bush_tasks::saveConfig(cfg)) {
    std::cout << tr("settings.save_failed") << "\n";
  }

  const std::string newName = localeName(code);
  std::cout << "✓ " << tr("settings.language_changed", {{"name", newName}, {"code", code}}) << "\n";
}

void renderVersion( ) {
  std::cout << "bush-tasks " << BUSH_TASKS_VERSION << "\n";
}

} // namespace

namespace bush_tasks {

void renderHeader(std::size_t taskCount) {
  const std::string version = BUSH_TASKS_VERSION;
  const std::string lang = currentLanguage( );

  std::cout << colorize(kBold, "bush-tasks " + version)
            << colorize(kDim, "  ·  " + std::to_string(taskCount) + " tasks") << colorize(kDim, "  ·  lang: " + lang)
            << "\n";
}

void renderTasks(const std::vector<Task>& tasks) {
  if (tasks.empty( )) {
    std::cout << colorize(kDim, tr("tasks.empty")) << "\n";
    return;
  }

  const std::string sep = separator( );

  for (std::size_t i = 0; i < tasks.size( ); ++i) {
    const Task& task = tasks[i];

    const std::string statusStr = "[" + task.status + "]";
    const std::string priorityStr = "[" + task.priority + "]";

    std::cout << colorize(kBold, std::to_string(i + 1) + ".") << " " << colorize(statusColor(task.status), statusStr)
              << " " << colorize(priorityColor(task.priority), priorityStr) << " " << task.text << " "
              << colorize(kDim, "(" + tr("tasks.created") + ": " + task.created + ")") << "\n";

    for (const auto& subtask : task.subtasks) {
      std::cout << "    " << colorize(kDim, "- " + subtask) << "\n";
    }

    if (i + 1 < tasks.size( )) {
      std::cout << colorize(kDim, sep) << "\n";
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

  bool mutated = false;

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
    mutated = true;
  } else if (command == "del") {
    const int index = readValidIndex(iss, tasks.size( ));
    if (index == 0) {
      return false;
    }
    tasks.erase(tasks.begin( ) + (index - 1));
    std::cout << tr("tasks.deleted") << "\n";
    mutated = true;
  } else if (command == "edit") {
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
    mutated = true;
  } else if (command == "sub") {
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
    mutated = true;
  } else if (command == "priority") {
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
    mutated = true;
  } else if (command == "status") {
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
    mutated = true;
  } else if (command == "tasks" || command == "list" || command == "ls") {
    // nothing: main loop will re-render
  } else if (command == "clear") {
    tasks.clear( );
    std::cout << tr("tasks.all_cleared") << "\n";
    mutated = true;
  } else if (command == "settings" || command == "config" || command == "lang") {
    handleSettingsCommand(iss);
  } else if (command == "help" || command == "?") {
    renderHelp( );
  } else if (command == "version") {
    renderVersion( );
  } else if (command == "exit" || command == "quit" || command == "q") {
    return true;
  } else {
    std::cout << tr("tasks.unknown_command", {{"command", command}}) << "\n";
    std::cout << tr("main.type_help") << "\n";
  }

  if (mutated) {
    sortTasks(tasks);
  }

  return false;
}

} // namespace bush_tasks
