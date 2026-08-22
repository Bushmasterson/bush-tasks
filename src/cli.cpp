#include "cli.h"
#include "core.h"

#include <iostream>
#include <sstream>

namespace bush_tasks
{
    void renderHeader()
    {
        std::cout << "bush-tasks" << std::endl;
    }

    void renderTasks(const std::vector<std::string> &tasks)
    {
        if (tasks.empty())
        {
            std::cout << "Список задач пуст" << std::endl;
            return;
        }

        for (size_t i = 0; i < tasks.size(); ++i)
        {
            std::cout << i + 1 << ". " << tasks[i] << std::endl;
        }
    }

    void renderHelp()
    {
        std::cout << "Команды: add <текст>, del <номер>, clear, exit" << std::endl;
    }

    bool handleCommand(const std::string &input, std::vector<std::string> &tasks)
    {
        std::string command;
        std::string argument;
        std::istringstream iss(input);
        iss >> command;

        if (command == "add")
        {
            std::getline(iss, argument);
            tasks.push_back(argument);
            std::cout << "Задача добавлена" << std::endl;
        }
        else if (command == "del")
        {
            int index;
            iss >> index;
            if (index < 1 || index > static_cast<int>(tasks.size()))
            {
                std::cout << "Неверный номер задачи" << std::endl;
            }
            else
            {
                tasks.erase(tasks.begin() + index - 1);
                std::cout << "Задача удалена" << std::endl;
            }
        }
        else if (command == "clear")
        {
            tasks.clear();
            std::cout << "Все задачи очищены" << std::endl;
        }
        else if (command == "exit")
        {
            return true;
        }
        else
        {
            std::cout << "Неизвестная команда" << std::endl;
        }

        return false;
    }

    void pause()
    {
        std::cout << "Нажми любую клавишу для продолжения..." << std::endl;
        std::cin.get();
    }
}