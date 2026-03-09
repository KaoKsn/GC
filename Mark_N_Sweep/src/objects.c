#include "objects.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

object_t *get_obj(vm_t *vm)
{
    object_t *obj = calloc(1, sizeof(object_t));
    if (obj == NULL) {
        fprintf(stderr, "Memory Allocation for the object FAILED!\n");
        return NULL;
    }
    vm_track_object(vm, obj);
    obj->is_marked = false;
    return obj;
}

object_t *get_array(vm_t *vm, size_t capacity)
{
    object_t *obj = get_obj(vm);
    if (obj == NULL) {
        return NULL;
    }
    
    obj->value.array.arr = calloc(capacity, sizeof(object_t *));
    if (obj->value.array.arr == NULL) {
        fprintf(stderr, "Failed Initializing an array of the requested size!\n");
        free(obj);
        return NULL;
    }
    // Initializing the array.
    obj->datatype = ARRAY;
    obj->value.array.capacity = capacity;
    obj->value.array.len = 0;
    return obj;
}

object_t *get_bool(vm_t *vm, bool value)
{
    object_t *obj = get_obj(vm);
    if (obj == NULL) {
        return NULL;
    }
    obj->datatype = BOOLEAN;
    obj->value.v_bool = value;
    return obj;
}

object_t *get_char(vm_t *vm, char value)
{
    object_t *obj = get_obj(vm);
    if (obj == NULL) {
        return NULL;
    }
    obj->datatype = CHARACTER;
    obj->value.v_char = value;
    return obj;
}

object_t *get_float(vm_t *vm, float value)
{
    object_t *obj = get_obj(vm);
    if (obj == NULL) {
        return NULL;
    }
    obj->datatype = REAL;
    obj->value.v_float = value;
    return obj;
}

object_t *get_int(vm_t *vm, int value)
{
    object_t *obj = get_obj(vm);
    if (obj == NULL) {
        return NULL;
    }
    obj->datatype = INTEGER;
    obj->value.v_int = value;
    return obj;
}

object_t *get_string(vm_t *vm, char *str)
{
    object_t *obj = get_obj(vm);
    if (obj == NULL) {
        return NULL;
    } else if (str == NULL) {
          fprintf(stderr, "Pushing NULL as char* dectected!\nAborting object creation...");
          free(obj);
          return NULL;     
    }
    int len_str = strlen(str);
    char *string = calloc(len_str + 1, sizeof(char));
    if (string == NULL) {
        fprintf(stderr, "Mem Allocation for string failed!\n");
        free(obj);
        return NULL;
    }
    strncpy(string, str, len_str);
    obj->datatype = STRING;
    obj->value.v_string = string;
    return obj;
}

// Print the object as a string.
void print_obj(object_t *obj)
{
    if (obj == NULL) {
        fprintf(stderr, "NULL object!\n");
        return;
    }

    switch (obj->datatype) {
        case BOOLEAN:
            printf("[BOOLEAN]\n");
            switch (obj->value.v_bool) {
                case 0:
                    printf("false\n");
                    break;
                case 1:
                    printf("true\n");
                    break;
            }
            break;
        case CHARACTER:
            printf("[CHARACTER]\n");
            printf("%c\n", obj->value.v_char);
            break;
        case INTEGER:
            printf("[INTEGER]\n");
            printf("%d\n", obj->value.v_int);
            break;
        case REAL:
            printf("[FLOAT]\n");
            printf("%.4f\n", obj->value.v_float);
            break;
        case STRING:
            printf("[STRING]\n");
            printf("%s\n", obj->value.v_string);
            printf("len: %ld\n", strlen(obj->value.v_string));
            break;
        case ARRAY:
            printf("[ARRAY]\n");
            printf("capacity: %d\n", obj->value.array.capacity);
            printf("len: %d\n", obj->value.array.len);
            for (int i = 0; i < obj->value.array.capacity; i++) {
                printf("\nIndex: %d\n", i);
                if (obj->value.array.arr[i] == NULL) {
                    printf("Nothing at this index.\n");    
                    continue;
                }
                print_obj(obj->value.array.arr[i]);
            }
            break;
        default:
            fprintf(stderr, "Invalid Object!\n");
    }
}

// Free an object.
/** Freeing an array recursively frees all its objects. **/
void free_obj(object_t *obj)
{
    if (obj == NULL) {
        return;
    }
    switch (obj->datatype) {
        case BOOLEAN:
        case CHARACTER:
        case INTEGER:
        case REAL:
            break;
        case STRING:
            // Just free the heap allocated to char * i.e v_string.
            free(obj->value.v_string);
            break;
        case ARRAY:
            // Just free the void ** holding elements and not the objects itself.
            free(obj->value.array.arr);
            break;
        default:
            fprintf(stderr, "Can't free an invalid oject!\n");
            return;
    }
    free(obj);
}

// Object len, only defined for strings and arrays.
int len(object_t *obj)
{
    if (obj == NULL) {
        return -1;
    }
    switch (obj->datatype) {
        case CHARACTER:
        case INTEGER:
        case REAL:
        case BOOLEAN: return 1;
        case STRING: return strlen(obj->value.v_string);
        case ARRAY: return obj->value.array.len; // return current length.
        default: return -1;
    }
}

bool set_array(object_t *obj, int index, object_t *src)
{
    if (obj == NULL || src == NULL) {
        fprintf(stderr, "Invalid Usage!\n");
        return false;
    } else if (obj->datatype != ARRAY) {
        fprintf(stderr, "Object is not an array!\n");
        return false;
    } else if (index < 0 || index > obj->value.array.capacity) {
        fprintf(stderr, "Indexing out of bounds!\n");
        return false;
    } else if (index == obj->value.array.capacity) {
          // Increase the capacity of the array, if full.
          object_t **tmp = realloc(obj->value.array.arr, sizeof(object_t *) * (obj->value.array.capacity + ARRAY_SIZE_INCREMENT));
          if (tmp == NULL) {
              fprintf(stderr, "Can't resize the array to fit the index. Aborting insertion!\n");
              return false;
          }
          obj->value.array.arr = tmp;
          obj->value.array.capacity += ARRAY_SIZE_INCREMENT;
          // Initialize to NULL to avoid dereferencing dangling pointers while printing.
          for (int i = index + 1; i < obj->value.array.capacity; i++) {
              obj->value.array.arr[i] = NULL;
          }
          obj->value.array.arr[index] = src;
          obj->value.array.len++;
    } else {
          if (obj->value.array.arr[index] == NULL) {
              obj->value.array.len++;
          }
          obj->value.array.arr[index] = src;
          // Leave the replaced object as is.
          // The GC will free it if it is not referenced anywhere else.
    }
    return true;
}

object_t *get_element(object_t *obj, int index)
{
    if (obj == NULL || obj->datatype != ARRAY) {
        fprintf(stderr, "Invalid Object! Can't index into it!\n");
        return NULL;
    } else if (index >= obj->value.array.capacity || index < 0) {
        fprintf(stderr, "Can't index out of bounds!\n");
        return NULL;
    } else if (obj->value.array.arr[index] == NULL) {
        fprintf(stderr, "Nothing at this index!\n");
    }
    return obj->value.array.arr[index];
}
