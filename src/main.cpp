#include "bush_tasks/cli.h"
#include "bush_tasks/config.h"
#include "bush_tasks/core.h"
#include "bush_tasks/i18n.h"
#include "bush_tasks/version.h.in"

#include <cstring>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

void printVersion( ) {
  std::cout << "bush-tasks " << BUSH_TASKS_VERSION << '\n';
}

void printUsage( ) {
  std::cout << "Usage: bush-tasks [OPTIONS]\n"
            << "\n"
            << "Options:\n"
            << "  -v, --version   Print version and exit\n"
            << "  -h, --help      Print help and exit\n"
            << "\n"
            << "Once running, use these interactive commands:\n"
            << "  add <text>\n"
            << "  del <number>\n"
            << "  edit <number> <text>\n"
            << "  sub <number> <text>\n"
            << "  priority <number> <low|medium|high|urgent>\n"
            << "  status <number> <pending|done|postponed>\n"
            << "  tasks\n"
            << "  settings [language <code>]\n"
            << "  clear\n"
            << "  exit\n";
}

void enableUtf8Console( ) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif
}

enum class CliAction { Continue, ExitSuccess, ExitError };

CliAction parseArguments(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    const char* arg = argv[i];
    if (std::strcmp(arg, "-v") == 0 || std::strcmp(arg, "--version") == 0) {
      printVersion( );
      return CliAction::ExitSuccess;
    }
    if (std::strcmp(arg, "-h") == 0 || std::strcmp(arg, "--help") == 0) {
      printUsage( );
      return CliAction::ExitSuccess;
    }
    std::cerr << "Unknown option: " << arg << "\n\n";
    printUsage( );
    return CliAction::ExitError;
  }
  return CliAction::Continue;
}

} // namespace

int main(int argc, char** argv) {
  enableUtf8Console( );

  switch (parseArguments(argc, argv)) {
  case CliAction::ExitSuccess:
    return 0;
  case CliAction::ExitError:
    return 2;
  case CliAction::Continue:
    break;
  }

  bush_tasks::initI18n( );

  const bush_tasks::Config cfg = bush_tasks::loadConfig( );
  if (!cfg.language.empty( )) {
    bush_tasks::setLanguage(cfg.language);
  }

  const std::string filePath = bush_tasks::configDirectory( ) + "/tasks.json";
  auto tasks = bush_tasks::loadTasks(filePath);

  while (true) {
    bush_tasks::renderHeader( );
    bush_tasks::renderTasks(tasks);
    bush_tasks::renderHelp( );

    std::string input;
    if (!std::getline(std::cin, input)) {
      break; // EOF (Ctrl+D / Ctrl+Z then Enter)
    }

    if (bush_tasks::handleCommand(input, tasks)) {
      break;
    }

    if (!bush_tasks::saveTasks(tasks, filePath)) {
      std::cerr << "Warning: failed to save tasks to " << filePath << '\n';
    }
  }

  return 0;
}
