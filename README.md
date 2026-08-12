# Bush-tasks

A simple and beautiful task manager for the terminal, built with C# and .NET.

It allows you to quickly manage your to-do list right from the console — no internet, no browser, no distractions.

## ✨ Features

- Add new tasks
- View tasks in a clean table layout
- Delete tasks by number
- Clear all tasks at once
- Tasks are automatically saved to `tasks.json`
- Colorful interface powered by Spectre.Console

## 🧰 Tech stack

- **Language:** C# 13
- **Framework:** .NET 10.0
- **Library:** Spectre.Console (for beautiful terminal UI)
- **Data storage:** JSON file (local)

## 🔧 How to build and run from source

### 1. Install .NET SDK

Make sure you have .NET 10.0 SDK installed.

Download it here:  
👉 [https://dotnet.microsoft.com/en-us/download](https://dotnet.microsoft.com/en-us/download)

Check installation:
bash
dotnet --version

2. Clone the repository

bash
git clone https://github.com/Bushmasterson/bush-tasks.git
cd bush-tasks

3. Build the executable

bash
dotnet publish -c Release -r win-x64 --self-contained true -p:PublishSingleFile=true -o ./publish

4. Run the app

bash
cd publish
./bush-tasks.exe
`