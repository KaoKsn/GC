#ifndef VM_H
#define VM_H

typedef struct VM vm_t;
#include "stack.h"
#include "objects.h"
#include <stdlib.h>

// A VM is a container that has stack of frames and stack of objects.
typedef struct VM {
    // frames is a stack of stacks.
    stack_t *frames;
    stack_t *objects;
} vm_t;

// A frame is a stack of references.
typedef struct Frame {
    // Objects that a frame references.
    stack_t *references;
} frame_t;

vm_t *create_vm();
void free_vm(vm_t *vm);
// A wrapper function to push only frames into the vm.
// Avoid direct pushing into the frames stack.
void vm_push_frame(vm_t *vm, frame_t *frame);
frame_t *get_frame(vm_t *vm);
void free_frame(frame_t *frame);

void vm_track_object(vm_t *vm, object_t *obj);
void frame_reference_obj(frame_t *frame, object_t *obj);

void mark(vm_t *vm);
void trace(vm_t *vm);
void trace_blacken_object(stack_t *grey_objects, object_t *obj);
void trace_mark_object(stack_t *grey_objects, object_t *obj);
void sweep(vm_t *vm);
void run_gc(vm_t *vm);
#endif
