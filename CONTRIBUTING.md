# Contributing to bush-tasks

Thanks for your interest! This document covers the essentials: how to build,
test, and submit changes.

---

## Requirements

| Tool     | Version | Notes                           |
| -------- | ------- | ------------------------------- |
| CMake    | 3.20+   |                                 |
| Ninja    | any     | Or Make, but Ninja is preferred |
| Compiler | C++17   | GCC 11+, Clang 14+, MSVC 19.30+ |
| Python   | 3.12+   | Only for pre-commit hooks       |
| Git      | any     |                                 |

---

## Setup

```sh
git clone https://github.com/Bushmasterson/bush-tasks.git
cd bush-tasks
py -m pip install --user pre-commit
py -m pre_commit install
```

The pre-commit hook runs `clang-format`, trims trailing whitespace, fixes
end-of-file, and validates YAML on every commit.

---

## Build

```sh
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Binary appears at `build/bush-tasks`.

Useful CMake options:

| Option                   | Default | Purpose                   |
| ------------------------ | ------- | ------------------------- |
| `BUSH_TASKS_BUILD_TESTS` | `OFF`   | Build unit tests          |
| `BUSH_TASKS_BUILD_CLI`   | `ON`    | Build the CLI executable  |
| `BUSH_TASKS_STATIC`      | `OFF`   | Static link (Linux/MinGW) |
| `BUSH_TASKS_WERROR`      | `OFF`   | Warnings as errors        |
| `BUSH_TASKS_STRIP`       | `ON`    | Strip release binaries    |

---

## Test

```sh
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUSH_TASKS_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

All tests must pass before submitting a PR.

---

## Code style

Enforced automatically by `clang-format` (`.clang-format` in the repo root).
Rules:

- LLVM base style, 2-space indent, 100-column limit
- Includes sorted alphabetically
- Braces on the same line
- No `using namespace` in headers

Run manually:

```sh
clang-format -i src/*.cpp include/bush_tasks/*.h
```

Third-party code (`third_party/`, `build/`) is excluded via
`.clang-format-ignore`.

---

## Commit messages

Format: `<type>: <imperative description in English>`

Types:

| Type       | When to use                             |
| ---------- | --------------------------------------- |
| `feat`     | New user-facing functionality           |
| `fix`      | Bug fix                                 |
| `refactor` | Code change without behavior change     |
| `chore`    | Build, CI, tooling, dependencies        |
| `docs`     | Documentation only                      |
| `test`     | Adding or fixing tests                  |
| `style`    | Formatting, whitespace, no logic change |

Examples:

```
feat: add due dates to tasks
fix: prevent crash when tasks.json is empty
docs: update build instructions for MSYS2
```

Keep the subject under 72 characters. Use the body for details if needed.

---

## Branch names

```
feat/<short-name>
fix/<short-name>
refactor/<short-name>
chore/<short-name>
docs/<short-name>
```

Example: `feat/due-dates`, `fix/empty-file-crash`.

---

## Pull requests

1. Sync with main:

   ```sh
   git checkout main
   git fetch origin
   git reset --hard origin/main
   ```

2. Create a branch, make changes, commit.

3. Run pre-commit and tests locally:

   ```sh
   py -m pre_commit run --all-files
   ctest --test-dir build --output-on-failure
   ```

4. Push and open a PR. Fill out the PR template.

5. Wait for CI to go green. Address review comments.

6. PRs are squash-merged into `main`.

### PR checklist

- [ ] Builds on Linux, Windows, and macOS (CI covers this)
- [ ] All tests pass
- [ ] `clang-format` applied (pre-commit handles it)
- [ ] `CHANGELOG.md` updated if user-facing change
- [ ] `README.md` updated if behavior or flags changed
- [ ] No new warnings with `-DBUSH_TASKS_WERROR=ON`

---

## Adding a language

1. Create `locales/<code>.json` based on `locales/en.json`.
2. Do **not** edit `cmake/locales_data.h.in` directly — locales are embedded
   at configure time from the `locales/` directory.
3. To ship a new built-in locale, add a `file(READ ...)` and a matching
   `@LOCALES_<CODE>_JSON@` entry in `CMakeLists.txt` and `locales_data.h.in`.

Custom locales without rebuilding are supported by dropping a JSON file into:

- `./locales/`
- `~/.local/share/bush-tasks/locales/` (Linux)
- `%APPDATA%\bush-tasks\locales` (Windows)
- `/usr/share/bush-tasks/locales/` (system installs)

---

## Reporting bugs

Open an issue using the **Bug report** template. Include:

- Operating system and version
- `bush-tasks --version` output
- Steps to reproduce
- Expected vs actual behavior
- Contents of `tasks.json` if relevant (redact sensitive data)

---

## Feature requests

Open an issue using the **Feature request** template. Describe:

- The problem you're trying to solve
- Your proposed solution
- Alternatives you considered

Not every request will be accepted — the project aims to stay small and focused.

---

## License

By contributing, you agree that your contributions are licensed under the
[MIT License](LICENSE).
