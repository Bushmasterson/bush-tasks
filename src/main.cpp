#include "bush_tasks_version.h.in"
#include "cli.h"
#include "core.h"

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

void printHelp( ) {
  std::cout << "Usage: bush-tasks [OPTIONS]\n"
            << "\n"
            << "Options:\n"
            << "  -v, --version   Print version and exit\n"
            << "  -h, --help      Print this help and exit\n"
            << "\n"
            << "Interactive commands (once running):\n"
            << "  add <text>\n"
            << "  del <number>\n"
            << "  edit <number> <text>\n"
            << "  sub <number> <text>\n"
            << "  priority <number> <low|medium|high|urgent>\n"
            << "  status <number> <done|postponed|pending>\n"
            << "  tasks\n"
            << "  clear\n"
            << "  exit\n";
}

} // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
  SetConsoleOutputCP(65001);
  SetConsoleCP(65001);
#endif

  for (int i = 1; i < argc; ++i) {
    const char* arg = argv[i];
    if (std::strcmp(arg, "-v") == 0 || std::strcmp(arg, "--version") == 0) {
      printVersion( );
      return 0;
    }
    if (std::strcmp(arg, "-h") == 0 || std::strcmp(arg, "--help") == 0) {
      printHelp( );
      return 0;
    }
    std::cerr << "Unknown option: " << arg << "\n\n";
    printHelp( );
    return 2;
  }

  const std::string filePath = "tasks.json";
  auto tasks = bush_tasks::loadTasks(filePath);

  while (true) {
    bush_tasks::renderHeader( );
    bush_tasks::renderTasks(tasks);
    bush_tasks::renderHelp( );

    std::string input;
    if (!std::getline(std::cin, input)) {
      break;
    }

    if (bush_tasks::handleCommand(input, tasks)) {
      break;
    }

    bush_tasks::saveTasks(tasks, filePath);
  }

  return 0;
}
