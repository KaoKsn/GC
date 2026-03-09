#ifndef OBJECTS_H
#define OBJECTS_H

#define ARRAY_SIZE_INCREMENT 4
typedef struct Object object_t;

#include <stdbool.h>
#include <stddef.h>
#include "vm.h"

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
    datatype_t datatype;
    value_t value;
    bool is_marked;
} object_t;

object_t *get_obj(vm_t *vm);
object_t *get_array(vm_t *vm, size_t capacity);
object_t *get_bool(vm_t *vm, bool value);
object_t *get_char(vm_t *vm, char value);
object_t *get_float(vm_t *vm, float value);
object_t *get_int(vm_t *vm, int value);
object_t *get_string(vm_t *vm, char *str);
void print_obj(object_t *obj); 
void free_obj(object_t *obj);

int len(object_t *obj);
bool set_array(object_t *obj, int index, object_t *src_obj);
object_t *get_element(object_t *obj, int index);

#endif
