#include "objects.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY_INCREMENTER 4

int main(){
      object_t *objects[7] = {
          get_int(3),
          get_float(5.5),
          get_string("thisstring"),
          get_char('c'),
          get_bool(true),
          get_array(3),
          get_string("second")
      };

      object_t *inte = get_int(4);
      object_t *aray = get_array(3);
      set_array(aray, 1, &inte);
      set_array(objects[5], 3, &objects[1]);
      print_obj(objects[5]);
      printf("\n");
      set_array(objects[5], 3, &aray);

      print_obj(aray);
      print_obj(objects[6]);
      print_obj(objects[5]);
      print_obj(get_element(objects[5], 1));

      printf("\nPrinting all objects\n");
      for (int i = 0; i < 7; i++) {
          print_obj(objects[i]);
      }
      for (int i = 0; i < 7; i++) {
          dec_refcount(&objects[i]);
    }
    dec_refcount(&aray);
    dec_refcount(&inte);
    return 0;
}

object_t *get_object()
{
    object_t *obj = malloc(sizeof(object_t));
    if (obj == NULL) {
        fprintf(stderr, "Memory Allocation Failed!\n");
        return NULL;
    }
    obj->refcount = 1;
    return obj;
}

void inc_refcount(object_t *obj)
{
    if (obj == NULL) {
        return;
    }
    obj->refcount++;
    return;
}

void dec_refcount(object_t **obj)
{
    if (obj == NULL || *obj == NULL) {
        return;
    }
    (*obj)->refcount--;
    // If references still exist, do nothing.
    if ((*obj)->refcount == 0) {
        *obj = free_obj(*obj);
    }
}

object_t *get_array(size_t capacity)
{
    object_t *obj = get_object();
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

object_t *get_bool(bool value)
{
    object_t *obj = get_object();
    if (obj == NULL) {
        return NULL;
    }
    obj->datatype = BOOLEAN;
    if (value == 1 || value == 0) {
        obj->value.v_bool = value;
    }
    return obj;
}

object_t *get_char(char value)
{
    object_t *obj = get_object();
    if (obj == NULL) {
        return NULL;
    }
    obj->datatype = CHARACTER;
    obj->value.v_char = value;
    return obj;
}

object_t *get_float(float value)
{
    object_t *obj = get_object();
    if (obj == NULL) {
        return NULL;
    }
    obj->datatype = REAL;
    obj->value.v_float = value;
    return obj;
}

object_t *get_int(int value)
{
    object_t *obj = get_object();
    if (obj == NULL) {
        return NULL;
    }
    obj->datatype = INTEGER;
    obj->value.v_int = value;
    return obj;
}

object_t *get_string(char *str)
{
    object_t *obj = get_object();
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
            printf("len: %d\n", strlen(obj->value.v_string));
            break;
        case ARRAY:
            printf("======\n");
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
            printf("======\n");
            break;
        default:
            fprintf(stderr, "Invalid Object!\n");
    }
}

// Free an object.
/** Freeing an array recursively frees all its objects. **/
void *free_obj(object_t *obj)
{
    if (obj == NULL) {
        return NULL;
    }
    switch (obj->datatype) {
        case BOOLEAN:
        case CHARACTER:
        case INTEGER:
        case REAL:
            break;
        case STRING:
            free(obj->value.v_string);
            break;
        // NOTE: Handle self referencing arrays & same object more than once in the array.
        case ARRAY:
            for (int i = 0; i < obj->value.array.capacity; i++) {
                // arr was calloc'd.
                dec_refcount(&(obj->value.array.arr[i]));
            }
            free(obj->value.array.arr);
            break;
        default:
            fprintf(stderr, "Can't free an invalid oject!\n");
            return NULL;
    }
    free(obj);
    return NULL;
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

bool set_array(object_t *obj, int index, object_t **src)
{
    if (obj == NULL || src == NULL || *src == NULL) {
        fprintf(stderr, "Invalid Usage!\n");
        return false;
    } else if (obj->datatype != ARRAY) {
        fprintf(stderr, "Object is not an array!\n");
        /* NOTE: Cannot differentiate btw 
         * objects created just to pass and 
         * oridinary object that exists for other reasons too.
         */
        // dec_refcount(src); - Not required since passing such objects is prohibited.
        return false;
    } else if (index < 0 || index > obj->value.array.capacity) {
        fprintf(stderr, "Indexing out of bounds!\n");
        // dec_refcount(src);
        return false;
    } else if (index == obj->value.array.capacity) {
          // Increase the capacity of the array, if full.
          object_t **tmp = realloc(obj->value.array.arr, sizeof(object_t *) * (obj->value.array.capacity + CAPACITY_INCREMENTER));
          if (tmp == NULL) {
              fprintf(stderr, "Can't resize the array to fit the index. Aborting insertion!\n");
              // dec_refcount(src);
              return false;
          }
          obj->value.array.arr = tmp;
          obj->value.array.capacity += CAPACITY_INCREMENTER;
          // Initialize to NULL to avoid dereferencing dangling pointers while printing.
          for (int i = index + 1; i < obj->value.array.capacity; i++) {
              obj->value.array.arr[i] = NULL;
          }
    } else if (obj->value.array.arr[index] != NULL) {
              dec_refcount(&(obj->value.array.arr[index]));
              obj->value.array.len--;
    }
    obj->value.array.len++;
    obj->value.array.arr[index] = *src;
    inc_refcount(*src);
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
