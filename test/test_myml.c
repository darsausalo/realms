#include "motor.h"
#include <minctest.h>

static const char* src1 = "entity_base:\n\
  health:\n\
    max: 100\n\
  speed:\n\
    max: 101\n\
    accel: 11\n\
  tag1:\n\
\n\
entity1:\n\
  inherits: entity_base\n\
  health:\n\
    max: 200\n\
  tag2:\n\
  tst1: x1\n\
\n\
entity2:\n\
  inherits@1: entity_base\n\
  inherits@2: entity1\n\
  speed:\n\
    max: 300\n\
  tag3:\n\
";

static const char* src2 = "entity_base:\n\
  tag4:\n\
\n\
entity1:\n\
  health:\n\
    max: 215\n\
\n\
entity2:\n\
  speed: 111\n\
  armor:\n\
    value: 15\n\
";

void test_myml_parse() {
  myml_parse_result_t result = myml_parse(src1);
  myml_table_t* table = result.table;
  lok(table);
  lok(myml_get_subtable(table, "entity_base"));
  lok(myml_get_subtable(table, "entity_base"));
  lok(myml_get_subtable(table, "entity1"));
  lok(myml_get_subtable(table, "entity2"));

  lsequal("100", myml_find_string(table, "entity_base.health.max"));
  lsequal("101", myml_find_string(table, "entity_base.speed.max"));
  lsequal("11", myml_find_string(table, "entity_base.speed.accel"));

  lsequal("entity_base", myml_find_string(table, "entity1.inherits"));
  lsequal("200", myml_find_string(table, "entity1.health.max"));
  lsequal("x1", myml_find_string(table, "entity1.tst1"));

  lsequal("entity_base", myml_find_string(table, "entity2.inherits@1"));
  lsequal("entity1", myml_find_string(table, "entity2.inherits@2"));
  lsequal("300", myml_find_string(table, "entity2.speed.max"));

  myml_free(table);
}

void test_myml_merge() {
  myml_table_t* table1 = myml_parse(src1).table;
  lok(table1);
  lok(!myml_find_subtable(table1, "entity_base.tag4"));

  myml_table_t* table2 = myml_parse(src2).table;
  lok(table2);

  myml_merge(table1, table2);
  myml_free(table2);

  lsequal("100", myml_find_string(table1, "entity_base.health.max"));
  lsequal("101", myml_find_string(table1, "entity_base.speed.max"));
  lsequal("11", myml_find_string(table1, "entity_base.speed.accel"));
  lok(myml_find_subtable(table1, "entity_base.tag4"));

  lsequal("entity_base", myml_find_string(table1, "entity1.inherits"));
  lsequal("215", myml_find_string(table1, "entity1.health.max"));
  lsequal("x1", myml_find_string(table1, "entity1.tst1"));

  lsequal("entity_base", myml_find_string(table1, "entity2.inherits@1"));
  lsequal("entity1", myml_find_string(table1, "entity2.inherits@2"));
  lsequal("111", myml_find_string(table1, "entity2.speed"));
  lok(myml_find_subtable(table1, "entity2.tag3"));
  lsequal("15", myml_find_string(table1, "entity2.armor.value"));

  myml_free(table1);
}

void test_myml_take_inherit() {
  myml_table_t* table1 = myml_parse(src1).table;
  lok(table1);
  lok(!myml_find_subtable(table1, "entity_base.tag4"));

  myml_table_t* table2 = myml_parse(src2).table;
  lok(table2);

  myml_merge(table1, table2);
  myml_free(table2);

  myml_error_t error = myml_take_inherit(table1);

  lok(!error.message);

  lsequal("100", myml_find_string(table1, "entity_base.health.max"));
  lsequal("101", myml_find_string(table1, "entity_base.speed.max"));
  lsequal("11", myml_find_string(table1, "entity_base.speed.accel"));
  lok(myml_find_subtable(table1, "entity_base.tag4"));

  lok(!myml_find_string(table1, "entity1.inherits"));
  lsequal("215", myml_find_string(table1, "entity1.health.max"));
  lsequal("x1", myml_find_string(table1, "entity1.tst1"));
  lsequal("101", myml_find_string(table1, "entity1.speed.max"));
  lsequal("11", myml_find_string(table1, "entity1.speed.accel"));
  lok(myml_find_subtable(table1, "entity1.tag1"));
  lok(myml_find_subtable(table1, "entity1.tag2"));
  lok(myml_find_subtable(table1, "entity1.tag4"));

  lok(!myml_find_string(table1, "entity2.inherits@1"));
  lok(!myml_find_string(table1, "entity2.inherits@2"));
  lsequal("111", myml_find_string(table1, "entity2.speed"));
  lsequal("15", myml_find_string(table1, "entity2.armor.value"));
  lok(myml_find_subtable(table1, "entity2.tag1"));
  lok(myml_find_subtable(table1, "entity2.tag2"));
  lok(myml_find_subtable(table1, "entity2.tag3"));
  lok(myml_find_subtable(table1, "entity2.tag4"));
  lsequal("215", myml_find_string(table1, "entity2.health.max"));

  myml_free(table1);
}

int main(int argc, char** argv) {
  lrun("myml_parse", test_myml_parse);
  lrun("myml_merge", test_myml_merge);
  lrun("myml_take_inherit", test_myml_take_inherit);
  lresults();
  return lfails != 0;
}
