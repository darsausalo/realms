#ifndef MYML_H
#define MYML_H

typedef struct myml_value_t myml_value_t;
typedef struct myml_table_t myml_table_t;

typedef struct myml_iter_t {
    myml_value_t* cur;
    myml_value_t* end;
} myml_iter_t;

typedef struct myml_error_t {
    int         line;
    int         column;
    const char* message;
} myml_error_t;

/* Alloc/free */
myml_table_t* myml_alloc();
void          myml_free(myml_table_t* table);

/* */
myml_value_t* myml_get_value(myml_table_t* table, const char* key);
const char*   myml_get_string(myml_table_t* table, const char* key);
myml_table_t* myml_get_subtable(myml_table_t* table, const char* key);
void          myml_get_location(myml_value_t* value, int* line, int* column);

const char*   myml_find_string(const myml_table_t* table, const char* key_path);
myml_table_t* myml_find_subtable(const myml_table_t* table,
                                 const char*         key_path);

myml_value_t* myml_set_string(myml_table_t* table, const char* key,
                              const char* string);
myml_value_t* myml_set_path_string(myml_table_t* table, const char* key_path,
                                   const char* string);

myml_iter_t myml_iter(myml_table_t* table);
bool        myml_end(myml_iter_t it);
void        myml_next(myml_iter_t* it);

const char*   myml_it_key(myml_iter_t it);
const char*   myml_it_string(myml_iter_t it);
myml_table_t* myml_it_subtable(myml_iter_t it);

myml_error_t myml_parse(myml_table_t* table, const char* source);

void         myml_merge(myml_table_t* dst, myml_table_t* src);
myml_error_t myml_take_inherit(myml_table_t* table);

#endif //MYML_H
