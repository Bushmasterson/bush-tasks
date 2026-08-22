#include "core.h"
#include "cli.h"

#include <iostream>
#include <windows.h>

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    const std::string filePath = "tasks.txt";
    auto tasks = bush_tasks::loadTasks(filePath);

    while (true)
    {
        bush_tasks::renderHeader();
        bush_tasks::renderTasks(tasks);
        bush_tasks::renderHelp();

        std::string input;
        std::getline(std::cin, input);

        if (bush_tasks::handleCommand(input, tasks))
        {
            break;
        }

        bush_tasks::saveTasks(tasks, filePath);
    }

    return 0;
}
