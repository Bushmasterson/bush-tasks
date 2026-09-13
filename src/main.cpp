#include "bush_tasks/cli.h"
#include "bush_tasks/config.h"
#include "bush_tasks/core.h"
#include "bush_tasks/i18n.h"
#include "bush_tasks/version.h.in"

#include <cstring>
#include <iostream>
#include <string>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

struct CliOptions {
  bool showVersion = false;
  bool showHelp = false;
  bool valid = true;
  std::string language;
};

void printVersion( ) {
  std::cout << "bush-tasks " << BUSH_TASKS_VERSION << '\n';
}

void printUsage( ) {
  using bush_tasks::tr;
  std::cout << tr("main.usage_header") << "\n\n"
            << tr("main.options_header") << "\n"
            << tr("main.opt_help") << "\n"
            << tr("main.opt_version") << "\n"
            << tr("main.opt_language") << "\n\n"
            << tr("main.commands_header") << "\n"
            << "  add <text>\n"
            << "  del <number>\n"
            << "  edit <number> <text>\n"
            << "  sub <number> <text>\n"
            << "  priority <number> <low|medium|high|urgent>\n"
            << "  status <number> <pending|done|postponed>\n"
            << "  tasks\n"
            << "  settings [language <code>]\n"
            << "  clear\n"
            << "  exit\n";
}

void enableUtf8Console( ) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif
}

CliOptions parseArguments(int argc, char** argv) {
  CliOptions opt;
  for (int i = 1; i < argc; ++i) {
    const char* arg = argv[i];
    if (std::strcmp(arg, "-v") == 0 || std::strcmp(arg, "--version") == 0) {
      opt.showVersion = true;
      continue;
    }
    if (std::strcmp(arg, "-h") == 0 || std::strcmp(arg, "--help") == 0) {
      opt.showHelp = true;
      continue;
    }
    if (std::strcmp(arg, "-l") == 0 || std::strcmp(arg, "--language") == 0) {
      if (i + 1 >= argc) {
        std::cerr << bush_tasks::tr("main.unknown_option", {{"option", arg}}) << "\n\n";
        opt.valid = false;
        return opt;
      }
      opt.language = argv[++i];
      continue;
    }
    std::cerr << bush_tasks::tr("main.unknown_option", {{"option", arg}}) << "\n\n";
    opt.valid = false;
    return opt;
  }
  return opt;
}

} // namespace

int main(int argc, char** argv) {
  enableUtf8Console( );

  // Load built-in locales first so that parseArguments() can already use tr().
  bush_tasks::initI18n( );

  // Apply persisted language, then any explicit CLI override.
  const bush_tasks::Config cfg = bush_tasks::loadConfig( );
  if (!cfg.language.empty( )) {
    bush_tasks::setLanguage(cfg.language);
  }

  const CliOptions opt = parseArguments(argc, argv);

  if (!opt.valid) {
    printUsage( );
    return 2;
  }

  if (!opt.language.empty( )) {
    if (!bush_tasks::setLanguage(opt.language)) {
      std::cerr << bush_tasks::tr("settings.unknown_language", {{"code", opt.language}}) << "\n";
      return 2;
    }
  }

  if (opt.showVersion) {
    printVersion( );
    return 0;
  }
  if (opt.showHelp) {
    printUsage( );
    return 0;
  }

  const std::string filePath = bush_tasks::configDirectory( ) + "/tasks.json";
  auto loadResult = bush_tasks::loadTasks(filePath);

  if (loadResult.status == bush_tasks::LoadStatus::Corrupt) {
    std::cerr << bush_tasks::tr("main.load_corrupt", {{"path", filePath}}) << "\n";
  }

  auto tasks = std::move(loadResult.tasks);

  while (true) {
    bush_tasks::renderHeader( );
    bush_tasks::renderTasks(tasks);
    bush_tasks::renderHelp( );

    std::string input;
    if (!std::getline(std::cin, input)) {
      break; // EOF (Ctrl+D / Ctrl+Z then Enter)
    }

    if (bush_tasks::handleCommand(input, tasks)) {
      break;
    }

    if (!bush_tasks::saveTasks(tasks, filePath)) {
      std::cerr << bush_tasks::tr("main.save_warning", {{"path", filePath}}) << "\n";
    }
  }

  return 0;
}
