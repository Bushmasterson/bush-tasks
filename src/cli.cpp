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

    void renderTasks(const std::vector<Task> &tasks)
    {
        if (tasks.empty())
        {
            std::cout << "Список задач пуст" << std::endl;
            return;
        }

        for (size_t i = 0; i < tasks.size(); ++i)
        {
            const auto &task = tasks[i];
            std::cout << i + 1 << ". [" << task.status << "] [" << task.priority << "] "
                       << task.text << " (создано: " << task.created << ")" << std::endl;

            for (size_t j = 0; j < task.subtasks.size(); ++j)
            {
                std::cout << "    - " << task.subtasks[j] << std::endl;
            }
        }
    }

    void renderHelp()
    {
        std::cout << "Commands: add <text>, del <text>, edit <number> <text>, "
                   << "sub <number> <text>, priority <number> <low|medium|high|urgent>, "
                   << "status <number> <done|postponed|pending>, done, tasks, clear, exit" << std::endl;
    }

    bool handleCommand(const std::string &input, std::vector<Task> &tasks)
    {
        std::string command;
        std::string argument;
        std::istringstream iss(input);
        iss >> command;

        if (command == "add")
        {
            std::getline(iss, argument);
            if (!argument.empty() && argument.front() == ' ')
            {
                argument.erase(0, 1);
            }

            Task task;
            task.text = argument;
            task.priority = "medium";
            task.created = currentDate();
            task.status = "pending";
            tasks.push_back(task);

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
                if (!argument.empty() && argument.front() == ' ')
                {
                    argument.erase(0, 1);
                }

                if (argument.empty())
                {
                    std::cout << "Не указан новый текст задачи" << std::endl;
                }
                else
                {
                    tasks[index - 1].text = argument;
                    std::cout << "Задача изменена" << std::endl;
                }
            }
        }
        else if (command == "sub")
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
                if (!argument.empty() && argument.front() == ' ')
                {
                    argument.erase(0, 1);
                }

                if (argument.empty())
                {
                    std::cout << "Не указан текст подзадачи" << std::endl;
                }
                else
                {
                    tasks[index - 1].subtasks.push_back(argument);
                    std::cout << "Подзадача добавлена" << std::endl;
                }
            }
        }
        else if (command == "priority")
        {
            int index;
            iss >> index;
            std::string level;
            iss >> level;

            if (index < 1 || index > static_cast<int>(tasks.size()))
            {
                std::cout << "Неверный номер задачи" << std::endl;
            }
            else if (level != "low" && level != "medium" && level != "high" && level != "urgent")
            {
                std::cout << "Приоритет должен быть: low, medium, high или urgent" << std::endl;
            }
            else
            {
                tasks[index - 1].priority = level;
                std::cout << "Приоритет обновлён" << std::endl;
            }
        }
        else if (command == "status")
        {
            int index;
            iss >> index;
            std::string newStatus;
            iss >> newStatus;

            if (index < 1 || index > static_cast<int>(tasks.size()))
            {
                std::cout << "Неверный номер задачи" << std::endl;
            }
            else if (newStatus != "done" && newStatus != "postponed" && newStatus != "pending")
            {
                std::cout << "Статус должен быть: done, postponed или pending" << std::endl;
            }
            else
            {
                tasks[index - 1].status = newStatus;
                std::cout << "Статус обновлён" << std::endl;
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
