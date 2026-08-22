#include "core.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

int main()
{
    const std::string testFile = "test_tasks.txt";

    // На случай, если файл остался от предыдущего запуска
    std::remove(testFile.c_str());

    auto tasks = bush_tasks::loadTasks(testFile);
    tasks.push_back("Тестовая задача");

    bush_tasks::saveTasks(tasks, testFile);
    tasks.clear();

    tasks = bush_tasks::loadTasks(testFile);
    assert(tasks.size() == 1);
    assert(tasks[0] == "Тестовая задача");

    // Убираем за собой
    std::remove(testFile.c_str());

    std::cout << "Test passed" << std::endl;
}
