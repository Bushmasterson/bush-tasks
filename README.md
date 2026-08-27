# bush-tasks

A simple, fast CLI task manager written in C++.

## Overview

bush-tasks is a lightweight command-line application for managing your to-do list directly from the terminal.

You can add tasks, delete them by number, clear the entire list, and exit the application — without unnecessary complexity.

## Features

- Add new tasks
- Delete tasks by number
- Clear the entire task list
- Simple interactive CLI
- Lightweight and fast
- Written in modern C++

## Getting Started

### Prerequisites

- A C++17-compatible compiler
- Git

On Windows, MinGW-w64 or MSVC can be used.

### Installation

Clone the repository:

```bash
git clone https://github.com/Bushmasterson/bush-tasks.git
cd bush-tasks
```

### Build the application with CMake:

```bash
cmake -S . -B build
cmake --build build --config Release
```

### Building Tests

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release
```

### Run the tests with CTest:

```bash
ctest --test-dir build -C Release --output-on-failure
```
