#include "core.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

int main()
{
    const std::string testFile = "test_tasks.json";

    // На случай, если файл остался от предыдущего запуска
    std::remove(testFile.c_str());

    auto tasks = bush_tasks::loadTasks(testFile);

    bush_tasks::Task task;
    task.text = "Тестовая задача";
    task.priority = "medium";
    task.created = bush_tasks::currentDate();
    task.status = "pending";
    tasks.push_back(task);

    bush_tasks::saveTasks(tasks, testFile);
    tasks.clear();

    tasks = bush_tasks::loadTasks(testFile);
    assert(tasks.size() == 1);
    assert(tasks[0].text == "Тестовая задача");
    assert(tasks[0].priority == "medium");
    assert(tasks[0].status == "pending");

    // Убираем за собой
    std::remove(testFile.c_str());

    std::cout << "Test passed" << std::endl;
}
