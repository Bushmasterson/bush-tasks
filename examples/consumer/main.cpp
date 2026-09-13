#include <bush_tasks/core.h>

#include <iostream>

int main( ) {
  const auto result = bush_tasks::loadTasks("test_tasks.json");

  if (result.status == bush_tasks::LoadStatus::Corrupt) {
    std::cerr << "Warning: file is corrupt\n";
  }

  std::cout << "bush_tasks package works.\n";
  std::cout << "Loaded tasks: " << result.tasks.size( ) << '\n';

  return 0;
}
