#include "core.h"

#include <cassert>
#include <iostream>
#include <string>

int main()
{
    auto tasks = bush_tasks::loadTasks("test_tasks.txt");
    tasks.push_back("Тестовая задача");

    bush_tasks::saveTasks(tasks, "test_tasks.txt");
    tasks.clear();

    tasks = bush_tasks::loadTasks("test_tasks.txt");
    assert(tasks.size() == 1);
    assert(tasks[0] == "Тестовая задача");

    std::cout << "Тест пройден" << std::endl;
}
