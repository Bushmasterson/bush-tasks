#include "bush_tasks/core.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
  t1.created = "2026-01-01";
  t1.subtasks = {"2 litres", "skim"};

  bush_tasks::Task t2;
  t2.text = "Write tests";
  t2.priority = bush_tasks::kPriorityUrgent;
  t2.status = bush_tasks::kStatusDone;
  t2.created = "2026-01-02";

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
  t1.created = "2026-01-01";

  CHECK(bush_tasks::saveTasks({t1}, kTestFile));

  bush_tasks::Task t2;
  t2.text = "Second";
  t2.priority = bush_tasks::kPriorityLow;
  t2.status = bush_tasks::kStatusDone;
  t2.created = "2026-01-02";

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

void testLoadNormalizesInvalidValues( ) {
  removeTestFiles( );
  {
    std::ofstream f(kTestFile);
    f << R"([
      {"text": "bogus", "priority": "critical", "status": "running", "created": "2026-01-01"},
      {"text": "no fields"}
    ])";
  }
  const auto result = bush_tasks::loadTasks(kTestFile);
  CHECK(result.status == bush_tasks::LoadStatus::Ok);
  CHECK(result.tasks.size( ) == 2);

  CHECK(result.tasks[0].priority == bush_tasks::kPriorityMedium);
  CHECK(result.tasks[0].status == bush_tasks::kStatusPending);

  CHECK(result.tasks[1].priority == bush_tasks::kPriorityMedium);
  CHECK(result.tasks[1].status == bush_tasks::kStatusPending);
  CHECK(result.tasks[1].created.empty( ));

  removeTestFiles( );
}

void testSortTasks( ) {
  std::vector<bush_tasks::Task> tasks;

  bush_tasks::Task a;
  a.text = "low pending";
  a.priority = bush_tasks::kPriorityLow;
  a.status = bush_tasks::kStatusPending;
  a.created = "2026-01-01";

  bush_tasks::Task b;
  b.text = "urgent pending";
  b.priority = bush_tasks::kPriorityUrgent;
  b.status = bush_tasks::kStatusPending;
  b.created = "2026-01-02";

  bush_tasks::Task c;
  c.text = "done";
  c.priority = bush_tasks::kPriorityUrgent;
  c.status = bush_tasks::kStatusDone;
  c.created = "2026-01-03";

  bush_tasks::Task d;
  d.text = "high postponed";
  d.priority = bush_tasks::kPriorityHigh;
  d.status = bush_tasks::kStatusPostponed;
  d.created = "2026-01-04";

  tasks = {a, c, d, b};
  bush_tasks::sortTasks(tasks);

  CHECK(tasks[0].text == "urgent pending");
  CHECK(tasks[1].text == "low pending");
  CHECK(tasks[2].text == "high postponed");
  CHECK(tasks[3].text == "done");
}

void testSortSameStatusAndPriorityByDate( ) {
  std::vector<bush_tasks::Task> tasks;

  bush_tasks::Task older;
  older.text = "older";
  older.priority = bush_tasks::kPriorityMedium;
  older.status = bush_tasks::kStatusPending;
  older.created = "2026-01-01";

  bush_tasks::Task newer;
  newer.text = "newer";
  newer.priority = bush_tasks::kPriorityMedium;
  newer.status = bush_tasks::kStatusPending;
  newer.created = "2026-03-01";

  tasks = {older, newer};
  bush_tasks::sortTasks(tasks);

  CHECK(tasks[0].text == "newer");
  CHECK(tasks[1].text == "older");
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
  CHECK(date.size( ) == 10); // yyyy-mm-dd
  CHECK(date[4] == '-');
  CHECK(date[7] == '-');
}

} // namespace

int main( ) {
  testLoadMissingFile( );
  testSaveEmptyList( );
  testSaveLoadRoundTrip( );
  testLoadCorruptFile( );
  testLoadNonArrayRoot( );
  testAtomicSaveCreatesBackup( );
  testLoadNormalizesInvalidValues( );
  testSortTasks( );
  testSortSameStatusAndPriorityByDate( );
  testPriorityValidation( );
  testStatusValidation( );
  testCurrentDateFormat( );

  removeTestFiles( );

  std::cout << "All tests passed.\n";
  return 0;
}
