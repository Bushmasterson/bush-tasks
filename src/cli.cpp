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
        std::cout << "Команды: add <текст>, del <номер>, edit <номер> <новый текст>, tasks, clear, exit" << std::endl;
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
        else if (command == "edit")
        {
            int index;
            iss >> index;
            if (index < 1 || index > static_cast<int>(tasks.size()))
            {
                std::cout << "Неверный номер задачи" << std::endl;
            }
            else
            {
                std::getline(iss, argument);
                if (argument.empty() || argument.find_first_not_of(" ") == std::string::npos)
                {
                    std::cout << "Не указан новый текст задачи" << std::endl;
                }
                else
                {
                    tasks[index - 1] = argument.substr(argument.find_first_not_of(" "));
                    std::cout << "Задача изменена" << std::endl;
                }
            }
        }
        else if (command == "clear")
        {
            tasks.clear();
            std::cout << "Все задачи очищены" << std::endl;
        }
        else if (command == "tasks")
        {
            renderTasks(tasks);
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
