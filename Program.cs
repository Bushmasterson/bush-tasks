using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using Spectre.Console;

namespace bush_tasks_cs;

internal static class Program
{
    private static readonly string TasksFilePath = Path.Combine(AppContext.BaseDirectory, "tasks.json");
    private static readonly JsonSerializerOptions JsonOptions = new() { WriteIndented = true };

    private static void Main()
    {
        var tasks = TaskStore.LoadTasks(TasksFilePath, JsonOptions);

        while (true)
        {
            Console.Clear();
            RenderHeader();
            RenderTasks(tasks);
            RenderHelp();

            var input = AnsiConsole.Ask<string>("[green]>[/]").Trim();
            if (string.IsNullOrWhiteSpace(input))
            {
                continue;
            }

            if (HandleCommand(input, tasks))
            {
                break;
            }

            Pause();
        }
    }

    private static bool HandleCommand(string input, List<string> tasks)
    {
        var parts = input.Split(' ', 2);
        var command = parts[0].ToLowerInvariant();
        var argument = parts.Length > 1 ? parts[1].Trim() : string.Empty;

        return command switch
        {
            "add" => ExecuteAdd(argument, tasks),
            "del" => ExecuteDelete(argument, tasks),
            "clear" => ExecuteClear(tasks),
            "exit" => ExecuteExit(),
            _ => ExecuteUnknownCommand(),
        };
    }

    private static void RenderHeader()
    {
        AnsiConsole.Write(new Markup("[bold teal]bush-tasks[/]\n"));
    }

    private static void RenderTasks(List<string> tasks)
    {
        if (tasks.Count == 0)
        {
            AnsiConsole.MarkupLine("[dim]Список задач пуст[/]");
            return;
        }

        var table = new Table()
            .Border(TableBorder.Rounded)
            .AddColumn("№")
            .AddColumn("Задача");

        for (var index = 0; index < tasks.Count; index++)
        {
            table.AddRow((index + 1).ToString(), tasks[index]);
        }

        AnsiConsole.Write(table);
    }

    private static void RenderHelp()
    {
        AnsiConsole.MarkupLine(
            "\n[grey]Команды:[/] [yellow]add[/] [grey]— добавить, [/][yellow]del <номер>[/] [grey]— удалить, [/][yellow]clear[/] [grey]— очистить, [/][yellow]exit[/] [grey]— выход[/]");
    }

    private static bool ExecuteAdd(string argument, List<string> tasks)
    {
        if (string.IsNullOrWhiteSpace(argument))
        {
            AnsiConsole.MarkupLine("[red]Укажите текст задачи[/]");
            return false;
        }

        tasks.Add(argument);
        TaskStore.SaveTasks(tasks, TasksFilePath, JsonOptions);
        AnsiConsole.MarkupLine("[green]Задача добавлена[/]");
        return false;
    }

    private static bool ExecuteDelete(string argument, List<string> tasks)
    {
        if (!int.TryParse(argument, out var index) || index < 1 || index > tasks.Count)
        {
            AnsiConsole.MarkupLine("[red]Неверный номер задачи[/]");
            return false;
        }

        tasks.RemoveAt(index - 1);
        TaskStore.SaveTasks(tasks, TasksFilePath, JsonOptions);
        AnsiConsole.MarkupLine("[green]Задача удалена[/]");
        return false;
    }

    private static bool ExecuteClear(List<string> tasks)
    {
        tasks.Clear();
        TaskStore.SaveTasks(tasks, TasksFilePath, JsonOptions);
        AnsiConsole.MarkupLine("[green]Все задачи очищены[/]");
        return false;
    }

    private static bool ExecuteExit()
    {
        AnsiConsole.MarkupLine("[grey]До свидания![/]");
        return true;
    }

    private static bool ExecuteUnknownCommand()
    {
        AnsiConsole.MarkupLine("[red]Неизвестная команда[/]");
        return false;
    }

    private static void Pause()
    {
        AnsiConsole.MarkupLine("[dim]Нажми любую клавишу для продолжения...[/]");
        Console.ReadKey(true);
    }
}

internal static class TaskStore
{
    public static List<string> LoadTasks(string path, JsonSerializerOptions options)
    {
        try
        {
            if (!File.Exists(path))
            {
                return new List<string>();
            }

            var json = File.ReadAllText(path);
            return JsonSerializer.Deserialize<List<string>>(json, options) ?? new List<string>();
        }
        catch
        {
            return new List<string>();
        }
    }

    public static void SaveTasks(List<string> tasks, string path, JsonSerializerOptions options)
    {
        try
        {
            var json = JsonSerializer.Serialize(tasks, options);
            File.WriteAllText(path, json);
        }
        catch
        {
            AnsiConsole.MarkupLine("[red]Не удалось сохранить список задач[/]");
        }
    }
}
