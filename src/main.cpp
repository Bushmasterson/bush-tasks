#include "cli.h"
#include "core.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
  SetConsoleOutputCP(65001);
  SetConsoleCP(65001);
#endif

  const std::string filePath = "tasks.json";
  auto tasks = bush_tasks::loadTasks(filePath);

  while (true) {
    bush_tasks::renderHeader();
    bush_tasks::renderTasks(tasks);
    bush_tasks::renderHelp();

    std::string input;
    std::getline(std::cin, input);

    if (bush_tasks::handleCommand(input, tasks)) {
      break;
    }

    bush_tasks::saveTasks(tasks, filePath);
  }

  return 0;
}
