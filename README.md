# bush-tasks

A small cross-platform CLI task manager written in C++17.

## Features

- Add, edit, delete tasks and subtasks
- Priority levels: `low`, `medium`, `high`, `urgent`
- Status tracking: `pending`, `postponed`, `done`
- Persistent storage in a JSON file (`tasks.json`)
- Native binaries for Linux, Windows, and macOS (including Apple Silicon)
- Zero dependencies — just a single static binary

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
add <text>                              Add a new task
del <number>                            Delete a task
edit <number> <text>                    Edit task text
sub <number> <text>                     Add a subtask
priority <number> <low|medium|high|urgent>   Change priority
status <number> <done|postponed|pending>     Change status
tasks                                   List all tasks
clear                                   Delete all tasks
exit                                    Quit
```

Flags:

```
-v, --version    Print version
-h, --help       Print help
```

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

## License

MIT — see [LICENSE](LICENSE).
