#include "core.h"

#include <iostream>

int main() {
  const auto tasks = bush_tasks::loadTasks("test_tasks.json");

  std::cout << "bush_tasks package works.\n";
  std::cout << "Loaded tasks: " << tasks.size() << '\n';

  return 0;
}
