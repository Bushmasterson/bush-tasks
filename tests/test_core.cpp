#include "core.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

int main( ) {
  const std::string testFile = "test_tasks.json";

  std::remove(testFile.c_str( ));

  auto tasks = bush_tasks::loadTasks(testFile);

  assert(tasks.empty( ));

  std::remove(testFile.c_str( ));

  std::cout << "All tests passed.\n";

  return 0;
}
