#include "bush_tasks/core.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace {

const std::string kTestFile = "test_tasks.json";
const std::string kTestTmp = kTestFile + ".tmp";
const std::string kTestBak = kTestFile + ".bak";

#define CHECK(cond)                                                                                                    \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      std::cerr << "FAILED: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n";                                     \
      std::exit(1);                                                                                                    \
    }                                                                                                                  \
  } while (0)

void removeTestFiles( ) {
  std::remove(kTestFile.c_str( ));
  std::remove(kTestTmp.c_str( ));
  std::remove(kTestBak.c_str( ));
}

void testLoadMissingFile( ) {
  removeTestFiles( );
  const auto result = bush_tasks::loadTasks(kTestFile);
  CHECK(result.status == bush_tasks::LoadStatus::Missing);
  CHECK(result.tasks.empty( ));
}

void testSaveEmptyList( ) {
  removeTestFiles( );
  CHECK(bush_tasks::saveTasks({ }, kTestFile));
  const auto result = bush_tasks::loadTasks(kTestFile);
  CHECK(result.status == bush_tasks::LoadStatus::Ok);
  CHECK(result.tasks.empty( ));
}

void testSaveLoadRoundTrip( ) {
  removeTestFiles( );

  bush_tasks::Task t1;
  t1.text = "Buy milk";
  t1.priority = bush_tasks::kPriorityHigh;
  t1.status = bush_tasks::kStatusPending;
  t1.created = "01.01.26";
  t1.subtasks = {"2 litres", "skim"};

  bush_tasks::Task t2;
  t2.text = "Write tests";
  t2.priority = bush_tasks::kPriorityUrgent;
  t2.status = bush_tasks::kStatusDone;
  t2.created = "02.01.26";

  CHECK(bush_tasks::saveTasks({t1, t2}, kTestFile));

  const auto result = bush_tasks::loadTasks(kTestFile);
  CHECK(result.status == bush_tasks::LoadStatus::Ok);

  const auto& loaded = result.tasks;
  CHECK(loaded.size( ) == 2);
  CHECK(loaded[0].text == t1.text);
  CHECK(loaded[0].priority == t1.priority);
  CHECK(loaded[0].status == t1.status);
  CHECK(loaded[0].subtasks == t1.subtasks);
  CHECK(loaded[1].text == t2.text);
  CHECK(loaded[1].priority == t2.priority);
  CHECK(loaded[1].status == t2.status);
}

void testLoadCorruptFile( ) {
  removeTestFiles( );
  {
    std::ofstream f(kTestFile);
    f << "{ this is not valid json";
  }
  const auto result = bush_tasks::loadTasks(kTestFile);
  CHECK(result.status == bush_tasks::LoadStatus::Corrupt);
  CHECK(result.tasks.empty( ));
  removeTestFiles( );
}

void testLoadNonArrayRoot( ) {
  removeTestFiles( );
  {
    std::ofstream f(kTestFile);
    f << R"({"not": "an array"})";
  }
  const auto result = bush_tasks::loadTasks(kTestFile);
  CHECK(result.status == bush_tasks::LoadStatus::Corrupt);
  CHECK(result.tasks.empty( ));
  removeTestFiles( );
}

void testAtomicSaveCreatesBackup( ) {
  removeTestFiles( );

  bush_tasks::Task t1;
  t1.text = "First";
  t1.priority = bush_tasks::kPriorityMedium;
  t1.status = bush_tasks::kStatusPending;
  t1.created = "01.01.26";

  CHECK(bush_tasks::saveTasks({t1}, kTestFile));

  bush_tasks::Task t2;
  t2.text = "Second";
  t2.priority = bush_tasks::kPriorityLow;
  t2.status = bush_tasks::kStatusDone;
  t2.created = "02.01.26";

  CHECK(bush_tasks::saveTasks({t1, t2}, kTestFile));

  // After second save, a .bak with the first version must exist.
  const auto backup = bush_tasks::loadTasks(kTestBak);
  CHECK(backup.status == bush_tasks::LoadStatus::Ok);
  CHECK(backup.tasks.size( ) == 1);
  CHECK(backup.tasks[0].text == "First");

  // And the main file must hold the second version.
  const auto current = bush_tasks::loadTasks(kTestFile);
  CHECK(current.status == bush_tasks::LoadStatus::Ok);
  CHECK(current.tasks.size( ) == 2);
  CHECK(current.tasks[1].text == "Second");

  removeTestFiles( );
}

void testPriorityValidation( ) {
  CHECK(bush_tasks::isValidPriority("low"));
  CHECK(bush_tasks::isValidPriority("medium"));
  CHECK(bush_tasks::isValidPriority("high"));
  CHECK(bush_tasks::isValidPriority("urgent"));
  CHECK(!bush_tasks::isValidPriority("critical"));
  CHECK(!bush_tasks::isValidPriority(""));
}

void testStatusValidation( ) {
  CHECK(bush_tasks::isValidStatus("pending"));
  CHECK(bush_tasks::isValidStatus("done"));
  CHECK(bush_tasks::isValidStatus("postponed"));
  CHECK(!bush_tasks::isValidStatus("running"));
  CHECK(!bush_tasks::isValidStatus(""));
}

void testCurrentDateFormat( ) {
  const std::string date = bush_tasks::currentDate( );
  CHECK(date.size( ) == 8); // dd.mm.yy
  CHECK(date[2] == '.');
  CHECK(date[5] == '.');
}

} // namespace

int main( ) {
  testLoadMissingFile( );
  testSaveEmptyList( );
  testSaveLoadRoundTrip( );
  testLoadCorruptFile( );
  testLoadNonArrayRoot( );
  testAtomicSaveCreatesBackup( );
  testPriorityValidation( );
  testStatusValidation( );
  testCurrentDateFormat( );

  removeTestFiles( );

  std::cout << "All tests passed.\n";
  return 0;
}
