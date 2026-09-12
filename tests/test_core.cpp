#include "bush_tasks/core.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

const std::string kTestFile = "test_tasks.json";

#define CHECK(cond)                                                                                                    \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      std::cerr << "FAILED: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n";                                     \
      std::exit(1);                                                                                                    \
    }                                                                                                                  \
  } while (0)

void removeTestFile( ) {
  std::remove(kTestFile.c_str( ));
}

void testLoadMissingFile( ) {
  removeTestFile( );
  const auto tasks = bush_tasks::loadTasks(kTestFile);
  CHECK(tasks.empty( ));
}

void testSaveEmptyList( ) {
  removeTestFile( );
  CHECK(bush_tasks::saveTasks({ }, kTestFile));
  const auto tasks = bush_tasks::loadTasks(kTestFile);
  CHECK(tasks.empty( ));
}

void testSaveLoadRoundTrip( ) {
  removeTestFile( );

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

  const auto loaded = bush_tasks::loadTasks(kTestFile);
  CHECK(loaded.size( ) == 2);
  CHECK(loaded[0].text == t1.text);
  CHECK(loaded[0].priority == t1.priority);
  CHECK(loaded[0].status == t1.status);
  CHECK(loaded[0].subtasks == t1.subtasks);
  CHECK(loaded[1].text == t2.text);
  CHECK(loaded[1].priority == t2.priority);
  CHECK(loaded[1].status == t2.status);
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
  testPriorityValidation( );
  testStatusValidation( );
  testCurrentDateFormat( );

  removeTestFile( );

  std::cout << "All tests passed.\n";
  return 0;
}
