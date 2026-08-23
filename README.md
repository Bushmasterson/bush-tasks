# bush-tasks

A simple, fast CLI task manager written in C++.

## Overview

bush-tasks is a lightweight terminal tool for managing your to-do list. Add tasks, remove them by number, list what's pending, or wipe the slate clean — all from the command line.

## Getting Started

### Prerequisites

- A C++ compiler — MinGW-w64 is recommended on Windows
- Git, to clone the repository

### Installation

1. Clone the repository

```bash
git clone https://github.com/Bushmasterson/bush-tasks.git
cd bush-tasks
```

2. Compile the project

```bash
g++ -std=c++17 -Iinclude src/main.cpp src/core.cpp src/cli.cpp -o bush-tasks.exe
```

3. Run it

```bash
./bush-tasks.exe
```

## Usage

| Command      | Description              |
| ------------ | ------------------------ |
| add <text>   | Adds a new task          |
| del <number> | Deletes a task by number |
| clear        | Clears the whole list    |
| exit         | Exits the program        |
