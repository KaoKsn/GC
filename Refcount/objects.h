#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct Object object_t;

typedef enum Datatype
{
    BOOLEAN,
    ARRAY,
    CHARACTER,
    REAL,
    INTEGER,
    STRING,
} datatype_t;

typedef union Value
{
    struct Array {
        object_t **arr;
        int capacity;
        int len;
    } array;
    float v_float;
    int v_int;
    char v_char;
    char *v_string;
    bool v_bool;
} value_t;

typedef struct Object
{
    int refcount;
    datatype_t datatype;
    value_t value;
} object_t;

object_t *get_object();
object_t *get_array(size_t capacity);
object_t *get_bool(bool value);
object_t *get_char(char value);
object_t *get_float(float value);
object_t *get_int(int value);
object_t *get_string(char *str);
void print_obj(object_t *obj); 
void *free_obj(object_t *obj);

int len(object_t *obj);
bool set_array(object_t *obj, int index, object_t **src_obj);
object_t *get_element(object_t *obj, int index);

void inc_refcount(object_t *obj);
void dec_refcount(object_t **obj);
