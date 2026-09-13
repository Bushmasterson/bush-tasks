# bush-tasks

[![CI](https://github.com/Bushmasterson/bush-tasks/actions/workflows/ci.yml/badge.svg)](https://github.com/Bushmasterson/bush-tasks/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/Bushmasterson/bush-tasks)](https://github.com/Bushmasterson/bush-tasks/releases/latest)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

A small cross-platform CLI task manager written in C++17.

## Features

- Add, edit, delete tasks and subtasks
- Priority levels: `low`, `medium`, `high`, `urgent`
- Status tracking: `pending`, `postponed`, `done`
- Persistent storage in a JSON file, kept in the user config directory
- Multi-language interface (`en`, `ru`)
- Native binaries for Linux, Windows, and macOS (including Apple Silicon)
- Zero runtime dependencies — just a single static binary

## Install

Download the binary for your platform from the [latest release](https://github.com/Bushmasterson/bush-tasks/releases/latest).

| Platform                      | File                           |
| ----------------------------- | ------------------------------ |
| Linux x86_64                  | `bush-tasks-linux-amd64`       |
| Linux i686                    | `bush-tasks-linux-i686`        |
| Linux arm64                   | `bush-tasks-linux-arm64`       |
| Linux armv7                   | `bush-tasks-linux-armv7`       |
| Windows x86_64                | `bush-tasks-windows-amd64.exe` |
| Windows i686                  | `bush-tasks-windows-x86.exe`   |
| Windows arm64                 | `bush-tasks-windows-arm64.exe` |
| macOS (Intel + Apple Silicon) | `bush-tasks-macos-universal`   |

On Linux and macOS, make it executable:

```sh
chmod +x bush-tasks-linux-amd64
./bush-tasks-linux-amd64
```

Release assets are signed with [GitHub Attestations](https://docs.github.com/en/actions/security-guides/using-artifact-attestations-to-establish-provenance-for-builds). To verify:

```sh
gh attestation verify bush-tasks-linux-amd64 --repo Bushmasterson/bush-tasks
```

## Usage

Run `bush-tasks` and use commands interactively:

```
add <text>                                     Add a new task
del <number>                                   Delete a task
edit <number> <text>                           Edit task text
sub <number> <text>                            Add a subtask
priority <number> <low|medium|high|urgent>     Change priority
status <number> <done|postponed|pending>       Change status
tasks                                          List all tasks
settings [language <code>]                     Show or change settings
clear                                          Delete all tasks
exit                                           Quit
```

Flags:

```
-v, --version             Print version and exit
-h, --help                Print help and exit
-l, --language <code>     Use the given language for this run
```

## Storage

Tasks and configuration are stored in the user config directory:

| Platform      | Path                              |
| ------------- | --------------------------------- |
| Linux / macOS | `~/.config/bush-tasks/tasks.json` |
| Windows       | `%APPDATA%\bush-tasks\tasks.json` |

Configuration file `config.json` lives next to `tasks.json`.

## Languages

Built-in locales: `en`, `ru`.

Change language interactively:

```
settings language ru
```

Or for a single run:

```sh
bush-tasks -l ru
```

Custom locale files can be dropped into `./locales/` or `~/.local/share/bush-tasks/locales/` without rebuilding.

## Build from source

Requirements: CMake 3.20+, Ninja, a C++17 compiler.

```sh
git clone https://github.com/Bushmasterson/bush-tasks.git
cd bush-tasks
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

The binary appears at `build/bush-tasks`.

Run tests:

```sh
cmake -B build -G Ninja -DBUSH_TASKS_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Using as a library

`bush-tasks` can also be embedded in your own CMake project:

```cmake
find_package(bush_tasks 2.2 CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE bush_tasks::bush_tasks)
```

```cpp
#include <bush_tasks/core.h>

int main() {
  auto tasks = bush_tasks::loadTasks("tasks.json");
  return tasks.empty() ? 0 : 1;
}
```

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for build instructions, coding style, and PR guidelines.

## License

MIT — see [LICENSE](LICENSE).
